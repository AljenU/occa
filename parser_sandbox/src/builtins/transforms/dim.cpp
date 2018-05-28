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
#include "exprNode.hpp"
#include "statement.hpp"
#include "variable.hpp"
#include "builtins/types.hpp"
#include "builtins/transforms/dim.hpp"

namespace occa {
  namespace lang {
    namespace transforms {
      dim::dim(parser_t &parser_) :
        statementTransform(parser_),
        scopeSmnt(NULL) {
        validStatementTypes = (statementType::expression |
                               statementType::declaration);
        validExprNodeTypes = exprNodeType::call;
      }

      statement_t* dim::transformStatement(statement_t &smnt) {
        bool success = true;
        if (smnt.type() & statementType::expression) {
          success = applyToExpr(smnt, ((expressionStatement&) smnt).expr);
        } else {
          success = applyToDeclStatement((declarationStatement&) smnt);
        }
        return success ? &smnt : NULL;
      }

      exprNode* dim::transformExprNode(exprNode &node) {
        callNode &call = (callNode&) node;
        if (!(call.value->type() & exprNodeType::variable)) {
          return &node;
        }

        // Check @dim
        variable_t &var = ((variableNode*) call.value)->value;
        attributeTokenMap::iterator it = var.attributes.find("dim");
        if (it == var.attributes.end()) {
          return &node;
        }
        attributeToken_t &dimAttr = it->second;
        if (!isValidDim(call, dimAttr)) {
          return NULL;
        }
        const int dimCount = (int) call.args.size();

        // Check @dimOrder
        intVector order(dimCount);
        it = var.attributes.find("dimOrder");
        if (it == var.attributes.end()) {
          for (int i = 0; i < dimCount; ++i) {
            order[i] = i;
          }
        } else if (!getDimOrder(dimAttr, it->second, order)) {
          return NULL;
        }
        // 3
        // 2 + (2 * 3)
        // TODO: Delete token propertly
        exprNode *index = call.args[order[dimCount - 1]];
        for (int i = (dimCount - 2); i >= 0; --i) {
          const int i2 = order[i];
          token_t *source = call.args[i2]->token;
          parenthesesNode indexInParen(source->clone(),
                                       *index);
          // Don't delete the initial call.args[...]
          if (i < (dimCount - 2)) {
            delete index;
          }
          parenthesesNode dimInParen(source->clone(),
                                     *(dimAttr.args[i2].expr));
          binaryOpNode mult(source->clone(),
                            op::mult,
                            dimInParen,
                            indexInParen);
          parenthesesNode multInParen(source->clone(),
                                      mult);
          parenthesesNode argInParen(source->clone(),
                                     *(call.args[i2]));

          index = new binaryOpNode(source->clone(),
                                   op::add,
                                   argInParen,
                                   multInParen);
        }
        exprNode *newValue = new subscriptNode(call.token,
                                               *(call.value),
                                               *index);
        // Don't delete the initial call.args[...]
        if (dimCount > 1) {
          delete index;
        }
        return newValue;
      }

      bool dim::isValidDim(callNode &call,
                           attributeToken_t &dimAttr) {
        const int dimCount = (int) dimAttr.args.size();
        const int argCount = (int) call.args.size();
        if (dimCount == argCount) {
          return true;
        }

        if (dimCount < argCount) {
          call.args[dimCount]->token->printError("Too many dimensions, expected "
                                                 + occa::toString(dimCount)
                                                 + " argument(s)");
        } else {
          call.value->token->printError("Missing dimensions, expected "
                                        + occa::toString(dimCount)
                                        + " argument(s)");
        }
        return false;
      }

      bool dim::getDimOrder(attributeToken_t &dimAttr,
                            attributeToken_t &dimOrderAttr,
                            intVector &order) {
        const int dimCount   = (int) dimAttr.args.size();
        const int orderCount = (int) dimOrderAttr.args.size();
        if (dimCount < orderCount) {
          dimAttr.printError("Too many dimensions, expected "
                             + occa::toString(dimCount)
                             + " argument(s)");
          return false;
        }
        if (dimCount > orderCount) {
          dimAttr.printError("Missing dimensions, expected "
                             + occa::toString(dimCount)
                             + " argument(s)");
          return false;
        }
        for (int i = 0; i < orderCount; ++i) {
          order[i] = (int) dimOrderAttr.args[i].expr->evaluate();
        }
        return true;
      }

      bool dim::applyToDeclStatement(declarationStatement &smnt) {
        const int declCount = (int) smnt.declarations.size();
        for (int i = 0; i < declCount; ++i) {
          if (!applyToExpr(smnt, smnt.declarations[i].value)) {
            return false;
          }
        }
        return true;
      }

      bool dim::applyToExpr(statement_t &smnt,
                            exprNode *&expr) {
        if (expr == NULL) {
          return true;
        }
        scopeSmnt = &smnt;
        expr = exprTransform::apply(*expr);
        return expr;
      }
    }
  }
}
