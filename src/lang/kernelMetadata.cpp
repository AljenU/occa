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

#include "occa/lang/kernelMetadata.hpp"

namespace occa {
  namespace lang {
    argumentInfo::argumentInfo(const bool isConst_) :
      isConst(isConst_) {}

    argumentInfo argumentInfo::fromJson(const json &j) {
      return argumentInfo(j["isConst"].boolean());
    }

    json argumentInfo::toJson() const {
      json j;
      j["isConst"] = isConst;
      return j;
    }

    kernelMetadata::kernelMetadata() {}

    kernelMetadata& kernelMetadata::operator += (const argumentInfo &argInfo) {
      argumentInfos.push_back(argInfo);
      return *this;
    }

    bool kernelMetadata::argIsConst(const int pos) const {
      if (pos < (int) argumentInfos.size()) {
        return argumentInfos[pos].isConst;
      }
      return false;
    }

    kernelMetadata kernelMetadata::fromJson(const json &j) {
      kernelMetadata meta;

      meta.name = j["name"].string();

      const jsonArray &argInfos = j["argumentInfos"].array();
      const int argumentCount = (int) argInfos.size();
      for (int i = 0; i < argumentCount; ++i) {
        meta.argumentInfos.push_back(argumentInfo::fromJson(argInfos[i]));
      }

      return meta;
    }

    json kernelMetadata::toJson() const {
      json j;

      j["name"] = name;

      const int argumentCount = (int) argumentInfos.size();
      json &argInfos = j["argumentInfos"].asArray();
      for (int k = 0; k < argumentCount; ++k) {
        argInfos += argumentInfos[k].toJson();
      }

      return j;
    }
  }
}
