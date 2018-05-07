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
#ifndef OCCA_LANG_ATTRIBUTE_HEADER
#define OCCA_LANG_ATTRIBUTE_HEADER

#include <iostream>
#include <vector>

#include "tokenContext.hpp"

namespace occa {
  namespace lang {
    class parser_t;
    class identifierToken;
    class attribute_t;
    class vartype_t;
    class variable_t;
    class function_t;
    class statement_t;
    class expressionStatement;

    typedef std::vector<attribute_t*> attributePtrVector;

    class attribute_t {
    protected:
      identifierToken *source;

    public:
      attribute_t();
      attribute_t(identifierToken &source_);
      virtual ~attribute_t();

      virtual std::string name() const = 0;

      virtual attribute_t* create(parser_t &parser,
                                  identifierToken &source_,
                                  const tokenRangeVector &argRanges) = 0;

      virtual attribute_t* clone() = 0;

      virtual bool isVariableAttribute() const;
      virtual bool isFunctionAttribute() const;
      virtual bool isStatementAttribute(const int stype) const;

      virtual bool onVariableLoad(parser_t &parser,
                                  variable_t &var);

      virtual bool onFunctionLoad(parser_t &parser,
                                  function_t &func);

      virtual bool onStatementLoad(parser_t &parser,
                                   statement_t &smnt);

      virtual void onUse(parser_t &parser,
                         statement_t &smnt,
                         exprNode &expr);

      void printWarning(const std::string &message);
      void printError(const std::string &message);
    };

    void copyAttributes(attributePtrVector &dest,
                        const attributePtrVector &src);
    void freeAttributes(attributePtrVector &attributes);
  }
}

#endif
