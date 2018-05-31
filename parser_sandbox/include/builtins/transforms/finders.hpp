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

#ifndef OCCA_LANG_BUILTINS_TRANSFORMS_FINDER_HEADER
#define OCCA_LANG_BUILTINS_TRANSFORMS_FINDER_HEADER

#include <list>
#include <vector>

#include "exprNode.hpp"
#include "statement.hpp"
#include "statementTransform.hpp"
#include "exprTransform.hpp"

namespace occa {
  namespace lang {
    typedef std::vector<statement_t*> statementPtrVector;
    typedef std::vector<exprNode*>    exprNodeVector;

    typedef bool (*statementMatcher)(statement_t &smnt);

    namespace transforms {
      //---[ Statement ]----------------
      class statementFinder : public statementTransform {
      private:
        statementPtrVector *statements;

      public:
        statementFinder();

        void getStatements(statement_t &smnt,
                           statementPtrVector &statements_);

        virtual statement_t* transformStatement(statement_t &smnt);

        virtual bool matches(statement_t &smnt) = 0;
      };

      class statementAttrFinder : public statementFinder {
      private:
        std::string attr;

      public:
        statementAttrFinder(const int validStatementTypes_,
                            const std::string &attr_);

        virtual bool matches(statement_t &smnt);
      };
      //================================

      //---[ Expr Node ]----------------
      class exprNodeFinder : public exprTransform {
      private:
        exprNodeVector *exprNodes;

      public:
        exprNodeFinder();

        void getExprNodes(exprNode &node,
                          exprNodeVector &exprNodes_);

        virtual exprNode* transformExprNode(exprNode &expr);

        virtual bool matches(exprNode &expr) = 0;
      };

      class exprNodeTypeFinder : public exprNodeFinder {
      public:
        exprNodeTypeFinder(const int validExprNodeTypes_);

        virtual bool matches(exprNode &expr);
      };

      class exprNodeAttrFinder : public exprNodeFinder {
      private:
        std::string attr;

      public:
        exprNodeAttrFinder(const int validExprNodeTypes_,
                           const std::string &attr_);

        virtual bool matches(exprNode &expr);
      };
      //================================

      //---[ Statement Tree ]-----------
      class smntTreeNode;
      class smntTreeHistory;

      typedef std::list<statement_t*>    statementPtrList;
      typedef std::vector<smntTreeNode*> smntTreeNodeVector;
      typedef std::list<smntTreeHistory> smntTreeHistoryList;

      class smntTreeNode {
      public:
        statement_t *smnt;
        smntTreeNodeVector children;

        smntTreeNode(statement_t *smnt_ = NULL);
        ~smntTreeNode();

        void free();

        int size();
        smntTreeNode* operator [] (const int index);

        void add(smntTreeNode *node);
      };

      class smntTreeHistory {
      public:
        smntTreeNode *node;
        statement_t *smnt;

        smntTreeHistory(smntTreeNode *node_,
                        statement_t *smnt_);
      };

      class smntTreeFinder : public statementTransform {
      public:
        smntTreeNode &root;
        statementMatcher matcher;
        smntTreeHistoryList history;
        int validSmntTypes;

        smntTreeFinder(const int validStatementTypes_,
                       smntTreeNode &root_,
                       statementMatcher matcher_);

        virtual statement_t* transformStatement(statement_t &smnt);

        void updateHistory(statement_t &smnt);

        void getStatementPath(statement_t &smnt,
                              statementPtrList &path);
      };
      //================================
    }

    //---[ Helper Methods ]-------------
    void findStatementsByAttr(const int validStatementTypes,
                              const std::string &attr,
                              statement_t &smnt,
                              statementPtrVector &statements);

    void findExprNodesByType(const int validExprNodeTypes,
                             exprNode &expr,
                             exprNodeVector &exprNodes);

    void findExprNodesByAttr(const int validExprNodeTypes,
                             const std::string &attr,
                             exprNode &expr,
                             exprNodeVector &exprNodes);

    void findStatementTree(const int validStatementTypes,
                           statement_t &smnt,
                           statementMatcher matcher,
                           transforms::smntTreeNode &root);
    //==================================
  }
}

#endif
