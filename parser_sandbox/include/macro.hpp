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
#ifndef OCCA_PARSER_MACRO_HEADER2
#define OCCA_PARSER_MACRO_HEADER2

#include <vector>
#include <iostream>

#include "stream.hpp"

namespace occa {
  namespace lang {
    class token_t;
    class macroToken;
    typedef std::vector<token_t*>  tokenVector;
    typedef std::vector<macroToken> macroTokenVector_t;

    class macroToken {
    public:
      token_t *token;
      int arg;

      macroToken();
      macroToken(token_t *token_);
      macroToken(const int arg_);

      inline bool isArg() const {
        return (arg >= 0);
      }
    };

    class macro_t : public streamSource<token_t*> {
    public:
      static const std::string VA_ARGS;

      std::string name;

      int argCount;
      mutable bool hasVarArgs;

      std::vector<tokenVector> args;
      int macroTokenIndex, argTokenIndex;
      macroTokenVector_t macroTokens;

      macro_t(tokenStream *sourceStream_,
              const std::string &name_);
      virtual ~macro_t();

      inline bool isFunctionLike() const {
        return ((argCount >= 0) || hasVarArgs);
      }

      bool loadArgs();

      virtual occa::baseStream<output_t>& clone() const;

      virtual bool isEmpty() const;

      virtual occa::streamSource<output_t>& operator >> (output_t &out);
    };
  }
}

#endif
