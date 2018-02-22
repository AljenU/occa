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
#ifndef OCCA_PARSER_EXPRESSION_HEADER2
#define OCCA_PARSER_EXPRESSION_HEADER2

#include <stack>
#include <vector>

#include "occa/parser/primitive.hpp"
#include "token.hpp"
#include "operator.hpp"
#include "variable.hpp"
#include "printer.hpp"

namespace occa {
  namespace lang {
    class exprNode;

    typedef std::vector<exprNode*>     exprNodeVector;
    typedef std::stack<exprNode*>      exprNodeStack;
    typedef std::stack<operatorToken*> operatorStack;
    typedef std::vector<token_t*>      tokenVector;

    namespace exprNodeType {
      extern const int empty;
      extern const int primitive;
      extern const int char_;
      extern const int string;
      extern const int identifier;
      extern const int variable;
      extern const int value;

      extern const int leftUnary;
      extern const int rightUnary;
      extern const int binary;
      extern const int ternary;
      extern const int op;

      extern const int pair;

      extern const int subscript;
      extern const int call;
      extern const int new_;
      extern const int delete_;
      extern const int throw_;
      extern const int sizeof_;
      extern const int funcCast;
      extern const int parenCast;
      extern const int constCast;
      extern const int staticCast;
      extern const int reinterpretCast;
      extern const int dynamicCast;
      extern const int parentheses;
      extern const int tuple;
      extern const int cudaCall;
    }

    //---[ Expression State ]-----------
    class exprLoadState {
    public:
      token_t *prevToken;
      token_t *nextToken;
      token_t *tokenBeforePair;
      exprNodeStack output;
      operatorStack operators;

      bool hasError;

      exprLoadState();

      int outputCount();
      int operatorCount();

      exprNode& lastOutput();
      operatorToken& lastOperator();
    };
    //==================================

    class exprNode {
    public:
      token_t *token;

      exprNode(token_t *token_);

      virtual ~exprNode();

      template <class TM>
      inline bool is() const {
        return (dynamic_cast<const TM*>(this) != NULL);
      }

      template <class TM>
      inline TM& to() {
        TM *ptr = dynamic_cast<TM*>(this);
        OCCA_ERROR("Unable to cast exprNode::to",
                   ptr != NULL);
        return *ptr;
      }

      template <class TM>
      inline const TM& to() const {
        const TM *ptr = dynamic_cast<const TM*>(this);
        OCCA_ERROR("Unable to cast exprNode::to",
                   ptr != NULL);
        return *ptr;
      }

      virtual int nodeType() const = 0;

      virtual exprNode& clone() const = 0;

      virtual bool canEvaluate() const;
      virtual primitive evaluate() const;

      virtual void print(printer &pout) const = 0;

      std::string toString() const;

      void debugPrint() const;

      virtual void debugPrint(const std::string &prefix) const = 0;

      void childDebugPrint(const std::string &prefix) const;

      // Load tokens as an expression
      static exprNode* load(const tokenVector &tokens);

      static void pushOutputNode(token_t *token,
                                 exprLoadState &state);

      static void closePair(operatorToken &opToken,
                            exprLoadState &state);

      static void extractArgs(exprNodeVector &args,
                              exprNode &node,
                              exprLoadState &state);

      static void transformLastPair(operatorToken &opToken,
                                    exprLoadState &state);

      static void attachPair(operatorToken &opToken,
                             exprLoadState &state);

      static bool operatorIsLeftUnary(operatorToken &opToken,
                                      exprLoadState &state);

      static operatorToken& getOperatorToken(operatorToken &opToken,
                                             exprLoadState &state);

      static operatorToken& applyFasterOperators(operatorToken &opToken,
                                                 exprLoadState &state);

      static void applyOperator(operatorToken &opToken,
                                exprLoadState &state);
    };

    //---[ Empty ]----------------------
    class emptyNode : public exprNode {
    public:
      emptyNode();
      ~emptyNode();

      virtual int nodeType() const;

      virtual exprNode& clone() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };
    //==================================

    //---[ Values ]---------------------
    class primitiveNode : public exprNode {
    public:
      primitive value;

      primitiveNode(token_t *token_,
                    primitive value_);

      primitiveNode(const primitiveNode& node);

      ~primitiveNode();

      virtual int nodeType() const;

      virtual exprNode& clone() const;

      virtual bool canEvaluate() const;
      virtual primitive evaluate() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };

    class charNode : public exprNode {
    public:
      std::string value;

      charNode(token_t *token_,
               const std::string &value_);

      charNode(const charNode& node);

      ~charNode();

      virtual int nodeType() const;

      virtual exprNode& clone() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };

    class stringNode : public exprNode {
    public:
      int encoding;
      std::string value;

      stringNode(token_t *token_,
                 const std::string &value_);

      stringNode(const stringNode& node);

      ~stringNode();

      virtual int nodeType() const;

      virtual exprNode& clone() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };

    class identifierNode : public exprNode {
    public:
      std::string value;

      identifierNode(token_t *token_,
                     const std::string &value_);

      identifierNode(const identifierNode& node);

      ~identifierNode();

      virtual int nodeType() const;

      virtual exprNode& clone() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };

    class variableNode : public exprNode {
    public:
      variable &value;

      variableNode(token_t *token_,
                   variable &value_);

      variableNode(const variableNode& node);

      ~variableNode();

      virtual int nodeType() const;

      virtual exprNode& clone() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };
    //==================================

    //---[ Operators ]------------------
    class leftUnaryOpNode : public exprNode {
    public:
      const unaryOperator_t &op;
      exprNode &value;

      leftUnaryOpNode(token_t *token_,
                      const unaryOperator_t &op_,
                      exprNode &value_);

      leftUnaryOpNode(const leftUnaryOpNode &node);

      ~leftUnaryOpNode();

      virtual int nodeType() const;
      opType_t opnodeType() const;

      virtual exprNode& clone() const;

      virtual bool canEvaluate() const;
      virtual primitive evaluate() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };

    class rightUnaryOpNode : public exprNode {
    public:
      const unaryOperator_t &op;
      exprNode &value;

      rightUnaryOpNode(const unaryOperator_t &op_,
                       exprNode &value_);

      rightUnaryOpNode(token_t *token,
                       const unaryOperator_t &op_,
                       exprNode &value_);

      rightUnaryOpNode(const rightUnaryOpNode &node);

      ~rightUnaryOpNode();

      virtual int nodeType() const;
      opType_t opnodeType() const;

      virtual exprNode& clone() const;

      virtual bool canEvaluate() const;
      virtual primitive evaluate() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };

    class binaryOpNode : public exprNode {
    public:
      const binaryOperator_t &op;
      exprNode &leftValue, &rightValue;

      binaryOpNode(const binaryOperator_t &op_,
                   exprNode &leftValue_,
                   exprNode &rightValue_);

      binaryOpNode(token_t *token,
                   const binaryOperator_t &op_,
                   exprNode &leftValue_,
                   exprNode &rightValue_);

      binaryOpNode(const binaryOpNode &node);

      ~binaryOpNode();

      virtual int nodeType() const;
      opType_t opnodeType() const;

      virtual exprNode& clone() const;

      virtual bool canEvaluate() const;
      virtual primitive evaluate() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };

    class ternaryOpNode : public exprNode {
    public:
      exprNode &checkValue, &trueValue, &falseValue;

      ternaryOpNode(exprNode &checkValue_,
                    exprNode &trueValue_,
                    exprNode &falseValue_);

      ternaryOpNode(token_t *token,
                    exprNode &checkValue_,
                    exprNode &trueValue_,
                    exprNode &falseValue_);

      ternaryOpNode(const ternaryOpNode &node);
      ~ternaryOpNode();

      virtual int nodeType() const;
      opType_t opnodeType() const;

      virtual exprNode& clone() const;

      virtual bool canEvaluate() const;
      virtual primitive evaluate() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };
    //==================================

    //---[ Pseudo Operators ]-----------
    class subscriptNode : public exprNode {
    public:
      exprNode &value, &index;

      subscriptNode(token_t *token_,
                    exprNode &value_,
                    exprNode &index_);

      subscriptNode(const subscriptNode &node);

      ~subscriptNode();

      virtual int nodeType() const;

      virtual exprNode& clone() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };

    class callNode : public exprNode {
    public:
      exprNode &value;
      exprNodeVector args;

      callNode(token_t *token_,
               exprNode &value_,
               exprNodeVector args_);

      callNode(const callNode &node);

      ~callNode();

      inline int argCount() const {
        return (int) args.size();
      }

      virtual int nodeType() const;

      virtual exprNode& clone() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };

    class newNode : public exprNode {
    public:
      type_t &type;
      exprNode &value;
      exprNode &size;

      newNode(token_t *token_,
              type_t &type_,
              exprNode &value_);

      newNode(token_t *token_,
              type_t &type_,
              exprNode &value_,
              exprNode &size_);

      newNode(const newNode &node);

      ~newNode();

      virtual int nodeType() const;

      virtual exprNode& clone() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };

    class deleteNode : public exprNode {
    public:
      exprNode &value;
      bool isArray;

      deleteNode(token_t *token_,
                 exprNode &value_,
                 const bool isArray_);

      deleteNode(const deleteNode &node);

      ~deleteNode();

      virtual int nodeType() const;

      virtual exprNode& clone() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };

    class throwNode : public exprNode {
    public:
      exprNode &value;

      throwNode(token_t *token_,
                exprNode &value_);

      throwNode(const throwNode &node);

      ~throwNode();

      virtual int nodeType() const;

      virtual exprNode& clone() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };
    //==================================

    //---[ Builtins ]-------------------
    class sizeofNode : public exprNode {
    public:
      exprNode &value;

      sizeofNode(token_t *token_,
                 exprNode &value_);

      sizeofNode(const sizeofNode &node);

      ~sizeofNode();

      virtual int nodeType() const;

      virtual exprNode& clone() const;

      virtual bool canEvaluate() const;
      virtual primitive evaluate() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };

    class funcCastNode : public exprNode {
    public:
      type_t &type;
      exprNode &value;

      funcCastNode(token_t *token_,
                   type_t &type_,
                   exprNode &value_);

      funcCastNode(const funcCastNode &node);

      ~funcCastNode();

      virtual int nodeType() const;

      virtual exprNode& clone() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };

    class parenCastNode : public exprNode {
    public:
      type_t &type;
      exprNode &value;

      parenCastNode(token_t *token_,
                    type_t &type_,
                    exprNode &value_);

      parenCastNode(const parenCastNode &node);

      ~parenCastNode();

      virtual int nodeType() const;

      virtual exprNode& clone() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };

    class constCastNode : public exprNode {
    public:
      type_t &type;
      exprNode &value;

      constCastNode(token_t *token_,
                    type_t &type_,
                    exprNode &value_);

      constCastNode(const constCastNode &node);

      ~constCastNode();

      virtual int nodeType() const;

      virtual exprNode& clone() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };

    class staticCastNode : public exprNode {
    public:
      type_t &type;
      exprNode &value;

      staticCastNode(token_t *token_,
                     type_t &type_,
                     exprNode &value_);

      staticCastNode(const staticCastNode &node);

      ~staticCastNode();

      virtual int nodeType() const;

      virtual exprNode& clone() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };

    class reinterpretCastNode : public exprNode {
    public:
      type_t &type;
      exprNode &value;

      reinterpretCastNode(token_t *token_,
                          type_t &type_,
                          exprNode &value_);

      reinterpretCastNode(const reinterpretCastNode &node);

      ~reinterpretCastNode();

      virtual int nodeType() const;

      virtual exprNode& clone() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };

    class dynamicCastNode : public exprNode {
    public:
      type_t &type;
      exprNode &value;

      dynamicCastNode(token_t *token_,
                      type_t &type_,
                      exprNode &value_);

      dynamicCastNode(const dynamicCastNode &node);

      ~dynamicCastNode();

      virtual int nodeType() const;

      virtual exprNode& clone() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };
    //==================================

    //---[ Misc ]-----------------------
    class pairNode : public exprNode {
    public:
      const operator_t &op;
      exprNode &value;

      pairNode(operatorToken &opToken,
               exprNode &value_);

      pairNode(const pairNode &node);

      ~pairNode();

      virtual int nodeType() const;

      virtual exprNode& clone() const;

      virtual bool canEvaluate() const;
      virtual primitive evaluate() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };

    class parenthesesNode : public exprNode {
    public:
      exprNode &value;

      parenthesesNode(token_t *token_,
                      exprNode &value_);

      parenthesesNode(const parenthesesNode &node);

      ~parenthesesNode();

      virtual int nodeType() const;

      virtual exprNode& clone() const;

      virtual bool canEvaluate() const;
      virtual primitive evaluate() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };

    class tupleNode : public exprNode {
    public:
      exprNodeVector args;

      tupleNode(token_t *token_,
                exprNodeVector args_);

      tupleNode(const tupleNode &node);

      ~tupleNode();

      inline int argCount() const {
        return (int) args.size();
      }

      virtual int nodeType() const;

      virtual exprNode& clone() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };
    //==================================

    //---[ Extensions ]-----------------
    class cudaCallNode : public exprNode {
    public:
      exprNode &value;
      exprNode &blocks, &threads;

      cudaCallNode(token_t *token_,
                   exprNode &value_,
                   exprNode &blocks_,
                   exprNode &threads_);

      cudaCallNode(const cudaCallNode &node);

      ~cudaCallNode();

      virtual int nodeType() const;

      virtual exprNode& clone() const;

      virtual void print(printer &pout) const;

      virtual void debugPrint(const std::string &prefix) const;
    };
    //==================================
  }
}

#endif
