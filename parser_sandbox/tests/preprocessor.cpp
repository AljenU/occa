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
#include "processingStages.hpp"
#include "preprocessor.hpp"

void testMacroDefines();
void testCppStandardTests();
void testIfElse();
void testIfElseDefines();
void testErrorDefines();
void testWeirdCase();
void testSpecialMacros();
void testEval();

using namespace occa::lang;

//---[ Util Methods ]-------------------
std::string source;
occa::stream<token_t*> stream;
token_t *token = NULL;

void setStream(const std::string &s) {
  ::source = s;
  stream = (tokenizer(source.c_str())
            .map(preprocessor())
            .map(newlineTokenMerger()));
}

void getToken() {
  delete token;
  token = NULL;
  stream >> token;
}

void setToken(const std::string &s) {
  setStream(s);
  getToken();
}

int getTokenType() {
  return token ? token->type() : 0;
}
//======================================

//---[ Macro Util Methods ]-------------

//======================================

//---[ Tests ]--------------------------
int main(const int argc, const char **argv) {
  testMacroDefines();
  // testCppStandardTests();
  testErrorDefines();
  testSpecialMacros();
  testWeirdCase();
  testIfElse();
  testIfElseDefines();
  testEval();
}

void testMacroDefines() {
  setStream(
    // Test #define
    "#define A\n"
    "A\n"
    // Test multi-token expansion
    "#define B 1 2 3\n"
    "B\n"
    // Test function-like macros
    "#define C(A1) A1\n"
    "C(1)\n"
    // Test multi-argument expansion
    "#define D(A1, A2) A1 A2 A2 A1\n"
    "D(2, 3)\n"
    // Test stringify
    "#define H(A1) #A1\n"
    "H(12)\n"
    // Test multi-token stringify
    "#define H(A1, A2) #A1 #A2\n"
    "H(12, 34)\n"
    // Test concat
    "#define E(A1, A2) A1 ## A2\n"
    "E(, 6)\n"
    "E(0, 7)\n"
    // Test varargs
    "#define F(A1, ...) A1 __VA_ARGS__\n"
    "F(7,)\n"
    "F(8, 9, 10,)\n"
    // Test only varargs
    "#define G(...) (X, ##__VA_ARGS__)\n"
    "G(11,)\n"
    "G()\n"
    // Errors:
    // - Argument missing
    "C()\n"
    // - Too many arguments
    "D(4, 5, 6)\n"
    // - Test stringify with concat fail
    "#define I(A1, A2) # A1 ## A2\n"
    "I(1, 3)\n"
  );
  while (!stream.isEmpty()) {
    token_t *token_;
    stream >> token_;
    token_->print(std::cout);
  }

  std::cerr << "Testing preprocessor errors:\n";
}

void testCppStandardTests() {
  // Test 1 in the C++ standard
  setStream(
    "#define hash_hash # ## #\n"
    "#define mkstr(a) # a\n"
    "#define in_between(a) mkstr(a)\n"
    "#define join(c, d) in_between(c hash_hash d)\n"
    "join(x, y)"
  );

  getToken();
  OCCA_ASSERT_EQUAL_BINARY(tokenType::newline,
                           token->type());
  OCCA_ASSERT_EQUAL_BINARY(tokenType::string,
                           token->type());

  const std::string output = token->to<stringToken>().value;
  OCCA_ASSERT_EQUAL("x ## y", output);

  // Test 2 in C++ standard
  setStream(
    // Defines
    "#define x      3\n"
    "#define f(a)   f(x * (a))\n"
    "#udnef  x\n"
    "#define x      2\n"
    "#define g      f\n"
    "#define z      z[0]\n"
    "#define h      g(~\n"
    "#define m(a)   a(w)\n"
    "#define w      0,1\n"
    "#define t(a)   a\n"
    "#define p()    int\n"
    "#define q(x)   x\n"
    "#define r(x,y) x ## y\n"
    "#define str(x) # x\n"
    // Source
    "f(y+1) + f(f(z)) % t(t(g)(0) + t)(1);\n"
    "g(x+(3,4)-w) | h 5) & m\n"
    "    (f)^m(m);\n"
    "p() i[q()] = { q(1), r(2,3), r(4,), r(,5), r(,) };\n"
    "char c[2][6] = { str(hello), str() };\n"
  );

  /*
    f(2 * y+1)) + f(2 * (f(2 * (z[0])))) % f(2 * (0)) + t(1);
    f(2 * (2+(3,4)-0,1)) | f(2 * (~ 5)) & f(2 * (0,1))^m(0,1);
    int i[] = { 1, 23, 4, 5, };
    char c[2][6] = { "hello", "" };
   */

  // Test 3 in C++ standard
  setStream(
    // Defines
    "#define str(s)      \n"
    "#define xstr(x)     \n"
    "#define debug(s, t) \n"
    "#define INCFILE(n)  \n"
    "#define glue(a, b)  \n"
    "#define xglue(a, b) \n"
    "#define HIGHLOW     \n"
    "#define LOW         \n"
    // Source
    "debug(1, 2);\n"
    "fputs(str(strncmp(\"abc\0d\", \"abc\", '\4')\n"
    "    == 0) str(: @\n), s);\n"
    "include xstr(INCFILE(2).h)\n"
    "glue(HIGH, LOW);\n"
    "xglue(HIGH, LOW)\n"
  );

  /*
    printf("x1= %d, x2= %s", x1, x2);
    fputs("strncmp(\"abc\\0d\", \"abc\", '\\4') == 0: @\n", s);
    include "vers2.h"
    "hello";
    "hello, world"
   */

  // Test 4 in C++ standard
  setStream(
    // Defines
    "#define t(x,y,z) x ## y ## z\n"
    // Source
    "int j[] = { t(1,2,3), t(,4,5), t(6,,7), t(8,9,),\n"
    "t(10,,), t(,11,), t(,,12), t(,,) };\n"
  );

  /*
    int j[] = { 123, 45, 67, 89,
      10, 11, 12, };
   */

  // Test 5 in C++ standard
  setStream(
    // Defines
    "#define OBJ_LIKE      (1-1)\n"
    "#define OBJ_LIKE       /* white space */ (1-1) /* other */\n"
    "#define FUNC_LIKE(a)    ( a )\n"
    "#define FUNC_LIKE( a )(      /* note the white space */ \\\n"
    "                a /* other stuff on this line\n"
    "                  */ )\n"
  );

  std::cerr << "Testing wrong macro redefinitions:";
  setStream(
    // Defines
    "#define OBJ_LIKE      (1-1)\n"
    "#define OBJ_LIKE      (0)\n"
    "#define FUNC_LIKE(a)  ( a )\n"
    "#define FUNC_LIKE(b)  ( a )\n"
  );

  // Test 6 in C++ standard
  setStream(
    // Defines
    "#define debug(...) fprintf(stderr, __VA_ARGS__)\n"
    "#define showlist(...) puts(#__VA_ARGS__)\n"
    "#define report(test, ...) ((test) ? puts(#test) : printf(__VA_ARGS__))\n"
    // Source
    "debug(\"Flag\");\n"
    "debug(\"X = %d\\n\", x);\n"
    "showlist(The first, second, and third items.);\n"
    "report(x>y, \"x is %d but y is %d\", x, y);\n"
  );

  /*
    fprintf(stderr, "Flag");
    fprintf(stderr, "X = %d\n", x);
    puts("The first, second, and third items.");
    ((x>y) ? puts("x>y") : printf("x is %d but y is %d", x, y));
   */
}

void testIfElse() {
  setStream(
    // Test #if true with #elif false
    "#if true\n"
    "1\n"
    "#elif false\n"
    "A\n"
    "#elif false\n"
    "B\n"
    "#else\n"
    "C\n"
    "#endif\n"
    // Test #if true with #elif true
    "#if true\n"
    "1\n"
    "#elif true\n"
    "D\n"
    "#elif true\n"
    "E\n"
    "#else\n"
    "F\n"
    "#endif\n"
    // Test #if false with 1st #elif true
    "#if false\n"
    "G\n"
    "#elif true\n"
    "1\n"
    "#elif false\n"
    "H\n"
    "#else\n"
    "I\n"
    "#endif\n"
    // Test #if false with 2nd #elif true
    "#if false\n"
    "J\n"
    "#elif false\n"
    "K\n"
    "#elif true\n"
    "1\n"
    "#else\n"
    "L\n"
    "#endif\n"
    // Test #else without #elif
    "#if false\n"
    "M\n"
    "#else\n"
    "1\n"
    "#endif\n"
    // Test #else without #elif
    "#if false\n"
    "N\n"
    "#elif false\n"
    "O\n"
    "#elif false\n"
    "P\n"
    "#else\n"
    "1\n"
    "#endif\n");

  int tokensFound = 0;
  do {
    getToken();
    if (getTokenType() & tokenType::newline) {
      continue;
    }
    if (token) {
      ++tokensFound;
      OCCA_ASSERT_EQUAL_BINARY(tokenType::primitive,
                               token->type());
      OCCA_ASSERT_EQUAL(1,
                        (int) token->to<primitiveToken>().value);
    }
  } while (token);

  OCCA_ASSERT_EQUAL(6,
                    tokensFound);
}

void testIfElseDefines () {
#if 0
  // Test defines
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
      if (getTokenType() & tokenType::primitive) {
        break;
      }
      if (!getTokenType()) {
        OCCA_FORCE_ERROR("[" << i << "] Expected more tokens");
      }
      if (getTokenType() != tokenType::newline) {
        token->printError("Expected only primitive or newline tokens");
        OCCA_FORCE_ERROR("Error on [" << i << "]");
      }
    }
    primitiveToken &pToken = *((primitiveToken*) token);
    OCCA_ASSERT_EQUAL(values[i],
                      (int) pToken.value);
  }
#endif
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
