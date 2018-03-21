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
#include "expression.hpp"
#include "parser.hpp"
#include "typeBuiltins.hpp"

namespace occa {
  namespace lang {
    parser_t::parser_t() :
      unknownFilter(true),
      root(),
      up(&root) {
      // Properly implement `identifier-nondigit` for identifiers
      // Meanwhile, we use the unknownFilter
      stream = (tokenizer
                .filter(unknownFilter)
                .map(preprocessor)
                .map(stringMerger)
                .map(newlineMerger));

      // Setup simple keyword -> statement peeks
      keywordPeek[keywordType::qualifier]   = statementType::declaration;
      keywordPeek[keywordType::type]        = statementType::declaration;
      keywordPeek[keywordType::if_]         = statementType::if_;
      keywordPeek[keywordType::switch_]     = statementType::switch_;
      keywordPeek[keywordType::case_]       = statementType::case_;
      keywordPeek[keywordType::default_]    = statementType::default_;
      keywordPeek[keywordType::for_]        = statementType::for_;
      keywordPeek[keywordType::while_]      = statementType::while_;
      keywordPeek[keywordType::do_]         = statementType::while_;
      keywordPeek[keywordType::break_]      = statementType::break_;
      keywordPeek[keywordType::continue_]   = statementType::continue_;
      keywordPeek[keywordType::return_]     = statementType::return_;
      keywordPeek[keywordType::public_]     = statementType::classAccess;
      keywordPeek[keywordType::protected_]  = statementType::classAccess;
      keywordPeek[keywordType::private_]    = statementType::classAccess;
      keywordPeek[keywordType::namespace_]  = statementType::namespace_;
      keywordPeek[keywordType::goto_]       = statementType::goto_;

      // Statement type -> loader function
      statementLoaders[statementType::expression]  = &parser_t::loadExpressionStatement;
      statementLoaders[statementType::declaration] = &parser_t::loadDeclarationStatement;
      statementLoaders[statementType::block]       = &parser_t::loadBlockStatement;
      statementLoaders[statementType::namespace_]  = &parser_t::loadNamespaceStatement;
      statementLoaders[statementType::typeDecl]    = &parser_t::loadTypeDeclStatement;
      statementLoaders[statementType::if_]         = &parser_t::loadIfStatement;
      statementLoaders[statementType::elif_]       = &parser_t::loadElifStatement;
      statementLoaders[statementType::else_]       = &parser_t::loadElseStatement;
      statementLoaders[statementType::for_]        = &parser_t::loadForStatement;
      statementLoaders[statementType::while_]      = &parser_t::loadWhileStatement;
      statementLoaders[statementType::switch_]     = &parser_t::loadSwitchStatement;
      statementLoaders[statementType::case_]       = &parser_t::loadCaseStatement;
      statementLoaders[statementType::default_]    = &parser_t::loadDefaultStatement;
      statementLoaders[statementType::continue_]   = &parser_t::loadContinueStatement;
      statementLoaders[statementType::break_]      = &parser_t::loadBreakStatement;
      statementLoaders[statementType::return_]     = &parser_t::loadReturnStatement;
      statementLoaders[statementType::classAccess] = &parser_t::loadClassAccessStatement;
      statementLoaders[statementType::attribute]   = &parser_t::loadAttributeStatement;
      statementLoaders[statementType::pragma]      = &parser_t::loadPragmaStatement;
      statementLoaders[statementType::goto_]       = &parser_t::loadGotoStatement;
      statementLoaders[statementType::gotoLabel]   = &parser_t::loadGotoLabelStatement;
    }

    parser_t::~parser_t() {
      clear();
    }

    //---[ Setup ]----------------------
    void parser_t::clear() {
      tokenizer.clear();
      preprocessor.clear();
      context.clear();

      root.clear();
      up = &root;

      freeKeywords(keywords);
      attributes.clear();

      success = true;
    }

    void parser_t::parseSource(const std::string &source) {
      setSource(source, false);
      if (!success) {
        return;
      }
      parseTokens();
    }

    void parser_t::parseFile(const std::string &filename) {
      setSource(filename, true);
      if (!success) {
        return;
      }
      parseTokens();
    }

    void parser_t::setSource(const std::string &source,
                             const bool isFile) {
      clear();

      if (isFile) {
        tokenizer.set(new file_t(source));
      } else {
        tokenizer.set(source.c_str());
      }

      loadTokens();
    }

    void parser_t::loadTokens() {
      token_t *token;
      while (!stream.isEmpty()) {
        stream >> token;
        context.tokens.push_back(token);
      }

      if (tokenizer.errors ||
          preprocessor.errors) {
        success = false;
        return;
      }

      context.setup();
      success = !context.hasError;

      if (success) {
        getKeywords(keywords);
      }
    }

    void parser_t::parseTokens() {
      loadAllStatements(root.children);
    }

    keyword_t* parser_t::getKeyword(token_t *token) {
      if (!(token_t::safeType(token) & tokenType::identifier)) {
        return NULL;
      }

      identifierToken &identifier = token->to<identifierToken>();
      return keywords.get(identifier.value).value();
    }
    //==================================

    //---[ Peek ]-----------------------
    int parser_t::peek() {
      const int tokens = context.size();
      if (!tokens) {
        return statementType::none;
      }

      int stype = statementType::none;
      int tokenIndex = 0;

      while (success                       &&
             (stype & statementType::none) &&
             (tokenIndex < tokens)) {

        token_t *token = context[tokenIndex];
        const int tokenType = token->type();

        if (tokenType & tokenType::identifier) {
          return peekIdentifier(tokenIndex);
        }

        if (tokenType & tokenType::op) {
          return peekOperator(tokenIndex);
        }

        if (tokenType & (tokenType::primitive |
                         tokenType::string    |
                         tokenType::char_)) {
          return statementType::expression;
        }

        if (tokenType & tokenType::pragma) {
          return statementType::pragma;
        }

        ++tokenIndex;
      }

      return (success
              ? stype
              : statementType::none);
    }

    int parser_t::peekIdentifier(const int tokenIndex) {
      token_t *token     = context[tokenIndex];
      keyword_t *keyword = getKeyword(token);

      if (!keyword) {
        // Test for : for it to be a goto label
        if (isGotoLabel(tokenIndex + 1)) {
          return statementType::gotoLabel;
        }
        // TODO: Attempt to find error by guessing the keyword type
        token->printError("Unknown identifier");
        success = false;
        return statementType::none;
      }

      const int kType = keyword->type();
      const int sType = keywordPeek[kType];

      if (sType) {
        return sType;
      }

      if (kType & keywordType::else_) {
        keyword_t *nextKeyword = getKeyword(context[tokenIndex + 1]);
        if (nextKeyword &&
            (nextKeyword->type() & keywordType::if_)) {
          return statementType::elif_;
        }
        return statementType::else_;
      }

      return statementType::expression;
    }

    bool parser_t::isGotoLabel(const int tokenIndex) {
      token_t *token = context[tokenIndex];
      if (!(token_t::safeType(token) & tokenType::op)) {
        return false;
      }
      operatorToken &opToken = token->to<operatorToken>();
      return (opToken.getOpType() & operatorType::colon);
    }

    int parser_t::peekOperator(const int tokenIndex) {
      const opType_t opType = (context[tokenIndex]
                               ->to<operatorToken>()
                               .getOpType());

      if (opType & operatorType::braceStart) {
        return statementType::block;
      }
      if (opType & operatorType::attribute) {
        return statementType::attribute;
      }
      return statementType::expression;
    }
    //==================================

    //---[ Type Loaders ]---------------
    vartype_t parser_t::loadType() {
      vartype_t vartype;
      if (!context.size()) {
        return vartype;
      }

      loadBaseType(vartype);
      if (!success ||
          !vartype.isValid()) {
        return vartype;
      }
      setPointers(vartype);
      setReference(vartype);

      // Get variable name

      if (willLoadFunctionPointer()) {
        loadFunctionPointer(vartype);
      } else {
        setArrays(vartype);
      }

      return vartype;
    }

    void parser_t::loadBaseType(vartype_t &vartype) {
      const int tokens = context.size();
      int tokenPos;
      for (tokenPos = 0; tokenPos < tokens; ++tokenPos) {
        token_t *token     = context[tokenPos];
        keyword_t *keyword = getKeyword(token);
        if (!keyword) {
          break;
        }

        const int kType = keyword->type();
        if (kType & keywordType::qualifier) {
          loadQualifier(token,
                        keyword->to<qualifierKeyword>().qualifier,
                        vartype);
          continue;
        }
        if ((kType & keywordType::type) &&
            !vartype.isValid()) {
          vartype.type = &(keyword->to<typeKeyword>().type_);
          continue;
        }
        break;
      }

      if (tokenPos == 0) {
        context[0]->printError("Unable to load type");
        success = false;
        return;
      }

      // Store token just in case we didn't load a type
      token_t *lastToken = context[tokenPos - (tokenPos == tokens)];
      context.set(tokenPos);

      if (vartype.isValid()) {
        return;
      }

      if (vartype.has(long_) ||
          vartype.has(longlong_)) {
        vartype.type = &int_;
        return;
      }

      lastToken->printError("Expected a type");
      success = false;
    }

    void parser_t::loadQualifier(token_t *token,
                                 const qualifier_t &qualifier,
                                 vartype_t &vartype) {
      // Handle long/long long case
      if (&qualifier == &long_) {
        if (vartype.has(long_)) {
          vartype -= long_;
          vartype += longlong_;
        }
        else if (vartype.has(longlong_)) {
          token->printWarning("'long long long' is tooooooo long,"
                              " ignoring additional longs");
        }
        else {
          vartype += long_;
        }
        return;
      }

      // Non-long qualifiers
      if (!vartype.has(qualifier)) {
        vartype += qualifier;
      } else {
        token->printWarning("Ignoring duplicate qualifier");
      }
    }

    void parser_t::setPointers(vartype_t &vartype) {
      while (success &&
             context.size()) {
        token_t *token = context[0];
        if (!(token_t::safeType(token) & tokenType::op)) {
          break;
        }
        operatorToken &opToken = token->to<operatorToken>();
        if (!(opToken.getOpType() & operatorType::mult)) {
          break;
        }
        context.set(1);
        setPointer(vartype);
      }
    }

    void parser_t::setPointer(vartype_t &vartype) {
      pointer_t pointer;

      const int tokens = context.size();
      int tokenPos;
      for (tokenPos = 0; tokenPos < tokens; ++tokenPos) {
        token_t *token     = context[tokenPos];
        keyword_t *keyword = getKeyword(token);
        if (!(keyword_t::safeType(keyword) & keywordType::qualifier)) {
          break;
        }

        const qualifier_t &qualifier = keyword->to<qualifierKeyword>().qualifier;
        if (!(qualifier.type() & qualifierType::forPointers)) {
          token->printError("Cannot add this qualifier to a pointer");
          success = false;
          break;
        }
        pointer += qualifier;
      }

      context.set(tokenPos);

      if (success) {
        vartype += pointer;
      }
    }

    void parser_t::setReference(vartype_t &vartype) {
      if (!context.size()) {
        return;
      }
      token_t *token = context[0];
      if (!(token_t::safeType(token) & tokenType::op)) {
        return;
      }
      operatorToken &opToken = token->to<operatorToken>();
      if (!(opToken.getOpType() & operatorType::bitAnd)) {
        return;
      }
      context.set(1);
      vartype.isReference = true;
    }

    bool parser_t::willLoadFunctionPointer() {
      /*
        (* -> function
        (^ |

        int *[3] -> int *p[3];
        int *()  -> int (*p)();
      */
      return false;
    }

    void parser_t::loadFunctionPointer(vartype_t &vartype) {
    }

    void parser_t::setArrays(vartype_t &vartype) {
      while (success &&
             context.size()) {
        token_t *token = context[0];
        if (!(token_t::safeType(token) & tokenType::op)) {
          break;
        }
        operatorToken &opToken = token->to<operatorToken>();
        if (!(opToken.getOpType() & operatorType::bracketStart)) {
          break;
        }
        const int nextTokenIndex = context.getClosingPair(0);
        context.push(1, nextTokenIndex);

        tokenVector tokens;
        context.getAndCloneTokens(tokens);
        exprNode *size = getExpression(tokens);
        if (size) {
          vartype += array_t(size);
        } else {
          success = false;
        }

        context.pop();
        context.set(nextTokenIndex + 1);
      }
    }

    class_t parser_t::loadClassType() {
      context[0]->printError("Cannot parse classes yet");
      success = false;
      return class_t();
    }

    struct_t parser_t::loadStructType() {
      context[0]->printError("Cannot parse structs yet");
      success = false;
      return struct_t();
    }

    enum_t parser_t::loadEnumType() {
      context[0]->printError("Cannot parse enum yet");
      success = false;
      return enum_t();
    }

    union_t parser_t::loadUnionType() {
      context[0]->printError("Cannot parse union yet");
      success = false;
      return union_t();
    }
    //==================================

    //---[ Statement Loaders ]----------
    void parser_t::loadAllStatements(statementPtrVector &statements) {
      statement_t *smnt = getNextStatement();
      while (smnt) {
        statements.push_back(smnt);
        smnt = getNextStatement();
      }

      if (!success) {
        const int count = (int) statements.size();
        for (int i = 0; i < count; ++i) {
          delete statements[i];
        }
        statements.clear();
      }
    }

    statement_t* parser_t::getNextStatement() {
      const int sType = peek();
      if ((!success) ||
          (sType & (statementType::none |
                    statementType::empty))) {
        return NULL;
      }

      statementLoaderMap::iterator it = statementLoaders.find(sType);
      if (it != statementLoaders.end()) {
        statementLoader_t loader = it->second;
        statement_t *smnt = (this->*loader)();
        if (!success) {
          delete smnt;
          smnt = NULL;
        }
        return smnt;
      }

      OCCA_FORCE_ERROR("[Waldo] Oops, forgot to implement a statement loader"
                       " for [" << stringifySetBits(sType) << "]");
      return NULL;
    }

    statement_t* parser_t::loadBlockStatement() {
      blockStatement *smnt = new blockStatement();
      loadAllStatements(smnt->children);
      return smnt;
    }

    statement_t* parser_t::loadExpressionStatement() {
      const int end = context.getNextOperator(operatorType::semicolon);
      if (end < 0) {
        context[context.size() - 1]->printError("Missing ;");
        success = false;
        return NULL;
      }

      context.push(0, end);

      tokenVector tokens;
      context.getAndCloneTokens(tokens);

      exprNode *expr = getExpression(tokens);
      if (!expr) {
        success = false;
        return NULL;
      }

      context.pop();
      context.set(end + 1);

      return new expressionStatement(*expr);
    }

    statement_t* parser_t::loadDeclarationStatement() {
      return NULL;
    }

    statement_t* parser_t::loadNamespaceStatement() {
      return NULL;
    }

    statement_t* parser_t::loadTypeDeclStatement() {
      return NULL;
    }

    statement_t* parser_t::loadIfStatement() {
      return NULL;
    }

    statement_t* parser_t::loadElifStatement() {
      return NULL;
    }

    statement_t* parser_t::loadElseStatement() {
      return NULL;
    }

    statement_t* parser_t::loadForStatement() {
      return NULL;
    }

    statement_t* parser_t::loadWhileStatement() {
      return NULL;
    }

    statement_t* parser_t::loadSwitchStatement() {
      return NULL;
    }

    statement_t* parser_t::loadCaseStatement() {
      return NULL;
    }

    statement_t* parser_t::loadDefaultStatement() {
      return NULL;
    }

    statement_t* parser_t::loadContinueStatement() {
      return NULL;
    }

    statement_t* parser_t::loadBreakStatement() {
      return NULL;
    }

    statement_t* parser_t::loadReturnStatement() {
      return NULL;
    }

    statement_t* parser_t::loadClassAccessStatement() {
      return NULL;
    }

    statement_t* parser_t::loadAttributeStatement() {
      return NULL;
    }

    statement_t* parser_t::loadPragmaStatement() {
      return NULL;
    }

    statement_t* parser_t::loadGotoStatement() {
      return NULL;
    }

    statement_t* parser_t::loadGotoLabelStatement() {
      return NULL;
    }
    //==================================
  }
}
