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
#include "modes/serial.hpp"
#include "modes/okl.hpp"
#include "builtins/types.hpp"
#include "builtins/attributes.hpp"

namespace occa {
  namespace lang {
    namespace okl {
      const std::string serialParser::exclusiveIndexName = "_occa_exclusive_index";

      serialParser::serialParser() :
        restrict_("__restrict__", (qualifierType::forPointers_ |
                                   qualifierType::custom)) {
        addAttribute<attributes::kernel>();
        addAttribute<attributes::outer>();
        addAttribute<attributes::inner>();
        addAttribute<attributes::shared>();
        addAttribute<attributes::exclusive>();
      }

      void serialParser::onClear() {
        macroMap::iterator mIt = preprocessor.compilerMacros.find("restrict");
        if (mIt != preprocessor.compilerMacros.end()) {
          preprocessor.compilerMacros.erase(mIt);
        }
      }

      void serialParser::beforePreprocessing() {
        std::string oldRestrict = restrict_.name;
        if (settings.has("serial/restrict")) {
          occa::json r = settings["serial/restrict"];
          if (r.isString()) {
            restrict_.name = r.string();
          } else if (r.isBoolean()) {
            if (r.boolean()) {
              restrict_.name = "__restrict__";
            } else {
              restrict_.name = "";
            }
          }
        }

        keywordMap::iterator it = keywords.find(oldRestrict);
        bool hasKeyword = (it != keywords.end());
        if (hasKeyword
            && (oldRestrict != restrict_.name)) {
          delete it->second;
          keywords.erase(it);
          hasKeyword = false;
        }
        if (!hasKeyword
            && restrict_.name.size()) {
          addKeyword(keywords,
                     new qualifierKeyword(restrict_));
        }

        macroMap::iterator mIt = preprocessor.compilerMacros.find("restrict");
        if (mIt != preprocessor.compilerMacros.end()) {
          delete mIt->second;
          preprocessor.compilerMacros.erase(mIt);
        }
        preprocessor.compilerMacros["restrict"] = (
          macro_t::defineBuiltin(preprocessor,
                                 "restrict",
                                 restrict_.name)
        );
      }

      void serialParser::afterParsing() {
        if (!success) return;
        if (settings.get("okl/validate", true)) {
          checkKernels(root);
        }

        if (!success) return;
        setupKernels();

        if (!success) return;
        setupExclusives();
      }

      void serialParser::setupKernels() {
        // Get @kernels
        statementPtrVector kernelSmnts;
        findStatementsByAttr(statementType::functionDecl,
                             "kernel",
                             root,
                             kernelSmnts);
        const int kernels = (int) kernelSmnts.size();
        for (int i = 0; i < kernels; ++i) {
          setupKernel(*((functionDeclStatement*) kernelSmnts[i]));
          if (!success) {
            break;
          }
        }
      }

      void serialParser::setupKernel(functionDeclStatement &kernelSmnt) {
        // @kernel -> extern "C"
        function_t &func = kernelSmnt.function;
        attributeToken_t &kernelAttr = kernelSmnt.attributes["kernel"];
        qualifiers_t &qualifiers = func.returnType.qualifiers;
        // Add extern "C"
        qualifiers.addFirst(kernelAttr.source->origin,
                            externC);
        // Remove other externs
        if (qualifiers.has(extern_)) {
          qualifiers -= extern_;
        }
        if (qualifiers.has(externCpp)) {
          qualifiers -= externCpp;
        }
        // Pass non-pointer arguments by reference
        const int argCount = (int) func.args.size();
        for (int i = 0; i < argCount; ++i) {
          variable_t &arg = *(func.args[i]);
          vartype_t &type = arg.vartype;
          if ((type.pointers.size() ||
               type.referenceToken)) {
            continue;
          }
          operatorToken opToken(arg.source->origin,
                                op::bitAnd);
          type.setReferenceToken(&opToken);
        }
      }

      void serialParser::setupExclusives() {
        // Get @exclusive declarations
        statementExprMap exprMap;
        findStatements(exprNodeType::variable,
                       root,
                       exclusiveVariableMatcher,
                       exprMap);

        setupExclusiveDeclarations(exprMap);
        if (!success) return;
        setupExclusiveIndices();
        if (!success) return;
        transformExprNodes(exprNodeType::variable,
                           root,
                           updateExclusiveExprNodes);
      }

      void serialParser::setupExclusiveDeclarations(statementExprMap &exprMap) {
        statementExprMap::iterator it = exprMap.begin();
        while (it != exprMap.end()) {
          statement_t *smnt = it->first;
          if (smnt->type() == statementType::declaration) {
            setupExclusiveDeclaration(*((declarationStatement*) smnt));
          }
          if (!success) {
            break;
          }
          ++it;
        }
      }

      void serialParser::setupExclusiveDeclaration(declarationStatement &declSmnt) {
        // Make sure the @exclusive variable is declared here, not used
        if (!exclusiveIsDeclared(declSmnt)) {
          return;
        }

        // Find inner-most outer loop
        statement_t *smnt = declSmnt.up;
        forStatement *innerMostOuterLoop = NULL;
        while (smnt) {
          if (smnt->hasAttribute("outer")) {
            innerMostOuterLoop = (forStatement*) smnt;
            break;
          }
          smnt = smnt->up;
        }

        // Check if index variable exists and is valid
        if (innerMostOuterLoop->scope.has(exclusiveIndexName)) {
          keyword_t &keyword = innerMostOuterLoop->scope.get(exclusiveIndexName);
          if (keyword.type() != keywordType::variable) {
            keyword.printError(exclusiveIndexName
                               + " is a restricted OCCA keyword");
            success = false;
          }
          return;
        }

        // Create index variable and its declaration statement
        const fileOrigin &origin = innerMostOuterLoop->source->origin;
        identifierToken varSource(origin, exclusiveIndexName);
        variable_t *indexVar = new variable_t(
          vartype_t(identifierToken(origin, "int"), int_),
          &varSource
        );

        // Create declaration statement for index variable
        declarationStatement &indexDeclSmnt = *(new declarationStatement(innerMostOuterLoop));
        innerMostOuterLoop->addFirst(indexDeclSmnt);
        // Add variable to decl + scope
        indexDeclSmnt.addDeclaration(*indexVar);
      }

      bool serialParser::exclusiveIsDeclared(declarationStatement &declSmnt) {
        const int declCount = (int) declSmnt.declarations.size();
        for (int i = 0; i < declCount; ++i) {
          variableDeclaration &decl = declSmnt.declarations[i];
          if (decl.variable->hasAttribute("exclusive")) {
            return true;
          }
        }
        return false;
      }

      void serialParser::setupExclusiveIndices() {
        transforms::smntTreeNode innerRoot;
        findStatementTree(statementType::for_,
                          root,
                          exclusiveInnerLoopMatcher,
                          innerRoot);

        // Get outer-most inner loops
        statementPtrVector outerMostInnerLoops;
        const int childCount = (int) innerRoot.size();
        for (int i = 0; i < childCount; ++i) {
          outerMostInnerLoops.push_back(innerRoot[i]->smnt);
        }
        // Get inner-most inner loops
        statementPtrVector innerMostInnerLoops;
        getInnerMostLoops(innerRoot, innerMostInnerLoops);

        // Initialize index variable to 0 before outer-most inner loop
        const int outerMostInnerCount = (int) outerMostInnerLoops.size();
        for (int i = 0; i < outerMostInnerCount; ++i) {
          forStatement &innerSmnt = *((forStatement*) outerMostInnerLoops[i]);
          keyword_t &keyword = innerSmnt.getScopeKeyword(exclusiveIndexName);
          variable_t &indexVar = ((variableKeyword&) keyword).variable;

          variableNode indexVarNode(innerSmnt.source,
                                    indexVar);
          primitiveNode zeroNode(innerSmnt.source,
                                 0);
          binaryOpNode assign(innerSmnt.source,
                              op::assign,
                              indexVarNode,
                              zeroNode);

          innerSmnt.up->addBefore(
            innerSmnt,
            *(new expressionStatement(&innerSmnt,
                                      *(assign.clone())))
          );
        }

        // Update index after inner-most inner loop
        const int innerMostInnerCount = (int) innerMostInnerLoops.size();
        for (int i = 0; i < innerMostInnerCount; ++i) {
          forStatement &innerSmnt = *((forStatement*) innerMostInnerLoops[i]);
          keyword_t &keyword = innerSmnt.getScopeKeyword(exclusiveIndexName);
          variable_t &indexVar = ((variableKeyword&) keyword).variable;

          variableNode indexVarNode(innerSmnt.source,
                                    indexVar);
          leftUnaryOpNode increment(innerSmnt.source,
                                    op::leftIncrement,
                                    indexVarNode);
          innerSmnt.addLast(
            *(new expressionStatement(&innerSmnt,
                                      *(increment.clone())))
          );
        }
      }

      bool serialParser::exclusiveVariableMatcher(exprNode &expr) {
        return expr.hasAttribute("exclusive");
      }

      bool serialParser::exclusiveInnerLoopMatcher(statement_t &smnt) {
        return (
          smnt.hasAttribute("inner")
          && smnt.inScope(exclusiveIndexName)
        );
      }

      void serialParser::getInnerMostLoops(transforms::smntTreeNode &innerRoot,
                                           statementPtrVector &loopSmnts) {
        const int count = (int) innerRoot.size();
        if (!count) {
          if (innerRoot.smnt) {
            loopSmnts.push_back(innerRoot.smnt);
          }
          return;
        }
        for (int i = 0; i < count; ++i) {
          getInnerMostLoops(*(innerRoot.children[i]),
                            loopSmnts);
        }
      }

      exprNode* serialParser::updateExclusiveExprNodes(statement_t &smnt,
                                                       exprNode &expr,
                                                       const bool isBeingDeclared) {
        variable_t &var = ((variableNode&) expr).value;
        if (!var.hasAttribute("exclusive")) {
          return &expr;
        }

        if (isBeingDeclared) {
          operatorToken startToken(var.source->origin,
                                   op::bracketStart);
          operatorToken endToken(var.source->origin,
                                 op::bracketEnd);
          var.vartype += array_t(startToken,
                                 endToken,
                                 new primitiveNode(var.source,
                                                   256));
          return &expr;
        }

        keyword_t &keyword = smnt.getScopeKeyword(exclusiveIndexName);
        variable_t &indexVar = ((variableKeyword&) keyword).variable;

        variableNode indexVarNode(var.source,
                                  indexVar);

        return new subscriptNode(var.source,
                                 expr,
                                 indexVarNode);
      }
    }
  }
}
