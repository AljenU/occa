#ifndef OCCA_PTHREADS_DEFINES_HEADER
#define OCCA_PTHREADS_DEFINES_HEADER

#include <cstdlib>
#include <cstdio>
#include <cmath>


//---[ Defines ]----------------------------------
#define OCCA_MAX_THREADS 512
#define OCCA_MEM_ALIGN   64

typedef struct float2_t { float  x,y;     } float2;
typedef struct float3_t { float  x,y,z;   } float3;
typedef struct float4_t { float  x,y,z,w; } float4;

typedef struct double2_t { double  x,y;     } double2;
typedef struct double3_t { double  x,y,z;   } double3;
typedef struct double4_t { double  x,y,z,w; } double4;
//================================================


//---[ Loop Info ]--------------------------------
#define occaOuterDim2 occaKernelArgs[0]
#define occaOuterDim1 occaKernelArgs[1]
#define occaOuterDim0 occaKernelArgs[2]
// - - - - - - - - - - - - - - - - - - - - - - - -
#define occaInnerDim2 occaKernelArgs[3]
#define occaInnerDim1 occaKernelArgs[4]
#define occaInnerDim0 occaKernelArgs[5]
// - - - - - - - - - - - - - - - - - - - - - - - -
#define occaOuterStart2 occaKernelArgs[6]
#define occaOuterEnd2   occaKernelArgs[7]
#define occaOuterStart1 occaKernelArgs[8]
#define occaOuterEnd1   occaKernelArgs[9]
#define occaOuterStart0 occaKernelArgs[10]
#define occaOuterEnd0   occaKernelArgs[11]
// - - - - - - - - - - - - - - - - - - - - - - - -
#define occaGlobalDim2 (occaInnerDim2 * occaOuterDim2)
#define occaGlobalId2  (occaOuterId2*occaInnerDim2 + occaInnerId2)

#define occaGlobalDim1 (occaInnerDim1 * occaOuterDim1)
#define occaGlobalId1  (occaOuterId1*occaInnerDim1 + occaInnerId1)

#define occaGlobalDim0 (occaInnerDim0 * occaOuterDim0)
#define occaGlobalId0  (occaOuterId0*occaInnerDim0 + occaInnerId0)
//================================================


//---[ Loops ]------------------------------------
#define occaOuterFor2 for(int occaOuterId2 = occaOuterStart2; occaOuterId2 < occaOuterEnd2; ++occaOuterId2)
#define occaOuterFor1 for(int occaOuterId1 = occaOuterStart1; occaOuterId1 < occaOuterEnd1; ++occaOuterId1)
#define occaOuterFor0 for(int occaOuterId0 = occaOuterStart0; occaOuterId0 < occaOuterEnd0; ++occaOuterId0)

#define occaOuterFor occaOuterFor2 occaOuterFor1 occaOuterFor0
// - - - - - - - - - - - - - - - - - - - - - - - -
#define occaInnerFor2 for(occaInnerId2 = 0; occaInnerId2 < occaInnerDim2; ++occaInnerId2)
#define occaInnerFor1 for(occaInnerId1 = 0; occaInnerId1 < occaInnerDim1; ++occaInnerId1)
#define occaInnerFor0 for(occaInnerId0 = 0; occaInnerId0 < occaInnerDim0; ++occaInnerId0)
#define occaInnerFor occaInnerFor2 occaInnerFor1 occaInnerFor0
// - - - - - - - - - - - - - - - - - - - - - - - -
#define occaGlobalFor0 occaOuterFor0 occaInnerFor0
//================================================


//---[ Standard Functions ]-----------------------
#define occaLocalMemFence
#define occaGlobalMemFence

#define occaBarrier(FENCE)
// - - - - - - - - - - - - - - - - - - - - - - - -
#define occaContinue continue
//================================================


//---[ Attributes ]-------------------------------
#define occaShared
#define occaPointer
#define occaVariable &
#define occaRestrict __restrict__
#define occaVolatile volatile
#define occaAligned  __attribute__ ((aligned (OCCA_MEM_ALIGN)))
#define occaFunctionShared
// - - - - - - - - - - - - - - - - - - - - - - - -
#define occaConst    const
#define occaConstant const
//================================================


//---[ Kernel Info ]------------------------------
#define occaKernelInfoArg   const int *occaKernelArgs, int occaInnerId0, int occaInnerId1, int occaInnerId2
#define occaFunctionInfoArg const int *occaKernelArgs, int occaInnerId0, int occaInnerId1, int occaInnerId2
#define occaFunctionInfo               occaKernelArgs,     occaInnerId0,     occaInnerId1,     occaInnerId2
// - - - - - - - - - - - - - - - - - - - - - - - -
#define occaKernel         extern "C"
#define occaFunction
#define occaDeviceFunction
//================================================


//---[ Math ]-------------------------------------
#define occaSqrt       sqrt
#define occaFastSqrt   sqrt
#define occaNativeSqrt sqrt

#define occaSin       sin
#define occaFastSin   sin
#define occaNativeSin sin

#define occaAsin       asin
#define occaFastAsin   asin
#define occaNativeAsin asin

#define occaSinh       sinh
#define occaFastSinh   sinh
#define occaNativeSinh sinh

#define occaAsinh       asinh
#define occaFastAsinh   asinh
#define occaNativeAsinh asinh

#define occaCos       cos
#define occaFastCos   cos
#define occaNativeCos cos

#define occaAcos       acos
#define occaFastAcos   acos
#define occaNativeAcos acos

#define occaCosh       cosh
#define occaFastCosh   cosh
#define occaNativeCosh cosh

#define occaAcosh       acosh
#define occaFastAcosh   acosh
#define occaNativeAcosh acosh

#define occaTan       tan
#define occaFastTan   tan
#define occaNativeTan tan

#define occaAtan       atan
#define occaFastAtan   atan
#define occaNativeAtan atan

#define occaTanh       tanh
#define occaFastTanh   tanh
#define occaNativeTanh tanh

#define occaAtanh       atanh
#define occaFastAtanh   atanh
#define occaNativeAtanh atanh

#define occaExp       exp
#define occaFastExp   exp
#define occaNativeExp exp

#define occaLog2       log2
#define occaFastLog2   log2
#define occaNativeLog2 log2

#define occaLog10       log10
#define occaFastLog10   log10
#define occaNativeLog10 log10
//================================================


//---[ Misc ]-------------------------------------
#define occaUnroll3(N) _Pragma(#N)
#define occaUnroll2(N) occaUnroll3(N)
#define occaUnroll(N)  occaUnroll2(unroll N)
//================================================


//---[ Private ]---------------------------------
template <class TM, const int SIZE>
class occaPrivate_t {
public:
  const int dim0, dim1, dim2;
  const int &id0, &id1, &id2;

  TM data[OCCA_MAX_THREADS][SIZE] occaAligned;

  occaPrivate_t(int dim0_, int dim1_, int dim2_,
                int &id0_, int &id1_, int &id2_) :
    dim0(dim0_),
    dim1(dim1_),
    dim2(dim2_),
    id0(id0_),
    id1(id1_),
    id2(id2_) {}

  ~occaPrivate_t(){}

  inline int index(){
    return ((id2*dim1 + id1)*dim0 + id0);
  }

  inline TM& operator [] (const int n){
    return data[index()][n];
  }

  inline operator TM(){
    return data[index()][0];
  }

  inline operator TM*(){
    return data[index()];
  }

  inline TM& operator = (const TM &t){
    data[index()][0] = t;
    return data[index()][0];
  }

  inline TM& operator += (const TM &t){
    data[index()][0] += t;
    return data[index()][0];
  }

  inline TM& operator -= (const TM &t){
    data[index()][0] -= t;
    return data[index()][0];
  }

  inline TM& operator /= (const TM &t){
    data[index()][0] /= t;
    return data[index()][0];
  }

  inline TM& operator *= (const TM &t){
    data[index()][0] *= t;
    return data[index()][0];
  }

  inline TM& operator ++ (){
    return (++data[index()][0]);
  }

  inline TM& operator ++ (int){
    return (data[index()][0]++);
  }

  inline TM& operator -- (){
    return (--data[index()][0]);
  }

  inline TM& operator -- (int){
    return (data[index()][0]--);
  }
};

#define occaPrivateArray( TYPE , NAME , SIZE )                          \
  occaPrivate_t<TYPE,SIZE> NAME(occaInnerDim0, occaInnerDim1, occaInnerDim2, \
                                occaInnerId0, occaInnerId1, occaInnerId2);

#define occaPrivate( TYPE , NAME )                                      \
  occaPrivate_t<TYPE,1> NAME(occaInnerDim0, occaInnerDim1, occaInnerDim2, \
                             occaInnerId0, occaInnerId1, occaInnerId2);
//================================================

#endif
