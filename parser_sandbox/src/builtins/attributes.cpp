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
#include "builtins/attributes.hpp"
#include "exprNode.hpp"
#include "parser.hpp"
#include "statement.hpp"
#include "variable.hpp"

namespace occa {
  namespace lang {
    //---[ @dim ]-----------------------
    dim::dim() :
      attribute_t() {}

    dim::dim(identifierToken &source_,
             exprNodeVector &dimSizes_) :
      attribute_t(source_),
      dimSizes(dimSizes_) {}

    dim::dim(const dim &other) {
      if (other.source) {
        source = (identifierToken*) other.source->clone();
      }

      const int count = (int) other.dimSizes.size();
      for (int i = 0; i < count; ++i) {
        dimSizes.push_back(&(other.dimSizes[i]->clone()));
      }
    }

    dim::~dim() {
    }

    std::string dim::name() const {
      return "dim";
    }

    bool dim::isVariableAttribute() const {
      return true;
    }

    attribute_t* dim::create(parser_t &parser,
                             identifierToken &source_,
                             const tokenRangeVector &argRanges) {

      exprNodeVector dimSizes_;
      const int args = (int) argRanges.size();
      for (int i = 0; i < args; ++i) {
        parser.context.push(argRanges[i].start,
                            argRanges[i].end);

        dimSizes_.push_back(parser.context.getExpression());

        parser.context.pop();
        parser.context.set(argRanges[i].end + 1);
      }

      return new dim(source_, dimSizes_);
    }

    attribute_t* dim::clone() {
      return new dim(*this);
    }

    bool dim::onVariableLoad(parser_t &parser,
                             variable_t &var) {
      return true;
    }

    int dim::size() {
      return (int) dimSizes.size();
    }

    exprNode* dim::operator [] (const int index) {
      const int size_ = (int) dimSizes.size();
      if ((index < 0)  || (index >= size_)) {
        return NULL;
      }
      return dimSizes[index];
    }
    //==================================

    //---[ @dimOrder ]------------------
    dimOrder::dimOrder() :
      attribute_t() {}

    dimOrder::dimOrder(identifierToken &source_) :
      attribute_t(source_) {}

    dimOrder::~dimOrder() {
    }

    std::string dimOrder::name() const {
      return "dimOrder";
    }

    bool dimOrder::isVariableAttribute() const {
      return true;
    }

    attribute_t* dimOrder::create(parser_t &parser,
                                  identifierToken &source_,
                                  const tokenRangeVector &argRanges) {
      return new dimOrder(source_);
    }

    attribute_t* dimOrder::clone() {
      if (source) {
        return new dimOrder(source->clone()->to<identifierToken>());
      }
      return new dimOrder();
    }

    bool dimOrder::onVariableLoad(parser_t &parser,
                                  variable_t &var) {
      return true;
    }
    //==================================

    //---[ @tile ]----------------------
    tile::tile() :
      attribute_t() {}

    tile::tile(identifierToken &source_) :
      attribute_t(source_) {}

    tile::~tile() {
    }

    std::string tile::name() const {
      return "tile";
    }

    attribute_t* tile::create(parser_t &parser,
                              identifierToken &source_,
                              const tokenRangeVector &argRanges) {
      return new tile(source_);
    }

    attribute_t* tile::clone() {
      if (source) {
        return new tile(source->clone()->to<identifierToken>());
      }
      return new tile();
    }

    bool tile::isStatementAttribute(const int stype) const {
      return (stype & statementType::for_);
    }

    bool tile::onStatementLoad(parser_t &parser,
                               statement_t &smnt) {
      return true;
    }
    //==================================

    //---[ @safeTile ]------------------
    safeTile::safeTile() :
      attribute_t() {}

    safeTile::safeTile(identifierToken &source_) :
      attribute_t(source_) {}

    safeTile::~safeTile() {
    }

    std::string safeTile::name() const {
      return "safeTile";
    }

    attribute_t* safeTile::create(parser_t &parser,
                                  identifierToken &source_,
                                  const tokenRangeVector &argRanges) {
      return new safeTile(source_);
    }

    attribute_t* safeTile::clone() {
      if (source) {
        return new safeTile(source->clone()->to<identifierToken>());
      }
      return new safeTile();
    }

    bool safeTile::isStatementAttribute(const int stype) const {
      return (stype & statementType::for_);
    }

    bool safeTile::onStatementLoad(parser_t &parser,
                                   statement_t &smnt) {
      return true;
    }
    //==================================
  }
}
