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
#ifndef OCCA_LANG_BASESTATEMENT_HEADER
#define OCCA_LANG_BASESTATEMENT_HEADER

#include <vector>

#include "printer.hpp"
#include "scope.hpp"
#include "trie.hpp"

namespace occa {
  namespace lang {
    class statement_t;
    class attribute_t;

    typedef std::vector<statement_t*> statementPtrVector;
    typedef std::vector<attribute_t>  attributeVector_t;

    namespace statementType {
      extern const int none;
      extern const int empty;

      extern const int pragma;

      extern const int block;
      extern const int namespace_;

      extern const int typeDecl;

      extern const int classAccess;

      extern const int expression;
      extern const int declaration;

      extern const int goto_;
      extern const int gotoLabel;

      extern const int if_;
      extern const int elif_;
      extern const int else_;
      extern const int for_;
      extern const int while_;
      extern const int switch_;
      extern const int case_;
      extern const int default_;
      extern const int continue_;
      extern const int break_;

      extern const int return_;

      extern const int attribute;
    }

    class statement_t {
    public:
      statement_t *up;
      attributeVector_t attributes;

      statement_t();

      virtual ~statement_t();

      template <class TM>
      inline bool is() const {
        return (dynamic_cast<const TM*>(this) != NULL);
      }

      template <class TM>
      inline TM& to() {
        TM *ptr = dynamic_cast<TM*>(this);
        OCCA_ERROR("Unable to cast statement_t::to",
                   ptr != NULL);
        return *ptr;
      }

      template <class TM>
      inline const TM& to() const {
        const TM *ptr = dynamic_cast<const TM*>(this);
        OCCA_ERROR("Unable to cast statement_t::to",
                   ptr != NULL);
        return *ptr;
      }

      statement_t& clone() const;
      virtual statement_t& clone_() const = 0;

      virtual int type() const = 0;

      virtual scope_t* getScope();

      void addAttribute(const attribute_t &attribute);

      virtual void print(printer &pout) const = 0;

      std::string toString() const;
      operator std::string() const;
      void print() const;
    };

    //---[ Empty ]----------------------
    class emptyStatement : public statement_t {
    public:
      emptyStatement();

      virtual statement_t& clone_() const;
      virtual int type() const;

      virtual void print(printer &pout) const;
    };
    //==================================

    //---[ Block ]------------------------
    class blockStatement : public statement_t {
    public:
      statementPtrVector children;
      scope_t scope;

      blockStatement();
      blockStatement(const blockStatement &other);

      virtual statement_t& clone_() const;
      virtual int type() const;

      virtual scope_t* getScope();

      statement_t* operator [] (const int index);

      int size() const;
      void add(statement_t &child);
      void clear();

      virtual void print(printer &pout) const;
      void printChildren(printer &pout) const;
    };
    //====================================
  }
}

#endif
