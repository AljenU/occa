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
#include "operator.hpp"

namespace occa {
  namespace lang {
    namespace operatorType {
      const opType_t not_              = (1L << 0);
      const opType_t positive          = (1L << 1);
      const opType_t negative          = (1L << 2);
      const opType_t tilde             = (1L << 3);
      const opType_t leftIncrement     = (1L << 4);
      const opType_t rightIncrement    = (1L << 5);
      const opType_t increment         = (leftIncrement |
                                          rightIncrement);
      const opType_t leftDecrement     = (1L << 6);
      const opType_t rightDecrement    = (1L << 7);
      const opType_t decrement         = (leftDecrement |
                                          rightDecrement);

      const opType_t add               = (1L << 8);
      const opType_t sub               = (1L << 9);
      const opType_t mult              = (1L << 10);
      const opType_t div               = (1L << 11);
      const opType_t mod               = (1L << 12);
      const opType_t arithmetic        = (add  |
                                          sub  |
                                          mult |
                                          div  |
                                          mod);

      const opType_t lessThan          = (1L << 13);
      const opType_t lessThanEq        = (1L << 14);
      const opType_t equal             = (1L << 15);
      const opType_t notEqual          = (1L << 16);
      const opType_t greaterThan       = (1L << 17);
      const opType_t greaterThanEq     = (1L << 18);
      const opType_t comparison        = (lessThan    |
                                          lessThanEq  |
                                          equal       |
                                          notEqual    |
                                          greaterThan |
                                          greaterThanEq);

      const opType_t and_              = (1L << 19);
      const opType_t or_               = (1L << 20);
      const opType_t boolean           = (and_ |
                                          or_);

      const opType_t bitAnd            = (1L << 21);
      const opType_t bitOr             = (1L << 22);
      const opType_t xor_              = (1L << 23);
      const opType_t leftShift         = (1L << 24);
      const opType_t rightShift        = (1L << 25);
      const opType_t shift             = (leftShift |
                                          rightShift);
      const opType_t bitOp             = (bitAnd    |
                                          bitOr     |
                                          xor_      |
                                          leftShift |
                                          rightShift);

      const opType_t assign            = (1L << 26);
      const opType_t addEq             = (1L << 27);
      const opType_t subEq             = (1L << 28);
      const opType_t multEq            = (1L << 29);
      const opType_t divEq             = (1L << 30);
      const opType_t modEq             = (1L << 31);
      const opType_t andEq             = (1L << 32);
      const opType_t orEq              = (1L << 33);
      const opType_t xorEq             = (1L << 34);
      const opType_t leftShiftEq       = (1L << 35);
      const opType_t rightShiftEq      = (1L << 36);
      const opType_t assignment        = (assign      |
                                          addEq       |
                                          subEq       |
                                          multEq      |
                                          divEq       |
                                          modEq       |
                                          andEq       |
                                          orEq        |
                                          xorEq       |
                                          leftShiftEq |
                                          rightShiftEq);

      const opType_t comma             = (1L << 37);
      const opType_t scope             = (1L << 38);
      const opType_t dot               = (1L << 39);
      const opType_t dotStar           = (1L << 40);
      const opType_t arrow             = (1L << 41);
      const opType_t arrowStar         = (1L << 42);

      const opType_t leftUnary         = (not_          |
                                          positive      |
                                          negative      |
                                          tilde         |
                                          leftIncrement |
                                          rightDecrement);

      const opType_t rightUnary        = (rightIncrement |
                                          rightDecrement);

      const opType_t binary            = (add           |
                                          sub           |
                                          mult          |
                                          div           |
                                          mod           |

                                          lessThan      |
                                          lessThanEq    |
                                          equal         |
                                          notEqual      |
                                          greaterThan   |
                                          greaterThanEq |

                                          and_          |
                                          or_           |
                                          bitAnd        |
                                          bitOr         |
                                          xor_          |
                                          leftShift     |
                                          rightShift    |

                                          assign        |
                                          addEq         |
                                          subEq         |
                                          multEq        |
                                          divEq         |
                                          modEq         |
                                          andEq         |
                                          orEq          |
                                          xorEq         |

                                          leftShiftEq   |
                                          rightShiftEq  |

                                          comma         |
                                          scope         |
                                          dot           |
                                          dotStar       |
                                          arrow         |
                                          arrowStar);

      const opType_t ternary           = (3L << 43);
      const opType_t colon             = (1L << 44);

      const opType_t braceStart        = (1L << 45);
      const opType_t braceEnd          = (1L << 46);
      const opType_t bracketStart      = (1L << 47);
      const opType_t bracketEnd        = (1L << 48);
      const opType_t parenthesesStart  = (1L << 49);
      const opType_t parenthesesEnd    = (1L << 50);

      const opType_t braces            = (braceStart       |
                                          braceEnd);
      const opType_t brackets          = (bracketStart     |
                                          bracketEnd);
      const opType_t parentheses       = (parenthesesStart |
                                          parenthesesEnd);

      const opType_t pair              = (braceStart       |
                                          braceEnd         |
                                          bracketStart     |
                                          bracketEnd       |
                                          parenthesesStart |
                                          parenthesesEnd);

      const opType_t pairStart         = (braceStart       |
                                          bracketStart     |
                                          parenthesesStart);

      const opType_t pairEnd           = (braceEnd         |
                                          bracketEnd       |
                                          parenthesesEnd);

      const opType_t lineComment       = (1L << 51);
      const opType_t blockCommentStart = (1L << 52);
      const opType_t blockCommentEnd   = (1L << 53);
      const opType_t comment           = (lineComment       |
                                          blockCommentStart |
                                          blockCommentEnd);

      const opType_t hash              = (1L << 54);
      const opType_t hashhash          = (1L << 55);
      const opType_t preprocessor      = (hash |
                                          hashhash);

      const opType_t semicolon         = (1L << 56);
      const opType_t ellipsis          = (1L << 57);
      const opType_t attribute         = (1L << 58);

      const opType_t special           = (hash           |
                                          hashhash       |
                                          semicolon      |
                                          ellipsis       |
                                          attribute);

      const opType_t overloadable      = (not_           |
                                          positive       |
                                          negative       |
                                          tilde          |
                                          leftIncrement  |
                                          leftDecrement  |
                                          rightIncrement |
                                          rightDecrement |

                                          add            |
                                          sub            |
                                          mult           |
                                          div            |
                                          mod            |

                                          lessThan       |
                                          lessThanEq     |
                                          equal          |
                                          notEqual       |
                                          greaterThan    |
                                          greaterThanEq  |

                                          and_           |
                                          or_            |
                                          bitAnd         |
                                          bitOr          |
                                          xor_           |
                                          leftShift      |
                                          rightShift     |

                                          assign         |
                                          addEq          |
                                          subEq          |
                                          multEq         |
                                          divEq          |
                                          modEq          |
                                          andEq          |
                                          orEq           |
                                          xorEq          |
                                          leftShiftEq    |
                                          rightShiftEq   |

                                          comma);
    }

    namespace op {
      //---[ Left Unary ]---------------
      const operator_t not_             ("!"  , operatorType::not_            , 1);
      const operator_t positive         ("+"  , operatorType::positive        , 1);
      const operator_t negative         ("-"  , operatorType::negative        , 1);
      const operator_t tilde            ("~"  , operatorType::tilde           , 1);
      const operator_t leftIncrement    ("++" , operatorType::leftIncrement   , 1);
      const operator_t leftDecrement    ("--" , operatorType::leftDecrement   , 1);
      //================================

      //---[ Right Unary ]--------------
      const operator_t rightIncrement   ("++" , operatorType::rightIncrement  , 1);
      const operator_t rightDecrement   ("--" , operatorType::rightDecrement  , 1);
      //================================

      //---[ Binary ]-------------------
      const operator_t add              ("+"  , operatorType::add             , 1);
      const operator_t sub              ("-"  , operatorType::sub             , 1);
      const operator_t mult             ("*"  , operatorType::mult            , 1);
      const operator_t div              ("/"  , operatorType::div             , 1);
      const operator_t mod              ("%"  , operatorType::mod             , 1);

      const operator_t lessThan         ("<"  , operatorType::lessThan        , 1);
      const operator_t lessThanEq       ("<=" , operatorType::lessThanEq      , 1);
      const operator_t equal            ("==" , operatorType::equal           , 1);
      const operator_t notEqual         ("!=" , operatorType::notEqual        , 1);
      const operator_t greaterThan      (">"  , operatorType::greaterThan     , 1);
      const operator_t greaterThanEq    (">=" , operatorType::greaterThanEq   , 1);

      const operator_t and_             ("&&" , operatorType::and_            , 1);
      const operator_t or_              ("||" , operatorType::or_             , 1);
      const operator_t bitAnd           ("&"  , operatorType::bitAnd          , 1);
      const operator_t bitOr            ("|"  , operatorType::bitOr           , 1);
      const operator_t xor_             ("^"  , operatorType::xor_            , 1);
      const operator_t leftShift        ("<<" , operatorType::leftShift       , 1);
      const operator_t rightShift       (">>" , operatorType::rightShift      , 1);

      const operator_t assign           (","  , operatorType::assign          , 1);
      const operator_t addEq            ("+=" , operatorType::addEq           , 1);
      const operator_t subEq            ("-=" , operatorType::subEq           , 1);
      const operator_t multEq           ("*=" , operatorType::multEq          , 1);
      const operator_t divEq            ("/=" , operatorType::divEq           , 1);
      const operator_t modEq            ("%=" , operatorType::modEq           , 1);
      const operator_t andEq            ("&=" , operatorType::andEq           , 1);
      const operator_t orEq             ("|=" , operatorType::orEq            , 1);
      const operator_t xorEq            ("^=" , operatorType::xorEq           , 1);
      const operator_t leftShiftEq      ("<<=", operatorType::leftShiftEq     , 1);
      const operator_t rightShiftEq     (">>=", operatorType::rightShiftEq    , 1);

      const operator_t comma            (","  , operatorType::comma           , 1);

      // Non-Overloadable
      const operator_t scope            ("::" , operatorType::scope           , 1);
      const operator_t dot              ("."  , operatorType::dot             , 1);
      const operator_t dotStar          (".*" , operatorType::dotStar         , 1);
      const operator_t arrow            ("->" , operatorType::arrow           , 1);
      const operator_t arrowStar        ("->*", operatorType::arrowStar       , 1);
      //================================

      //---[ Ternary ]------------------
      const operator_t ternary          ("?"  , operatorType::ternary         , 1);
      const operator_t colon            (":"  , operatorType::colon           , 1);
      //================================

      //---[ Pairs ]--------------------
      const operator_t braceStart       ("{"  , operatorType::braceStart      , 1);
      const operator_t braceEnd         ("}"  , operatorType::braceEnd        , 1);
      const operator_t bracketStart     ("["  , operatorType::bracketStart    , 1);
      const operator_t bracketEnd       ("]"  , operatorType::bracketEnd      , 1);
      const operator_t parenthesesStart ("("  , operatorType::parenthesesStart, 1);
      const operator_t parenthesesEnd   (")"  , operatorType::parenthesesEnd  , 1);
      //================================

      //---[ Comments ]-----------------
      const operator_t lineComment      ("//" , operatorType::lineComment      , 1);
      const operator_t blockCommentStart("/*" , operatorType::blockCommentStart, 1);
      const operator_t blockCommentEnd  ("*/" , operatorType::blockCommentEnd  , 1);
      //================================

      //---[ Special ]------------------
      const operator_t hash             ("#"  , operatorType::hash            , 1);
      const operator_t hashhash         ("##" , operatorType::hashhash        , 1);

      const operator_t semicolon        (";"  , operatorType::semicolon       , 1);
      const operator_t ellipsis         ("...", operatorType::ellipsis        , 1);
      const operator_t attribute        ("@"  , operatorType::attribute       , 1);
      //================================
    }

    operator_t::operator_t(const std::string &str_,
                           opType_t opType_,
                           int precedence_) :
      str(str_),
      opType(opType_),
      precedence(precedence_) {}

    void operator_t::print(printer &pout) const {
      pout << str;
    }
  }
}
