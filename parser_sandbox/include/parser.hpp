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
#ifndef OCCA_LANG_PARSER_HEADER
#define OCCA_LANG_PARSER_HEADER

#include <list>
#include <map>
#include <vector>

#include "keyword.hpp"
#include "preprocessor.hpp"
#include "processingStages.hpp"
#include "statement.hpp"
#include "tokenizer.hpp"
#include "tokenContext.hpp"

namespace occa {
  namespace lang {
    class parser_t;

    typedef stream<token_t*>          tokenStream;
    typedef std::list<statement_t*>   statementList;
    typedef std::vector<attribute_t*> attributeVector;
    typedef std::map<int, int>        keywordToStatementMap;

    typedef statement_t* (parser_t::*statementLoader_t)();
    typedef std::map<int, statementLoader_t> statementLoaderMap;

    class parser_t {
    public:
      //---[ Stream ]-------------------
      tokenStream stream;
      tokenizer_t tokenizer;
      preprocessor_t preprocessor;
      stringTokenMerger stringMerger;
      newlineTokenMerger newlineMerger;
      unknownTokenFilter unknownFilter;
      //================================

      //---[ Status ]-------------------
      tokenContext context;
      keywordTrie keywords;
      keywordToStatementMap keywordPeek;
      statementLoaderMap statementLoaders;

      blockStatement root;
      blockStatement *up;
      attributeVector attributes;

      bool success;
      //================================

      parser_t();
      ~parser_t();

      //---[ Setup ]--------------------
      void clear();

      void parseSource(const std::string &source);
      void parseFile(const std::string &filename);

      void setSource(const std::string &source,
                     const bool isFile);
      void loadTokens();
      void parseTokens();

      keyword_t* getKeyword(token_t *token);
      //================================

      //---[ Peek ]---------------------
      int peek();

      int peekIdentifier(const int tokenIndex);
      bool isGotoLabel(const int tokenIndex);

      int peekOperator(const int tokenIndex);
      //================================

      //---[ Type Loaders ]-------------
      vartype_t loadType();

      void loadBaseType(vartype_t &vartype);

      void loadQualifier(token_t *token,
                         const qualifier_t &qualifier,
                         vartype_t &vartype);

      void setPointers(vartype_t &vartype);
      void setPointer(vartype_t &vartype);

      class_t loadClassType();
      struct_t loadStructType();
      enum_t loadEnumType();
      union_t loadUnionType();
      //================================

      //---[ Statement Loaders ]--------
      void loadAllStatements(statementPtrVector &statements);

      statement_t* getNextStatement();

      statement_t *loadBlockStatement();

      statement_t *loadExpressionStatement();

      statement_t *loadDeclarationStatement();

      statement_t *loadNamespaceStatement();

      statement_t *loadTypeDeclStatement();

      statement_t *loadIfStatement();
      statement_t *loadElifStatement();
      statement_t *loadElseStatement();

      statement_t *loadForStatement();
      statement_t *loadWhileStatement();

      statement_t *loadSwitchStatement();
      statement_t *loadCaseStatement();
      statement_t *loadDefaultStatement();
      statement_t *loadContinueStatement();
      statement_t *loadBreakStatement();

      statement_t *loadReturnStatement();

      statement_t *loadClassAccessStatement();

      statement_t *loadAttributeStatement();

      statement_t *loadPragmaStatement();

      statement_t *loadGotoStatement();
      statement_t *loadGotoLabelStatement();
      //================================
    };
  }
}
#endif
