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
#include "occa/tools/env.hpp"
#include "occa/tools/testing.hpp"

#include "tokenizer.hpp"
#include "preprocessor.hpp"

void testMacroDefines();
void testIfElseDefines();
void testErrorDefines();
void testWeirdCase();
void testSpecialMacros();
void testEval();

//---[ Util Methods ]-------------------
std::string source;
occa::stream<occa::lang::token_t*> stream;
occa::lang::token_t *token = NULL;

void setStream(const std::string &s) {
  source = s;
  stream = (occa::lang::tokenizer(source.c_str())
            .map(occa::lang::preprocessor()));
}

void getToken() {
  delete token;
  stream >> token;
}

void setToken(const std::string &s) {
  setStream(s);
  getToken();
}

int tokenType() {
  return token ? token->type() : 0;
}
//======================================

//---[ Macro Util Methods ]-------------

//======================================

//---[ Tests ]--------------------------
int main(const int argc, const char **argv) {
  testMacroDefines();
  testErrorDefines();
  testSpecialMacros();
  testWeirdCase();
  testIfElseDefines();
  testEval();
}

void testMacroDefines() {
  #if 0
  OCCA_ASSERT_EQUAL("",
                    pp.processSource("#define A\n"
                                     "A"));

  OCCA_ASSERT_EQUAL("1 2 3",
                    pp.processSource("#define B 1 2 3\n"
                                     "B"));

  pp.processSource("#define C(A1) A1\n");
  OCCA_ASSERT_EQUAL("",
                    pp.applyMacros("C()"));
  OCCA_ASSERT_EQUAL("1",
                    pp.applyMacros("C(1)"));

  pp.processSource("#define D(A1, A2) A1 A2\n");
  OCCA_ASSERT_EQUAL("2 3",
                    pp.applyMacros("D(2, 3)"));
  OCCA_ASSERT_EQUAL("4 5",
                    pp.applyMacros("D(4, 5, 6)"));

  pp.processSource("#define E(A1, A2) A1##A2\n");
  OCCA_ASSERT_EQUAL("6",
                    pp.applyMacros("E(, 6)"));
  OCCA_ASSERT_EQUAL("07",
                    pp.applyMacros("E(0, 7)"));

  pp.processSource("#define F(A1, ...) A1 __VA_ARGS__\n");
  OCCA_ASSERT_EQUAL("7 ",
                    pp.applyMacros("F(7,)"));
  OCCA_ASSERT_EQUAL("8 9, 10,",
                    pp.applyMacros("F(8, 9, 10,)"));

  pp.processSource("#define G(...) (X, ##__VA_ARGS__)\n");
  OCCA_ASSERT_EQUAL("(X, 11 )",
                    pp.applyMacros("G(11,)"));
  OCCA_ASSERT_EQUAL("(X, )",
                    pp.applyMacros("G()"));

  pp.processSource("#define H(A1) #A1\n");
  OCCA_ASSERT_EQUAL("\"12\"",
                    pp.applyMacros("H(12)"));

  pp.processSource("#define I(A1, A2) #A1##A2\n");
  OCCA_ASSERT_EQUAL("\"1\"3",
                    pp.applyMacros("I(1, 3)"));
#endif
}

void testIfElseDefines() {
  setStream(""
            // ""
            "#ifdef FOO\n"
            "1\n"
            "#endif\n"
            // "2"
            "#ifndef FOO\n"
            "2\n"
            "#endif\n"
            // ""
            "#if defined(FOO)\n"
            "3\n"
            "#endif\n"
            // "4"
            "#if !defined(FOO)\n"
            "4\n"
            "#endif\n"
            // Redefine FOO
            "#define FOO 9\n"
            // "5"
            "#ifdef FOO\n"
            "5\n"
            "#endif\n"
            // ""
            "#ifndef FOO\n"
            "6\n"
            "#endif\n"
            // "7"
            "#if defined(FOO)\n"
            "7\n"
            "#endif\n"
            // ""
            "#if !defined(FOO)\n"
            "8\n"
            "#endif\n"
            // "9"
            "FOO\n"
            // "10"
            "#undef FOO\n"
            "#define FOO 10\n"
            "FOO"
            // "11"
            "#define FOO 11\n"
            "FOO"
            "#undef FOO\n"
            // ""
            "#ifdef FOO\n"
            "12\n"
            "#endif\n"
            // "13"
            "#ifndef FOO\n"
            "13\n"
            "#endif\n"
            // ""
            "#if defined(FOO)\n"
            "14\n"
            "#endif\n"
            // "15"
            "#if !defined(FOO)\n"
            "15\n"
            "#endif\n");
  int values[9] = {
    2, 4, 5, 7, 9, 10, 11, 13, 15
  };
  for (int i = 0; i < 9; ++i) {
    while (true) {
      getToken();
      if (tokenType() & occa::lang::tokenType::primitive) {
        break;
      }
      if (!tokenType()) {
        OCCA_FORCE_ERROR("[" << i << "] Expected more tokens");
      }
      if (tokenType() != occa::lang::tokenType::newline) {
        token->printError("Expected only primitive or newline tokens");
        OCCA_FORCE_ERROR("Error on [" << i << "]");
      }
    }
    occa::lang::primitiveToken &pToken =
      *((occa::lang::primitiveToken*) token);
    OCCA_ASSERT_EQUAL(values[i],
                      (int) pToken.value);
  }
}

void testWeirdCase() {
  #if 0
  // Should print out "x ## y"
  // std::string str = pp.processSource("#define hash_hash # ## #\n"
  //                                              "#define mkstr(a) # a\n"
  //                                              "#define in_between(a) mkstr(a)\n"
  //                                              "#define join(c, d) in_between(c hash_hash d)\n"
  //                                              "join(x, y)\n");
  // std::cout << "str = " << str << '\n';
#endif
}

void testErrorDefines() {
  #if 0
  std::stringstream ss;
  pp.exitOnFatalError = false;
  pp.setOutputStream(ss);

  const bool printOutput = true;

  pp.processSource("#error \"Error\"\n");
  OCCA_ASSERT_EQUAL(0,
                    !ss.str().size());
  if (printOutput) {
    std::cout << ss.str();
  }
  ss.str("");

  pp.clear();
  pp.exitOnFatalError = false;
  pp.setOutputStream(ss);

  pp.processSource("#warning \"Warning\"\n");
  OCCA_ASSERT_EQUAL(0,
                    !ss.str().size());
  if (printOutput) {
    std::cout << ss.str();
  }
  ss.str("");
#endif
}

void testSpecialMacros() {
  #if 0
  OCCA_ASSERT_EQUAL("10\n",
                    pp.processSource("#line 10\n"
                                     "__LINE__\n"));

  OCCA_ASSERT_EQUAL("20\n"
                    + occa::env::PWD + "foo\n"
                    "22\n",
                    pp.processSource("#line 20 \"foo\"\n"
                                     "__LINE__\n"
                                     "__FILE__\n"
                                     "__LINE__\n"));

  OCCA_ASSERT_EQUAL("0\n"
                    "1\n"
                    "3\n"
                    "2\n",
                    pp.processSource("__COUNTER__\n"
                                     "__COUNTER__\n"
                                     "__LINE__\n"
                                     "__COUNTER__\n"));

  std::cout << pp.processSource("_DATE_ macro: __DATE__\n"
                                "_TIME_ macro: __TIME__\n");
#endif
}

void testEval() {
  #if 0
  // Types
  OCCA_ASSERT_EQUAL<int>(1 + 1,
                         pp.eval<int>("1 + 1"));
  OCCA_ASSERT_EQUAL<bool>(true,
                          pp.eval<bool>("1 + 1"));
  OCCA_ASSERT_EQUAL<double>(0.5 + 1.5,
                            pp.eval<double>("0.5 + 1.5"));
  OCCA_ASSERT_EQUAL("2",
                    pp.eval<std::string>("1 + 1"));
  OCCA_ASSERT_EQUAL<double>(100000000000L,
                            pp.eval<long>("100000000000L"));

  // Unary Operators
  OCCA_ASSERT_EQUAL<int>(+1,
                         pp.eval<int>("+1"));
  OCCA_ASSERT_EQUAL<int>(-1,
                         pp.eval<int>("-1"));
  OCCA_ASSERT_EQUAL<int>(!1,
                         pp.eval<int>("!1"));
  OCCA_ASSERT_EQUAL<int>(~1,
                         pp.eval<int>("~1"));

  // Binary Operators
  OCCA_ASSERT_EQUAL<double>(1 + 2,
                            pp.eval<double>("1 + 2"));
  OCCA_ASSERT_EQUAL<double>(1 - 2,
                            pp.eval<double>("1 - 2"));
  OCCA_ASSERT_EQUAL<int>(1 / 2,
                         pp.eval<double>("1 / 2"));
  OCCA_ASSERT_EQUAL<double>(1 / 2.0,
                            pp.eval<double>("1 / 2.0"));
  OCCA_ASSERT_EQUAL<int>(5 % 2,
                         pp.eval<int>("5 % 2"));

  // Bit operators
  OCCA_ASSERT_EQUAL<int>(1 & 3,
                         pp.eval<int>("1 & 3"));
  OCCA_ASSERT_EQUAL<int>(1 ^ 3,
                         pp.eval<int>("1 ^ 3"));
  OCCA_ASSERT_EQUAL<int>(1 | 3,
                         pp.eval<int>("1 | 3"));

  // Shift operators
  OCCA_ASSERT_EQUAL<int>(0,
                         pp.eval<int>("1 >> 1"));
  OCCA_ASSERT_EQUAL<int>(2,
                         pp.eval<int>("1 << 1"));

  // Parentheses
  OCCA_ASSERT_EQUAL<double>(3*(1 + 1),
                            pp.eval<int>("3*(1 + 1)"));

  OCCA_ASSERT_EQUAL<double>((3 * 1)*(1 + 1),
                            pp.eval<int>("(3 * 1)*(1 + 1)"));

  /*
  // NaN and Inf
  float x = pp.eval<float>("1/0");
  OCCA_ASSERT_EQUAL(true, x != x);

  x = pp.eval<float>("0/0");
  OCCA_ASSERT_EQUAL(true, x != x);
  */
#endif
}
//======================================
