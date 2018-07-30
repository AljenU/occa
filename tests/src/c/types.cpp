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
#include <occa.hpp>
#include <occa/c/types.hpp>
#include <occa/tools/testing.hpp>

void testNewOccaTypes();
void testCTypeWrappers();

int main(const int argc, const char **argv) {
  testNewOccaTypes();
  testCTypeWrappers();
}

void testNewOccaTypes() {
#define TEST_OCCA_TYPE(VALUE, OCCA_TYPE)        \
  do {                                          \
    occaType v = occa::c::newOccaType(VALUE);   \
    ASSERT_EQ(v.type, OCCA_TYPE);               \
    occaFree(v);                                \
  } while (0)

  TEST_OCCA_TYPE((void*) NULL, OCCA_PTR);

  TEST_OCCA_TYPE((bool) true, OCCA_BOOL);
  TEST_OCCA_TYPE((int8_t) 1, OCCA_INT8);
  TEST_OCCA_TYPE((uint8_t) 1, OCCA_UINT8);
  TEST_OCCA_TYPE((int16_t) 1, OCCA_INT16);
  TEST_OCCA_TYPE((uint16_t) 1, OCCA_UINT16);
  TEST_OCCA_TYPE((int32_t) 1, OCCA_INT32);
  TEST_OCCA_TYPE((uint32_t) 1, OCCA_UINT32);
  TEST_OCCA_TYPE((int64_t) 1, OCCA_INT64);
  TEST_OCCA_TYPE((uint64_t) 1, OCCA_UINT64);
  TEST_OCCA_TYPE((float) 1.0, OCCA_FLOAT);
  TEST_OCCA_TYPE((double) 1.0, OCCA_DOUBLE);

  TEST_OCCA_TYPE(occa::device(), OCCA_DEVICE);
  TEST_OCCA_TYPE(occa::kernel(), OCCA_KERNEL);
  TEST_OCCA_TYPE(occa::memory(), OCCA_MEMORY);
  TEST_OCCA_TYPE(*(new occa::properties()), OCCA_PROPERTIES);

  ASSERT_THROW_START {
    occa::c::newOccaType<std::string>("hi");
  } ASSERT_THROW_END;

#undef TEST_OCCA_TYPE
}

template <class TM>
int getOccaType(bool isUnsigned) {
  switch (sizeof(TM)) {
  case 1: return isUnsigned ? OCCA_UINT8 : OCCA_INT8;
  case 2: return isUnsigned ? OCCA_UINT16 : OCCA_INT16;
  case 4: return isUnsigned ? OCCA_UINT32 : OCCA_INT32;
  case 8: return isUnsigned ? OCCA_UINT64 : OCCA_INT64;
  }
}

void testCTypeWrappers() {
#define TEST_OCCA_C_TYPE(VALUE, OCCA_TYPE)      \
  do {                                          \
    occaType value = VALUE;                     \
    ASSERT_EQ(value.type, OCCA_TYPE);           \
} while (0)

  TEST_OCCA_C_TYPE(occaPtr(NULL), OCCA_PTR);

  TEST_OCCA_C_TYPE(occaBool(true), OCCA_BOOL);
  TEST_OCCA_C_TYPE(occaInt8(1), OCCA_INT8);
  TEST_OCCA_C_TYPE(occaUInt8(1), OCCA_UINT8);
  TEST_OCCA_C_TYPE(occaInt16(1), OCCA_INT16);
  TEST_OCCA_C_TYPE(occaUInt16(1), OCCA_UINT16);
  TEST_OCCA_C_TYPE(occaInt32(1), OCCA_INT32);
  TEST_OCCA_C_TYPE(occaUInt32(1), OCCA_UINT32);
  TEST_OCCA_C_TYPE(occaInt64(1), OCCA_INT64);
  TEST_OCCA_C_TYPE(occaUInt64(1), OCCA_UINT64);

  TEST_OCCA_C_TYPE(occaChar(1), getOccaType<char>(false));
  TEST_OCCA_C_TYPE(occaUChar(1), getOccaType<unsigned char>(true));
  TEST_OCCA_C_TYPE(occaShort(1), getOccaType<short>(false));
  TEST_OCCA_C_TYPE(occaUShort(1), getOccaType<unsigned short>(true));
  TEST_OCCA_C_TYPE(occaInt(1), getOccaType<int>(false));
  TEST_OCCA_C_TYPE(occaUInt(1), getOccaType<unsigned int>(true));
  TEST_OCCA_C_TYPE(occaLong(1), getOccaType<long>(false));
  TEST_OCCA_C_TYPE(occaULong(1), getOccaType<unsigned long>(true));

  TEST_OCCA_C_TYPE(occaFloat(1.0), OCCA_FLOAT);
  TEST_OCCA_C_TYPE(occaDouble(1.0), OCCA_DOUBLE);

  TEST_OCCA_C_TYPE(occaStruct(NULL, 0), OCCA_STRUCT);

  std::string str = "123";
  TEST_OCCA_C_TYPE(occaString(str.c_str()), OCCA_STRING);

#undef TEST_OCCA_C_TYPE
}
