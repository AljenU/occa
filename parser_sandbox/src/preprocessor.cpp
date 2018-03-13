/* The MIT License (MIT)
 *
 * Copyright (c) 2014-2018 David Medina and Tim Warburton
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 */
#include <sstream>
#include <stdlib.h>

#include "occa/tools/hash.hpp"
#include "occa/tools/io.hpp"
#include "occa/tools/lex.hpp"
#include "occa/tools/string.hpp"
#include "occa/parser/primitive.hpp"

#include "preprocessor.hpp"
#include "specialMacros.hpp"
#include "expression.hpp"
#include "tokenizer.hpp"

namespace occa {
  namespace lang {
    namespace ppStatus {
      const int reading    = (1 << 0);
      const int ignoring   = (1 << 1);
      const int foundIf    = (1 << 2);
      const int foundElse  = (1 << 3);
      const int finishedIf = (1 << 4);
    }

    // TODO: Add actual compiler macros as well
    preprocessor_t::preprocessor_t() {
      init();
      initDirectives();
    }

    preprocessor_t::preprocessor_t(const preprocessor_t &pp) :
      withInputCache(pp),
      withOutputCache(pp) {
      *this = pp;
    }

    preprocessor_t::~preprocessor_t() {
      clear_();
    }

    void preprocessor_t::init() {
      pushStatus(ppStatus::reading);
      // Always start off as if we passed a newline
      incrementNewline();
      expandingMacros = true;

      compilerMacros.autoFreeze = false;

      const int specialMacroCount = 7;
      macro_t *specialMacros[specialMacroCount] = {
        new definedMacro(*this),    // defined()
        new hasIncludeMacro(*this), // __has_include()
        new fileMacro(*this),       // __FILE__
        new lineMacro(*this),       // __LINE__
        new dateMacro(*this),       // __DATE__
        new timeMacro(*this),       // __TIME__
        new counterMacro(*this)     // __COUNTER__
      };
      for (int i = 0; i < specialMacroCount; ++i) {
        compilerMacros.add(specialMacros[i]->name(), specialMacros[i]);
      }

      // Alternative representations
      compilerMacros.add("and"   , macro_t::defineBuiltin(*this, "and"   , "&&"));
      compilerMacros.add("and_eq", macro_t::defineBuiltin(*this, "and_eq", "&="));
      compilerMacros.add("bitand", macro_t::defineBuiltin(*this, "bitand", "&"));
      compilerMacros.add("bitor" , macro_t::defineBuiltin(*this, "bitor" , "|"));
      compilerMacros.add("compl" , macro_t::defineBuiltin(*this, "compl" , "~"));
      compilerMacros.add("not"   , macro_t::defineBuiltin(*this, "not"   , "!"));
      compilerMacros.add("not_eq", macro_t::defineBuiltin(*this, "not_eq", "!="));
      compilerMacros.add("or"    , macro_t::defineBuiltin(*this, "or"    , "||"));
      compilerMacros.add("or_eq" , macro_t::defineBuiltin(*this, "or_eq" , "|="));
      compilerMacros.add("xor"   , macro_t::defineBuiltin(*this, "xor"   , "^"));
      compilerMacros.add("xor_eq", macro_t::defineBuiltin(*this, "xor_eq", "^="));

      compilerMacros.freeze();
      compilerMacros.autoFreeze = true;

      warnings = 0;
      errors   = 0;
    }

    void preprocessor_t::clear() {
      clear_();
      init();
    }

    void preprocessor_t::clear_() {
      errors   = 0;
      warnings = 0;

      while (inputCache.size()) {
        delete inputCache.front();
        inputCache.pop_front();
      }
      while (outputCache.size()) {
        delete outputCache.front();
        outputCache.pop_front();
      }

      statusStack.clear();

      macroTrie *tries[2] = {
        &compilerMacros,
        &sourceMacros
      };

      for (int i = 0; i < 2; ++i) {
        macroTrie &trie = *(tries[i]);
        const int trieSize = trie.size();
        for (int j = 0; j < trieSize; ++j) {
          delete trie.values[j];
        }
        trie.clear();
      }
      dependencies.clear();
    }

    preprocessor_t& preprocessor_t::operator = (const preprocessor_t &pp) {
      clear();

      statusStack     = pp.statusStack;
      status          = pp.status;
      passedNewline   = pp.passedNewline;
      expandingMacros = pp.expandingMacros;

      directives     = pp.directives;
      compilerMacros = pp.compilerMacros;
      sourceMacros   = pp.sourceMacros;

      dependencies = pp.dependencies;
      warnings     = pp.warnings;
      errors       = pp.errors;

      directives.freeze();
      directives.autoFreeze = true;

      // Copy cache
      tokenList *caches[2] = { &inputCache, &outputCache };
      const tokenList *ppCaches[2] = { &pp.inputCache, &pp.outputCache };
      for (int i = 0; i < 2; ++i) {
        tokenList &cache         = *(caches[i]);
        const tokenList &ppCache = *(ppCaches[i]);

        tokenList::const_iterator it = ppCache.begin();
        while (it != ppCache.end()) {
          cache.push_back((*it)->clone());
          ++it;
        }
      }

      // Copy macros
      compilerMacros.autoFreeze = false;
      sourceMacros.autoFreeze   = false;
      macroTrie *tries[2] = { &compilerMacros, &sourceMacros };
      const macroTrie *ppTries[2] = { &pp.compilerMacros, &pp.sourceMacros };
      for (int i = 0; i < 2; ++i) {
        macroTrie &trie         = *(tries[i]);
        const macroTrie &ppTrie = *(ppTries[i]);

        const int trieSize = ppTrie.size();
        for (int j = 0; j < trieSize; ++j) {
          const macro_t *macro = ppTrie.values[j];
          trie.add(macro->name(), &(macro->clone(*this)));
        }
      }

      compilerMacros.autoFreeze = true;
      sourceMacros.autoFreeze   = true;

      return *this;
    }

    void preprocessor_t::initDirectives() {
      directives.autoFreeze = false;
      directives.add("if"     , &preprocessor_t::processIf);
      directives.add("ifdef"  , &preprocessor_t::processIfdef);
      directives.add("ifndef" , &preprocessor_t::processIfndef);
      directives.add("elif"   , &preprocessor_t::processElif);
      directives.add("else"   , &preprocessor_t::processElse);
      directives.add("endif"  , &preprocessor_t::processEndif);

      directives.add("define" , &preprocessor_t::processDefine);
      directives.add("undef"  , &preprocessor_t::processUndef);

      directives.add("error"  , &preprocessor_t::processError);
      directives.add("warning", &preprocessor_t::processWarning);

      directives.add("include", &preprocessor_t::processInclude);
      directives.add("pragma" , &preprocessor_t::processPragma);
      directives.add("line"   , &preprocessor_t::processLine);
      directives.freeze();
      directives.autoFreeze = true;
    }

    void preprocessor_t::warningOn(token_t *token,
                                   const std::string &message) {
      ++warnings;
      token->printWarning(message);
    }

    void preprocessor_t::errorOn(token_t *token,
                                 const std::string &message) {
      ++errors;
      token->printError(message);
    }

    tokenMap& preprocessor_t::clone_() const {
      return *(new preprocessor_t(*this));
    }

    void* preprocessor_t::passMessageToInput(const occa::properties &props) {
      const std::string inputName = props.get<std::string>("inputName");
      if (inputName == "preprocessor_t") {
        return (void*) this;
      }
      if (input) {
        return input->passMessageToInput(props);
      }
      return NULL;
    }

    void preprocessor_t::pushStatus(const int status_) {
      statusStack.push_back(status);
      status = status_;
    }

    int preprocessor_t::popStatus() {
      if (statusStack.size() == 0) {
        return 0;
      }
      status = statusStack.back();
      statusStack.pop_back();
      return status;
    }

    void preprocessor_t::swapReadingStatus() {
      if (status & ppStatus::reading) {
        status &= ~ppStatus::reading;
        status |= ppStatus::ignoring;
      } else {
        status &= ~ppStatus::ignoring;
        status |= ppStatus::reading;
      }
    }

    void preprocessor_t::incrementNewline() {
      // We need to keep passedNewline 'truthy'
      //   until after the next token
      passedNewline = 2;
    }

    void preprocessor_t::decrementNewline() {
      passedNewline -= !!passedNewline;
    }

    macro_t* preprocessor_t::getMacro(const std::string &name) {
      macroTrie::result_t result = sourceMacros.get(name);
      if (result.success()) {
        return result.value();
      }
      result = compilerMacros.get(name);
      if (result.success()) {
        return result.value();
      }
      return NULL;
    }

    token_t* preprocessor_t::getSourceToken() {
      token_t *token = NULL;
      if (!inputIsEmpty()) {
        getNextInput(token);
      }
      return token;
    }

    void preprocessor_t::fetchNext() {
      processToken(getSourceToken());
    }

    void preprocessor_t::expandMacro(identifierToken &source,
                                     macro_t &macro) {
      tokenVector tokens;
      macro.expand(tokens, source);

      const int tokenCount = (int) tokens.size();
      for (int i = (tokenCount - 1); i >= 0; --i) {
        pushInput(tokens[i]);
      }
    }

    void preprocessor_t::skipToNewline() {
      tokenVector lineTokens;
      getLineTokens(lineTokens);

      // Push the newline token
      const int tokens = (int) lineTokens.size();
      if (tokens) {
        pushOutput(lineTokens[tokens-1]);
        lineTokens.pop_back();
      }

      freeTokenVector(lineTokens);
    }

    // lineTokens might be partially initialized
    //   so we don't want to clear it
    void preprocessor_t::getLineTokens(tokenVector &lineTokens) {
      while (!inputIsEmpty()) {
        token_t *token = getSourceToken();

        if (token->type() & tokenType::newline) {
          incrementNewline();
          lineTokens.push_back(token);
          break;
        }

        lineTokens.push_back(token);
      }
    }

    // lineTokens might be partially initialized
    //   so we don't want to clear it
    void preprocessor_t::getExpandedLineTokens(tokenVector &lineTokens) {
      // Make sure we don't ignore these tokens
      int oldStatus = status;
      status = ppStatus::reading;

      while (!inputIsEmpty()) {
        token_t *token;
        (*this) >> token;

        if (token->type() & tokenType::newline) {
          incrementNewline();
          lineTokens.push_back(token);
          status = oldStatus;
          break;
        }

        lineTokens.push_back(token);
      }
      status = oldStatus;
    }

    void preprocessor_t::warnOnNonEmptyLine(const std::string &message) {
      tokenVector lineTokens;
      getLineTokens(lineTokens);
      if (lineTokens.size()) {
        // Don't account for the newline token
        if (lineTokens[0]->type() != tokenType::newline) {
          warningOn(lineTokens[0],
                    message);
        }
        freeTokenVector(lineTokens);
      }
    }

    void preprocessor_t::processToken(token_t *token) {
      decrementNewline();

      const int tokenType = token->type();

      if (tokenType & tokenType::newline) {
        incrementNewline();
        pushOutput(token);
        return;
      }

      // Only process operators when ignoring
      //   for potential #
      if (status & ppStatus::ignoring) {
        if (!(tokenType & tokenType::op)) {
          delete token;
          return;
        }
        if (!(token->getOpType() & operatorType::preprocessor)) {
          delete token;
          return;
        }
      }

      if (tokenType & tokenType::identifier) {
        processIdentifier(token->to<identifierToken>());
      }
      else if (tokenType & tokenType::op) {
        processOperator(token->to<operatorToken>());
      }
      else {
        pushOutput(token);
      }
    }

    void preprocessor_t::processIdentifier(identifierToken &token) {
      // Ignore tokens inside disabled #if/#elif/#else regions
      if (status & ppStatus::ignoring) {
        delete &token;
        return;
      }

      macro_t *macro = (expandingMacros
                        ? getMacro(token.value)
                        : NULL);
      if (macro) {
        if (!macro->isFunctionLike) {
          expandMacro(token, *macro);
          delete &token;
          return;
        }

        if (inputIsEmpty()) {
          pushOutput(&token);
          return;
        }

        // Make sure that the macro starts with a '('
        token_t *nextToken = getSourceToken();
        if (nextToken->getOpType() & operatorType::parenthesesStart) {
          expandMacro(token, *macro);
          delete &token;
          delete nextToken;
          return;
        }
        // Prioritize possible variable if no () is found:
        //   #define FOO()
        //   int FOO;
        if (nextToken) {
          pushOutput(&token);
          pushOutput(nextToken);
        } else {
          pushOutput(&token);
        }
        return;
      }

      pushOutput(&token);
    }

    void preprocessor_t::processOperator(operatorToken &token) {
      if ((token.opType() != operatorType::hash) ||
          !passedNewline) {
        pushOutput(&token);
        return;
      }
      delete &token;

      if (inputIsEmpty()) {
        return;
      }

      // NULL or an empty # is ok
      token_t *directive = getSourceToken();
      if (directive->type() & tokenType::newline) {
        incrementNewline();
        pushOutput(directive);
        return;
      }

      // Check for valid directive
      if (directive->type() != tokenType::identifier) {
        errorOn(directive,
                "Unknown preprocessor directive");
        skipToNewline();
        return;
      }

      identifierToken &directiveToken = directive->to<identifierToken>();
      const std::string &directiveStr = directiveToken.value;
      directiveTrie::result_t result  = directives.get(directiveStr);
      if (!result.success()) {
        errorOn(directive,
                "Unknown preprocessor directive");
        delete directive;
        skipToNewline();
        return;
      }

      processDirective_t processFunc = result.value();

      if ((status & ppStatus::ignoring)                   &&
          (processFunc != &preprocessor_t::processIf)     &&
          (processFunc != &preprocessor_t::processIfdef)  &&
          (processFunc != &preprocessor_t::processIfndef) &&
          (processFunc != &preprocessor_t::processElif)   &&
          (processFunc != &preprocessor_t::processElse)   &&
          (processFunc != &preprocessor_t::processEndif)) {

        delete directive;
        skipToNewline();
        return;
      }

      (this->*(processFunc))(directiveToken);

      delete directive;
    }


    bool preprocessor_t::lineIsTrue(identifierToken &directive,
                                    bool &isTrue) {
      tokenVector lineTokens;
      getExpandedLineTokens(lineTokens);

      // Remove the newline token
      if (lineTokens.size()) {
        delete lineTokens.back();
        lineTokens.pop_back();
      }

      exprNode *expr = getExpression(lineTokens);

      // Errors when expr is NULL are handled
      //   while forming the expression
      bool exprError = !expr;
      if (expr) {
        if (expr->type() & exprNodeType::empty) {
          errorOn(&directive,
                  "Expected a value or expression");
          exprError = true;
        }
        else if (!expr->canEvaluate()) {
          errorOn(&directive,
                  "Unable to evaluate expression");
          exprError = true;
        }
      }

      // Default to #if false with error
      if (exprError) {
        pushStatus(ppStatus::ignoring |
                   ppStatus::foundIf);
        return false;
      }

      isTrue = expr->evaluate();
      delete expr;

      return true;
    }

    bool preprocessor_t::getIfdef(identifierToken &directive,
                                  bool &isTrue) {
      token_t *token = getSourceToken();
      const int tokenType = token_t::safeType(token);

      if (!(tokenType & tokenType::identifier)) {
        // Print from the directive if we don't
        //   have a token in the same line
        token_t *errorToken = &directive;
        if (tokenType & tokenType::newline) {
          incrementNewline();
          pushOutput(token);
        } else if (tokenType & ~tokenType::none) {
          errorToken = token;
        }
        errorOn(errorToken,
                "Expected an identifier");
        delete token;

        // Default to false
        pushStatus(ppStatus::ignoring |
                   ppStatus::foundIf);
        return false;
      }

      const std::string &macroName = token->to<identifierToken>().value;
      isTrue = getMacro(macroName);
      delete token;

      return true;
    }

    void preprocessor_t::processIf(identifierToken &directive) {
      // Nested case
      if (status & ppStatus::ignoring) {
        skipToNewline();
        pushStatus(ppStatus::ignoring |
                   ppStatus::foundIf  |
                   ppStatus::finishedIf);
        return;
      }

      bool isTrue;
      if (!lineIsTrue(directive, isTrue)) {
        return;
      }

      pushStatus(ppStatus::foundIf | (isTrue
                                      ? ppStatus::reading
                                      : ppStatus::ignoring));
    }

    void preprocessor_t::processIfdef(identifierToken &directive) {
      // Nested case
      if (status & ppStatus::ignoring) {
        skipToNewline();
        pushStatus(ppStatus::ignoring |
                   ppStatus::foundIf  |
                   ppStatus::finishedIf);
        return;
      }

      bool isTrue;
      if (!getIfdef(directive, isTrue)) {
        return;
      }

      pushStatus(ppStatus::foundIf | (isTrue
                                      ? ppStatus::reading
                                      : ppStatus::ignoring));

      warnOnNonEmptyLine("Extra tokens after macro name");
    }

    void preprocessor_t::processIfndef(identifierToken &directive) {
      // Nested case
      if (status & ppStatus::ignoring) {
        skipToNewline();
        pushStatus(ppStatus::ignoring |
                   ppStatus::foundIf  |
                   ppStatus::finishedIf);
        return;
      }

      bool isTrue;
      if (!getIfdef(directive, isTrue)) {
        return;
      }

      pushStatus(ppStatus::foundIf | (isTrue
                                      ? ppStatus::ignoring
                                      : ppStatus::reading));

      warnOnNonEmptyLine("Extra tokens after macro name");
    }

    void preprocessor_t::processElif(identifierToken &directive) {
      // Check for errors
      if (!(status & ppStatus::foundIf)) {
        errorOn(&directive,
                "#elif without #if");
        skipToNewline();
        return;
      }
      if (status & ppStatus::foundElse) {
        errorOn(&directive,
                "#elif found after an #else directive");
        status &= ~ppStatus::reading;
        status |= (ppStatus::ignoring |
                   ppStatus::finishedIf);
        skipToNewline();
        return;
      }

      // Make sure to test #elif expression is valid
      bool isTrue;
      if (!lineIsTrue(directive, isTrue)) {
        return;
      }

      // If we already finished, keep old state
      if (status & ppStatus::finishedIf) {
        return;
      }

      if (status & ppStatus::reading) {
        swapReadingStatus();
        status |= ppStatus::finishedIf;
      } else if (isTrue) {
        status = (ppStatus::foundIf |
                  ppStatus::reading);
      }
    }

    void preprocessor_t::processElse(identifierToken &directive) {
      warnOnNonEmptyLine("Extra tokens after #else directive");

      // Test errors
      if (!(status & ppStatus::foundIf)) {
        errorOn(&directive,
                "#else without #if");
        return;
      }
      if (status & ppStatus::foundElse) {
        errorOn(&directive,
                "Two #else directives found for the same #if");
        status &= ~ppStatus::reading;
        status |= (ppStatus::ignoring |
                   ppStatus::finishedIf);
        return;
      }

      // Make sure to error on multiple #else
      status |= ppStatus::foundElse;

      // Test status cases
      if (status & ppStatus::finishedIf) {
        return;
      }

      if (status & ppStatus::reading) {
        swapReadingStatus();
        status |= ppStatus::finishedIf;
      } else {
        swapReadingStatus();
      }
    }

    void preprocessor_t::processEndif(identifierToken &directive) {
      warnOnNonEmptyLine("Extra tokens after #endif directive");

      if (!(status & ppStatus::foundIf)) {
        errorOn(&directive,
                "#endif without #if");
      } else {
        popStatus();
      }
    }

    void preprocessor_t::processDefine(identifierToken &directive) {
      token_t *token = getSourceToken();
      if (token_t::safeType(token) != tokenType::identifier) {
        if (!token || passedNewline) {
          incrementNewline();
          errorOn(&directive,
                  "Expected an identifier");
        } else {
          errorOn(token,
                  "Expected an identifier");
        }
        delete token;
        skipToNewline();
        return;
      }

      macro_t &macro = *(new macro_t(*this, token->to<identifierToken>()));
      macro.loadDefinition();

      const std::string &name = macro.name();

      // TODO: Error if the definitions aren't the same
      if (sourceMacros.has(name)) {
        delete getMacro(name);
        sourceMacros.remove(name);
      }
      sourceMacros.add(name, &macro);

      // Macro clones the token
      delete token;
    }

    void preprocessor_t::processUndef(identifierToken &directive) {
      token_t *token = getSourceToken();
      const int tokenType = token_t::safeType(token);
      if (tokenType != tokenType::identifier) {
        if (tokenType & (tokenType::none |
                         tokenType::newline)) {
          incrementNewline();
          errorOn(&directive,
                  "Expected an identifier");
        } else {
          errorOn(token,
                  "Expected an identifier");
        }
        skipToNewline();
        return;
      }
      // Remove macro
      const std::string &macroName = token->to<identifierToken>().value;
      delete getMacro(macroName);
      sourceMacros.remove(macroName);
      delete token;
    }

    void preprocessor_t::processError(identifierToken &directive) {
      tokenVector lineTokens;
      getExpandedLineTokens(lineTokens);

      const int tokenCount = (int) lineTokens.size();
      if (!tokenCount) {
        errorOn(&directive,
                "");
      }
      else {
        // Don't include the \n in the message
        const char *start = lineTokens[0]->origin.position.start;
        const char *end   = lineTokens[tokenCount - 1]->origin.position.start;
        const std::string message(start, end - start);
        errorOn(lineTokens[0],
                message);
      }

      freeTokenVector(lineTokens);
    }

    void preprocessor_t::processWarning(identifierToken &directive) {
      tokenVector lineTokens;
      getExpandedLineTokens(lineTokens);

      const int tokenCount = (int) lineTokens.size();
      if (!tokenCount) {
        warningOn(&directive,
                  "");
      }
      else {
        // Don't include the \n in the message
        const char *start = lineTokens[0]->origin.position.start;
        const char *end   = lineTokens[tokenCount - 1]->origin.position.start;
        const std::string message(start, end - start);
        warningOn(lineTokens[0],
                  message);
      }

      freeTokenVector(lineTokens);
    }

    void preprocessor_t::processInclude(identifierToken &directive) {
      // Don't cache since the input might change
      tokenizer_t *tokenizer = (tokenizer_t*) getInput("tokenizer_t");
      if (!tokenizer) {
        warningOn(&directive,
                  "Unable to apply #include due to the lack of a tokenizer");
        skipToNewline();
        return;
      }

      const std::string header = tokenizer->getHeader();

      if (!io::exists(header)) {
        errorOn(&directive,
                "File does not exist");
        skipToNewline();
        return;
      }

      dependencies[header] = true;

      tokenVector lineTokens;
      getExpandedLineTokens(lineTokens);

      if (header.size()) {
        tokenizer->pushSource(header);
      } else {
        errorOn(&directive,
                "Expected a header to include");
        freeTokenVector(lineTokens);
        return;
      }

      // Ignore the newline token
      if (lineTokens.size() > 1) {
        warningOn(lineTokens[0],
                  "Extra tokens after the #include header");
      }
      freeTokenVector(lineTokens);
    }

    void preprocessor_t::processPragma(identifierToken &directive) {
      tokenVector lineTokens;
      getExpandedLineTokens(lineTokens);

      // Remove the newline token
      if (lineTokens.size()) {
        delete lineTokens.back();
        lineTokens.pop_back();
      }

      const std::string value = stringifyTokens(lineTokens, true);

      pushOutput(new pragmaToken(directive.origin,
                                 value));

      freeTokenVector(lineTokens);
    }

    void preprocessor_t::processLine(identifierToken &directive) {
      tokenVector lineTokens;
      getExpandedLineTokens(lineTokens);

      // Don't cache since the input might change
      tokenizer_t *tokenizer = (tokenizer_t*) getInput("tokenizer_t");
      if (!tokenizer) {
        tokenizer = (tokenizer_t*) getInput("tokenizer_t");
      }
      if (!tokenizer) {
        warningOn(&directive,
                  "Unable to apply #line due to the lack of a tokenizer");
        freeTokenVector(lineTokens);
        return;
      }

      int tokenCount = (int) lineTokens.size();
      if (tokenCount <= 1) {
        token_t *source = (tokenCount
                           ? lineTokens[0]
                           : (token_t*) &directive);
        errorOn(source,
                "Expected a line number");
        freeTokenVector(lineTokens);
        return;
      }

      // Get line number
      int line = -1;
      std::string filename = tokenizer->origin.file->filename;

      token_t *lineToken = lineTokens[0];
      if (lineToken->type() & tokenType::primitive) {
        line = lineToken->to<primitiveToken>().value;
        if (line < 0) {
          errorOn(lineToken,
                  "Line number must be greater or equal to 0");
        }
      } else {
        errorOn(lineToken,
                "Expected a line number");
      }
      if (line < 0) {
        freeTokenVector(lineTokens);
        return;
      }

      // Get new filename
      if (tokenCount > 2) {
        token_t *filenameToken = lineTokens[1];
        if (filenameToken->type() & tokenType::string) {
          filename = filenameToken->to<stringToken>().value;
        } else {
          errorOn(filenameToken,
                  "Expected a filename");
          freeTokenVector(lineTokens);
          return;
        }
      }

      if (tokenCount > 3) {
        warningOn(lineTokens[2],
                  "Extra tokens are unused");
      }

      tokenizer->setOrigin(line, filename);

      freeTokenVector(lineTokens);
    }
    //====================================
  }
}
