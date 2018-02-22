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
#include "tokenStream.hpp"
#include "occa/tools/string.hpp"

namespace occa {
  namespace lang {
    tokenStream::~tokenStream() {}

    token_t* tokenStream::getToken() {
      return _getToken();
    }

    token_t* tokenStream::_getToken() {
      OCCA_FORCE_ERROR("_getToken() not implemented");
      return NULL;
    }

    token_t* tokenStream::getSourceToken() {
      OCCA_ERROR("Calling getSourceToken without a source tokenStream",
                 sourceStream != NULL);
      return sourceStream->_getToken();
    }

    tokenStreamWithMap::~tokenStreamWithMap() {
      const int transformCount = (int) transforms.size();
      for (int i = 0; i < transformCount; ++i) {
        tokenStreamTransform *transform = transforms[i];
        if (!transform->removeRef()) {
          delete transform;
        }
      }
      transforms.clear();
    }

    void tokenStreamWithMap::copyStreamTransforms(const tokenStreamWithMap &stream) {
      transforms = stream.transforms;
    }

    token_t* tokenStreamWithMap::getToken() {
      tokenStream *lastTransform = this;

      const int transformCount = (int) transforms.size();
      for (int i = 0; i < transformCount; ++i) {
        tokenStreamTransform *transform = transforms[i];
        transform->sourceStream = lastTransform;
        lastTransform = transform;
      }

      return lastTransform->_getToken();
    }

    tokenStreamWithMap& tokenStreamWithMap::map(tokenStreamTransform *transform) {
      transform->addRef();
      transforms.push_back(transform);
      return *this;
    }

    tokenStreamTransform::~tokenStreamTransform() {}
    tokenStreamTransformWithMap::~tokenStreamTransformWithMap() {}
  }
}
