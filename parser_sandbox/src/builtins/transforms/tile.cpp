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
#include "builtins/transforms/tile.hpp"

namespace occa {
  namespace lang {
    namespace transforms {
      tile::tile(parser_t &parser_) :
        statementTransform(parser_),
        variableReplacer(parser_) {
        validStatementTypes = statementType::for_;
      }

      bool tile::isValidInit(statement_t &smnt) {
        if (smnt.type() != statementType::declaration) {
          smnt.printError("[@tile] Expected a declaration statement");
          return false;
        }
        // Can only have one declaration
        declarationStatement &declSmnt = (declarationStatement&) smnt;
        if (declSmnt.declarations.size() > 1) {
          declSmnt.declarations[1].printError("[@tile] Can only transform 1 iterator variable");
          return false;
        }
        // Valid types: {char, short, int, long}
        variable_t &var = declSmnt.declarations[0].variable();
        const type_t *type = var.vartype.type;
        if (!type ||
            ((*type != char_)  &&
             (*type != short_) &&
             (*type != int_))) {
          var.printError("[@tile] Iterator variable needs to be of type"
                         " [char, short, int, long]");
          return false;
        }
        return true;
      }

      bool tile::isValidCheck(variable_t &var,
                              statement_t &smnt) {
        if (smnt.type() != statementType::expression) {
          smnt.printError("[@tile] Expected comparing ["
                          + var.name()
                          + "] with some bound");
          return false;
        }
        // Check valid operator (<, <=, >=, >)
        exprNode &expr = *(((expressionStatement&) smnt).expr);
        if (expr.type() != exprNodeType::binary) {
          smnt.printError("[@tile] Expected to compare ["
                          + var.name()
                          + "] with one of these operators [<, <=, >=, >]");
          return false;
        }
        binaryOpNode &opNode = (binaryOpNode&) expr;
        if (!(opNode.opType() & (operatorType::lessThan      |
                                 operatorType::lessThanEq    |
                                 operatorType::greaterThanEq |
                                 operatorType::greaterThan))) {
          smnt.printError("[@tile] Expected to compare ["
                          + var.name()
                          + "] with one of these operators [<, <=, >=, >]");
          return false;
        }
        if (!sameVariable(var, opNode)) {
          smnt.printError("[@tile] Expected to compare ["
                          + var.name()
                          + "] with one of these operators [<, <=, >=, >]");
          return false;
        }
        return true;
      }

      bool tile::isValidUpdate(variable_t &var,
                               statement_t &smnt) {
        if (smnt.type() != statementType::expression) {
          smnt.printError("[@tile] Expected to update ["
                          + var.name()
                          + "]");
          return false;
        }
        // Check valid operator (++, --, +=, -=)
        exprNode &expr = *(((expressionStatement&) smnt).expr);
        udim_t eType = expr.type();
        if (!(eType & (exprNodeType::leftUnary  |
                       exprNodeType::rightUnary |
                       exprNodeType::binary))) {
          smnt.printError("[@tile] Expected update ["
                          + var.name()
                          + "] with one of these operators [++, --, +=, -=]");
          return false;
        }
        bool validOp  = false;
        bool validVar = false;
        if (eType == exprNodeType::leftUnary) {
          leftUnaryOpNode &opNode = (leftUnaryOpNode&) expr;
          validOp = (opNode.opType() & (operatorType::leftIncrement |
                                        operatorType::leftDecrement));
          validVar = sameVariable(var, opNode);
        }
        else if (eType == exprNodeType::rightUnary) {
          rightUnaryOpNode &opNode = (rightUnaryOpNode&) expr;
          validOp = (opNode.opType() & (operatorType::rightIncrement |
                                        operatorType::rightDecrement));
          validVar = sameVariable(var, opNode);
        }
        else { // eType == exprNodeType::binary
          binaryOpNode &opNode = (binaryOpNode&) expr;
          validOp = (opNode.opType() & (operatorType::addEq |
                                        operatorType::subEq));
          validVar = sameVariable(var, opNode);
        }
        if (!validOp) {
          expr.printError("[@tile] Expected update ["
                          + var.name()
                          + "] with one of these operators [++, --, +=, -=]");
          return false;
        }
        if (!validVar) {
          expr.startNode()->printError("[@tile] Expected update ["
                                       + var.name()
                                       + "] with one of these operators [++, --, +=, -=]");
          return false;
        }
        return true;
      }

      bool tile::sameVariable(variable_t &var,
                              leftUnaryOpNode &opNode) {
        if (opNode.value->type() != exprNodeType::variable) {
          return false;
        }
        variable_t &var2 = ((variableNode*) opNode.value)->value;
        return (&var == &var2);
      }

      bool tile::sameVariable(variable_t &var,
                              rightUnaryOpNode &opNode) {
        if (opNode.value->type() != exprNodeType::variable) {
          return false;
        }
        variable_t &var2 = ((variableNode*) opNode.value)->value;
        return (&var == &var2);
      }

      bool tile::sameVariable(variable_t &var,
                              binaryOpNode &opNode) {
        variable_t *checkVar = NULL;
        if (opNode.leftValue->type() == exprNodeType::variable) {
          checkVar = &(((variableNode*) opNode.leftValue)->value);
        }
        if (opNode.rightValue->type() == exprNodeType::variable) {
          checkVar = &(((variableNode*) opNode.rightValue)->value);
        }
        // Check matching variables
        return (checkVar
                && (checkVar == &var));
      }

      statement_t* tile::transformStatement(statement_t &smnt) {
        forStatement &forSmnt = (forStatement&) smnt;
        attributeTokenMap::iterator it = forSmnt.attributes.find("tile");
        if (it == forSmnt.attributes.end()) {
          return &smnt;
        }

        if (!isValidInit(*forSmnt.init)) {
          return NULL;
        }
        variable_t &iter = (((declarationStatement*) forSmnt.init)
                            ->declarations[0]
                            .variable());
        if (!isValidCheck(iter, *forSmnt.check) ||
            !isValidUpdate(iter, *forSmnt.update)) {
          return NULL;
        }

        // Create the block and inner-block for-loops
        forStatement &blockForSmnt = *(new forStatement(forSmnt.up,
                                                        forSmnt.source->clone()));
        forStatement &innerForSmnt = *(new forStatement(&blockForSmnt,
                                                        forSmnt.source->clone()));

        // Rename the block interator
        variable_t &blockIter = iter.clone();
        blockIter.name() = "_occa_tiled_" + iter.name();

        if (!setupNewForStatements(forSmnt,
                                   iter, blockIter,
                                   blockForSmnt, innerForSmnt)
            || !setupBlockForStatement(forSmnt,
                                       blockIter,
                                       blockForSmnt)
            || !setupInnerForStatement(forSmnt,
                                       iter, blockIter,
                                       innerForSmnt)) {
          delete &blockForSmnt;
          delete &innerForSmnt;
          return NULL;
        }
        return &blockForSmnt;
      }

      bool tile::setupNewForStatements(forStatement &forSmnt,
                                       variable_t &iter,
                                       variable_t &blockIter,
                                       forStatement &blockForSmnt,
                                       forStatement &innerForSmnt) {
        innerForSmnt.swap(forSmnt);
        // Remove @tile to prevent recursive updates
        innerForSmnt.attributes.erase("tile");

        // Setup initial statements
        blockForSmnt.setLoopStatements(forSmnt.init, forSmnt.check, NULL);
        innerForSmnt.setLoopStatements(NULL, NULL, forSmnt.update);
        forSmnt.setLoopStatements(NULL, NULL, NULL);

        variableReplacer.set(iter, blockIter);

        // Replace instances of x with _occa_tiled_x
        return (variableReplacer.statementTransform::apply(*blockForSmnt.init)
                && variableReplacer.statementTransform::apply(*blockForSmnt.check));
      }

      bool tile::setupBlockForStatement(forStatement &forSmnt,
                                        variable_t &blockIter,
                                        forStatement &blockForSmnt) {
        /*
          for (x = START; x < END; x += INC)
          ->
          for (xTile = START; xTile < END; NULL )
          for (xTile = START; xTile < END; xTile += (INC * TILE))
        */
        blockForSmnt.update = new emptyStatement(&blockForSmnt, NULL);
        return true;
      }

      bool tile::setupInnerForStatement(forStatement &forSmnt,
                                        variable_t &iter,
                                        variable_t &blockIter,
                                        forStatement &innerForSmnt) {
        /*
          for (x = START; x < END; x += INC)
          ->
          for (NULL; NULL; x += INC)
          for (x = xTile; x < (xTile + TILE); x += INC)
        */
        innerForSmnt.init  = new emptyStatement(&innerForSmnt, NULL);
        innerForSmnt.check = new emptyStatement(&innerForSmnt, NULL);
        return true;
      }
    }
  }
}
