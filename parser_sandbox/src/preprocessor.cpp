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
    preprocessor::preprocessor() :
      errorOnToken(NULL),
      directives(getDirectiveTrie()) {

      // Always start off as if we passed a newline
      incrementNewline();

      const int specialMacroCount = 7;
      compilerMacros.autoFreeze = false;
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
      compilerMacros.add("and"   , macro_t::define(*this, "and"   , "&&"));
      compilerMacros.add("and_eq", macro_t::define(*this, "and_eq", "&="));
      compilerMacros.add("bitand", macro_t::define(*this, "bitand", "&"));
      compilerMacros.add("bitor" , macro_t::define(*this, "bitor" , "|"));
      compilerMacros.add("compl" , macro_t::define(*this, "compl" , "~"));
      compilerMacros.add("not"   , macro_t::define(*this, "not"   , "!"));
      compilerMacros.add("not_eq", macro_t::define(*this, "not_eq", "!="));
      compilerMacros.add("or"    , macro_t::define(*this, "or"    , "||"));
      compilerMacros.add("or_eq" , macro_t::define(*this, "or_eq" , "|="));
      compilerMacros.add("xor"   , macro_t::define(*this, "xor"   , "^"));
      compilerMacros.add("xor_eq", macro_t::define(*this, "xor_eq", "^="));
      compilerMacros.autoFreeze = true;
      compilerMacros.freeze();

      pushStatus(ppStatus::reading);
    }

    preprocessor::preprocessor(const preprocessor &pp) :
      cacheMap(pp),
      statusStack(pp.statusStack),
      status(pp.status),
      passedNewline(pp.passedNewline),
      errorOnToken(pp.errorOnToken),
      directives(getDirectiveTrie()),
      sourceMacros(pp.sourceMacros) {}

    preprocessor::directiveTrie& preprocessor::getDirectiveTrie() {
      static directiveTrie trie;
      if (trie.isEmpty()) {
        trie.autoFreeze = false;
        trie.add("if"     , &preprocessor::processIf);
        trie.add("ifdef"  , &preprocessor::processIfdef);
        trie.add("ifndef" , &preprocessor::processIfndef);
        trie.add("elif"   , &preprocessor::processElif);
        trie.add("else"   , &preprocessor::processElse);
        trie.add("endif"  , &preprocessor::processEndif);

        trie.add("define" , &preprocessor::processDefine);
        trie.add("undef"  , &preprocessor::processUndef);

        trie.add("error"  , &preprocessor::processError);
        trie.add("warning", &preprocessor::processWarning);

        trie.add("include", &preprocessor::processInclude);
        trie.add("pragma" , &preprocessor::processPragma);
        trie.add("line"   , &preprocessor::processLine);
        trie.freeze();
      }
      return trie;
    }

    void preprocessor::preprint(std::ostream &out) {
      errorOnToken->preprint(out);
    }

    void preprocessor::postprint(std::ostream &out) {
      errorOnToken->postprint(out);
    }

    void preprocessor::errorOn(token_t *token,
                               const std::string &message) {
      errorOnToken = token;
      if (token) {
        printError(message);
      }
      errorOnToken = NULL;
    }

    tokenMap& preprocessor::cloneMap() const {
      return *(new preprocessor(*this));
    }

    token_t* preprocessor::pop() {
      return getToken();
    }

    void preprocessor::pushStatus(const int status_) {
      statusStack.push_back(status);
      status = status_;
    }

    int preprocessor::popStatus() {
      if (statusStack.size() == 0) {
        return 0;
      }
      status = statusStack.back();
      statusStack.pop_back();
      return status;
    }

    void preprocessor::swapReadingStatus() {
      if (status & ppStatus::reading) {
        status &= ~ppStatus::reading;
        status |= ppStatus::ignoring;
      } else {
        status &= ppStatus::ignoring;
        status |= ~ppStatus::reading;
      }
    }

    void preprocessor::incrementNewline() {
      // We need to keep passedNewline 'truthy'
      //   until after the next token
      passedNewline = 2;
    }

    void preprocessor::decrementNewline() {
      passedNewline -= !!passedNewline;
    }

    macro_t* preprocessor::getMacro(const std::string &name) {
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

    token_t* preprocessor::getSourceToken() {
      token_t *token;
      *(this->input) >> token;
      return token;
    }

    token_t* preprocessor::getToken() {
      token_t *token = NULL;
      while (!token) {
        decrementNewline();

        token = getSourceToken();
        if (!token) {
          return NULL;
        }
        std::cout << "1. [";
        if (token->type() & ~tokenType::newline) {
          token->print(std::cout);
        }
        std::cout << "] ("
                  << ((status & ppStatus::ignoring)
                      ? "ignoring"
                      : "reading")
                  << ")\n";

        const int tokenType = token->type();
        if (tokenType & tokenType::identifier) {
          token = processIdentifier(token->to<identifierToken>());
        } else if (tokenType & tokenType::op) {
          token = processOperator(token->to<operatorToken>());
        } else if (tokenType & tokenType::newline) {
          incrementNewline();
          push(token);
          token = NULL;
        }
        std::cout << "2. [";
        if (token &&
            (token->type() & ~tokenType::newline)) {
          token->print(std::cout);
        }
        std::cout << "] ("
                  << ((status & ppStatus::ignoring)
                      ? "ignoring"
                      : "reading")
                  << ")\n";
      }
      return token;
    }

    void preprocessor::expandMacro(macro_t &macro) {
      // TODO
    }

    void preprocessor::skipToNewline() {
      token_t *token = getSourceToken();
      while (token) {
        const int tokenType = token->type();
        if (tokenType & tokenType::newline) {
          incrementNewline();
          push(token);
          return;
        }
        delete token;
        token = getSourceToken();
      }
    }

    void preprocessor::getLineTokens(tokenVector &lineTokens) {
      while (true) {
        token_t *token = getSourceToken();
        if (!token) {
          break;
        }
        if (token->type() & tokenType::newline) {
          incrementNewline();
          // Add the newline token back to the queue
          // Only used by processIf, so nothing else
          //   should be expanding in between...
          push(token);
          break;
        }
        lineTokens.push_back(token);
      }
    }

    void preprocessor::freeLineTokens(tokenVector &lineTokens) {
      const int tokens = (int) lineTokens.size();
      for (int i = 0; i < tokens; ++i) {
        delete lineTokens[i];
      }
      lineTokens.clear();
    }

    void preprocessor::warnOnNonEmptyLine(const std::string &message) {
      tokenVector lineTokens;
      getLineTokens(lineTokens);
      if (lineTokens.size()) {
        lineTokens[0]->printWarning(message);
        freeLineTokens(lineTokens);
      }
    }

    token_t* preprocessor::processIdentifier(identifierToken &token) {
      // Ignore tokens inside disabled #if/#elif/#else regions
      if (status & ppStatus::ignoring) {
        delete &token;
        return NULL;
      }

      macro_t *macro = getMacro(token.value);
      if (macro) {
        if (!macro->isFunctionLike()) {
          expandMacro(*macro);
          delete &token;
          return NULL;
        }
        // Make sure that the macro starts with a '('
        token_t *nextToken = getSourceToken();
        if (token_t::safeType(nextToken) & tokenType::op) {
          const opType_t opType = nextToken->to<operatorToken>().op.opType;
          if (opType & operatorType::parenthesesEnd) {
            expandMacro(*macro);
            delete &token;
            delete nextToken;
            return NULL;
          }
        }
        push(nextToken);
      }
      return &token;
    }

    token_t* preprocessor::processOperator(operatorToken &token) {
      // // Ignore tokens inside disabled #if/#elif/#else regions
      // if (status & ppStatus::ignoring) {
      //   delete &token;
      //   return NULL;
      // }
      if ((token.op.opType != operatorType::hash) ||
          !passedNewline) {
        return &token;
      }
      delete &token;

      // NULL or an empty # is ok
      token_t *directive = getSourceToken();
      if (!directive ||
          (directive->type() & tokenType::newline)) {
        incrementNewline();
        return directive;
      }

      // Check for valid directive
      if (directive->type() != tokenType::identifier) {
        errorOn(directive,
                "Unknown preprocessor directive");
        skipToNewline();
        return NULL;
      }
      identifierToken &directiveToken = directive->to<identifierToken>();
      const std::string &directiveStr = directiveToken.value;
      directiveTrie::result_t result  = directives.get(directiveStr);
      if (!result.success()) {
        errorOn(directive,
                "Unknown preprocessor directive");
        delete directive;
        skipToNewline();
        return NULL;
      }
      (this->*(result.value()))(directiveToken);
      delete directive;
      return NULL;
    }

    void preprocessor::processIf(identifierToken &directive) {
      // Nested case
      if (status & ppStatus::ignoring) {
        skipToNewline();
        pushStatus(ppStatus::ignoring |
                   ppStatus::foundIf  |
                   ppStatus::finishedIf);
        return;
      }

      tokenVector lineTokens;
      getLineTokens(lineTokens);
      exprNode *expr = getExpression(lineTokens);

      // Test for errors
      // (expr == NULL) error is handled
      //   while forming the expression
      bool exprError = !expr;
      if (expr &&
          (expr->type() & exprNodeType::empty)) {
        errorOn(&directive,
                "Expected a value or expression");
        exprError = true;
      }
      else if (!expr->canEvaluate()) {
        errorOn(&directive,
                "Unable to evaluate expression");
        exprError = true;
      }
      // Default to #if false with error
      if (exprError) {
        pushStatus(ppStatus::ignoring |
                   ppStatus::foundIf);
        return;
      }

      pushStatus(ppStatus::foundIf | (((bool) expr->evaluate())
                                      ? ppStatus::reading
                                      : ppStatus::ignoring));
    }

    void preprocessor::processIfdef(identifierToken &directive) {
      // Nested case
      if (status & ppStatus::ignoring) {
        skipToNewline();
        pushStatus(ppStatus::ignoring |
                   ppStatus::foundIf  |
                   ppStatus::finishedIf);
        return;
      }

      token_t *token = getSourceToken();
      const int tokenType = token_t::safeType(token);

      if (!(tokenType & tokenType::identifier)) {
          // Print from the directive if we don't
          //   have a token in the same line
        token_t *errorToken = &directive;
        if (tokenType & tokenType::newline) {
          incrementNewline();
          push(token);
        } else if (tokenType & ~tokenType::none) {
          errorToken = token;
        }
        errorOn(errorToken,
                "Expected an identifier");

        // Default to false
        pushStatus(ppStatus::ignoring |
                   ppStatus::foundIf);
        return;
      }

      const std::string &macroName = token->to<identifierToken>().value;
      pushStatus(ppStatus::foundIf | (getMacro(macroName)
                                      ? ppStatus::reading
                                      : ppStatus::ignoring));

      warnOnNonEmptyLine("Extra tokens after macro name");
      delete token;
    }

    void preprocessor::processIfndef(identifierToken &directive) {
      const int oldStatus = status;
      const int oldErrors = errors;
      processIfdef(directive);
      // Keep the ignoring status if ifdef found an error
      if (oldErrors != errors) {
        return;
      }
      // If we're in a nested #if 0, keep the status
      if (oldStatus & ppStatus::ignoring) {
        return;
      }
      swapReadingStatus();
    }

    void preprocessor::processElif(identifierToken &directive) {
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

      const int oldErrors = errors;
      // Run processIf to catch possible errors
      processIf(directive);
      // processIf pushes to stack, gotta pop it out
      const int newStatus = popStatus();

      // Keep the ignoring status if ifdef found an error
      if (oldErrors != errors) {
        status = newStatus;
        return;
      }
      // If we already finished, keep old state
      if (status & ppStatus::finishedIf) {
        skipToNewline();
        return;
      }
      status = newStatus;
    }

    void preprocessor::processElse(identifierToken &directive) {
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

    void preprocessor::processEndif(identifierToken &directive) {
      warnOnNonEmptyLine("Extra tokens after #endif directive");

      if (!(status & ppStatus::foundIf)) {
        errorOn(&directive,
                "#endif without #if");
      } else {
        popStatus();
      }
    }

    void preprocessor::processDefine(identifierToken &directive) {
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
        skipToNewline();
        return;
      }
      // TODO
      // macro_t *macro;
      // sourceMacros.add(macro->name(), macro);
    }

    void preprocessor::processUndef(identifierToken &directive) {
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
      delete token;
      sourceMacros.remove(macroName);
    }

    void preprocessor::processError(identifierToken &directive) {
      // TODO
      const std::string message = "message";
      errorOn(&directive,
              message);
      skipToNewline();
    }

    void preprocessor::processWarning(identifierToken &directive) {
      // TODO
      const std::string message = "message";
      directive.printWarning(message);
      skipToNewline();
    }

    void preprocessor::processInclude(identifierToken &directive) {
      // TODO
    }

    void preprocessor::processPragma(identifierToken &directive) {
      // TODO
    }

    void preprocessor::processLine(identifierToken &directive) {
      // TODO
    }
    //====================================
  }
}
