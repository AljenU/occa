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
#include "scope.hpp"
#include "variable.hpp"
#include "type.hpp"

namespace occa {
  namespace lang {
    scope_t::scope_t() {}

    scope_t::~scope_t() {
      clear();
    }

    void scope_t::clear() {
      freeKeywords(keywords);
    }

    int scope_t::size() {
      return (int) keywords.size();
    }

    bool scope_t::has(const std::string &name) {
      return (keywords.find(name) != keywords.end());
    }

    keyword_t& scope_t::get(const std::string &name) {
      static keyword_t noKeyword;
      keywordMapIterator it = keywords.find(name);
      if (it != keywords.end()) {
        return *it->second;
      }
      return noKeyword;
    }

    bool scope_t::add(const type_t &type) {
      const std::string &name = type.name();
      if (!name.size()) {
        return true;
      }
      keywordMapIterator it = keywords.find(name);
      if (it == keywords.end()) {
        keywords[name] = new typeKeyword(type.clone());
        return true;
      }
      type.printError("[" + name + "] is already defined");
      it->second->printError("[" + name + "] was first defined here");
      return false;
    }

    bool scope_t::add(const function_t &func) {
      const std::string &name = func.name();
      if (!name.size()) {
        return true;
      }
      keywordMapIterator it = keywords.find(name);
      if (it == keywords.end()) {
        keywords[name] = new functionKeyword(func.clone().to<function_t>());
        return true;
      }
      func.printError("[" + name + "] is already defined");
      it->second->printError("[" + name + "] was first defined here");
      return false;
    }

    bool scope_t::add(const variable_t &var) {
      const std::string &name = var.name();
      if (!name.size()) {
        return true;
      }
      keywordMapIterator it = keywords.find(name);
      if (it == keywords.end()) {
        keywords[name] = new variableKeyword(*(new variable_t(var)));
        return true;
      }
      var.printError("[" + name + "] is already defined");
      it->second->printError("[" + name + "] was first defined here");
      return false;
    }
  }
}
