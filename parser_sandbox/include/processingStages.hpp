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
#ifndef OCCA_PARSER_PROCESSINGSTAGES_HEADER2
#define OCCA_PARSER_PROCESSINGSTAGES_HEADER2

#include "stream.hpp"

namespace occa {
  namespace lang {
    class token_t;

    typedef streamMap<token_t*, token_t*> tokenMap;
    typedef cacheMap<token_t*, token_t*> tokenCacheMap;

    class newlineTokenMerger : public tokenCacheMap {
    public:
      newlineTokenMerger();
      newlineTokenMerger(const newlineTokenMerger &map);

      virtual tokenMap& cloneMap() const;
      virtual token_t* pop();
    };

    class stringTokenMerger : public tokenCacheMap {
    public:
      stringTokenMerger();
      stringTokenMerger(const stringTokenMerger &map);

      virtual tokenMap& cloneMap() const;
      virtual token_t* pop();
    };
  }
}

#endif
