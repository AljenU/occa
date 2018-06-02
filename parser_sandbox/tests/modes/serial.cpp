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

#define OCCA_TEST_PARSER_TYPE okl::serialParser

#include "modes/serial.hpp"
#include "../parserUtils.hpp"

void testPreprocessor();
void testKernel();
void testExclusives();

int main(const int argc, const char **argv) {
  testPreprocessor();
  testKernel();
  testExclusives();

  return 0;
}

//---[ Preprocessor ]-------------------
void testPreprocessor() {
  statement_t *statement;

  // #define restrict __restrict__
  setStatement("const restrict int a;",
               statementType::declaration);
  OCCA_ASSERT_EQUAL("__restrict__",
                    parser.restrict_.name);
}
//======================================

//---[ @kernel ]------------------------
void testExtern();
void testArgs();

void testKernel() {
  testExtern();
  testArgs();
}

void testExtern() {
  // @kernel -> extern "C"

#define func (statement->to<functionDeclStatement>().function)

  statement_t *statement;
  setStatement("@kernel void foo() {}",
               statementType::functionDecl);

  OCCA_ASSERT_TRUE(func.returnType.has(externC));

#undef func
}

void testArgs() {
  // @kernel args -> by reference
#define func       (statement->to<functionDeclStatement>().function)
#define arg(N)     (*(args[N]))
#define argType(N) (arg(N).vartype)

  statement_t *statement;
  setStatement("@kernel void foo(\n"
               "const int A,\n"
               "const int *B,\n"
               "const int &C,\n"
               ") {}",
               statementType::functionDecl);

  variablePtrVector &args = func.args;
  const int argCount = (int) args.size();
  OCCA_ASSERT_EQUAL(3,
                    argCount);

  OCCA_ASSERT_EQUAL("A",
                    arg(0).name());
  OCCA_ASSERT_NOT_EQUAL((void*) NULL,
                        argType(0).referenceToken);

  OCCA_ASSERT_EQUAL("B",
                    arg(1).name());
  OCCA_ASSERT_EQUAL((void*) NULL,
                    argType(1).referenceToken);

  OCCA_ASSERT_EQUAL("C",
                    arg(2).name());
  OCCA_ASSERT_NOT_EQUAL((void*) NULL,
                        argType(2).referenceToken);

#undef func
#undef arg
}
//======================================

//---[ @exclusive ]---------------------
void testExclusives() {
  // TODO:
  //   @exclusive ->
  //     - std::vector<value>
  //     - vec.reserve(loopIterations)
  //     - Add iterator index to inner-most @inner loop
  parseAndPrintSource(
    "@kernel void foo() {\n"
    "  for (int o1 = 0; o1 < O1; ++o1; @outer) {\n"
    "    for (int o0 = 0; o0 < O0; ++o0; @outer) {\n"
    "      @exclusive int excl;\n"
    "      if (true) {\n"
    "        for (int i0 = 0; i0 < I0; ++i0; @inner) {\n"
    "          for (;;) {\n"
    "             excl = i0;\n"
    "          }\n"
    "        }\n"
    "      }\n"
    "    }\n"
    "  }\n"
    "  for (int o1 = 0; o1 < O1; ++o1; @outer) {\n"
    "    for (int o0 = 0; o0 < O0; ++o0; @outer) {\n"
    "      @exclusive int excl;\n"
    "      if (true) {\n"
    "        for (int i1 = 0; i1 < I1; ++i1; @inner) {\n"
    "          for (int i0 = 0; i0 < I0; ++i0; @inner) {\n"
    "            for (;;) {\n"
    "               excl = i0;\n"
    "            }\n"
    "            for (;;) {\n"
    "               excl = i0;\n"
    "            }\n"
    "          }\n"
    "        }\n"
    "      }\n"
    "    }\n"
    "  }\n"
    "}\n"
  );
}
//======================================
