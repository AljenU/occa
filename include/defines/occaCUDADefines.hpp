#ifndef OCCA_CUDA_DEFINES_HEADER
#define OCCA_CUDA_DEFINES_HEADER

//---[ Defines ]----------------------------------
//================================================


//---[ Loop Info ]--------------------------------
#define occaOuterDim2 gridDim.z
#define occaOuterId2  blockIdx.z

#define occaOuterDim1 gridDim.y
#define occaOuterId1  blockIdx.y

#define occaOuterDim0 gridDim.x
#define occaOuterId0  blockIdx.x
// - - - - - - - - - - - - - - - - - - - - - - - -
#define occaInnerDim2 blockDim.z
#define occaInnerId2  threadIdx.z

#define occaInnerDim1 blockDim.y
#define occaInnerId1  threadIdx.y

#define occaInnerDim0 blockDim.x
#define occaInnerId0  threadIdx.x
// - - - - - - - - - - - - - - - - - - - - - - - -
#define occaGlobalDim2 (occaInnerDim2 * occaOuterDim2)
#define occaGlobalId2 (occaOuterId2*occaInnerDim2 + occaInnerId2)

#define occaGlobalDim1 (occaInnerDim1 * occaOuterDim1)
#define occaGlobalId1 (occaOuterId1*occaInnerDim1 + occaInnerId1)

#define occaGlobalDim0 (occaInnerDim0 * occaOuterDim0)
#define occaGlobalId0 (occaOuterId0*occaInnerDim0 + occaInnerId0)
//================================================


//---[ Loops ]------------------------------------
#define occaOuterFor2
#define occaOuterFor1
#define occaOuterFor0
#define occaOuterFor
// - - - - - - - - - - - - - - - - - - - - - - - -
#define occaInnerFor2
#define occaInnerFor1
#define occaInnerFor0
#define occaInnerFor
// - - - - - - - - - - - - - - - - - - - - - - - -
#define occaGlobalFor0
//================================================


//---[ Standard Functions ]-----------------------
#define occaLocalMemFence
#define occaGlobalMemFence

#define occaBarrier(FENCE)      __syncthreads()
#define occaInnerBarrier(FENCE) __syncthreads()
#define occaOuterBarrier(FENCE)
// - - - - - - - - - - - - - - - - - - - - - - - -
#define occaContinue return
//================================================


//---[ Attributes ]-------------------------------
#define occaShared   __shared__
#define occaPointer
#define occaVariable
#define occaRestrict __restrict__
#define occaVolatile volatile
#define occaAligned
#define occaFunctionShared
// - - - - - - - - - - - - - - - - - - - - - - - -
#define occaConst    const
#define occaConstant __constant__
//================================================


//---[ Kernel Info ]------------------------------
#define occaKernelInfoArg   int occaKernelInfoArg_
#define occaFunctionInfoArg int occaKernelInfoArg_
#define occaFunctionInfo        occaKernelInfoArg_
// - - - - - - - - - - - - - - - - - - - - - - - -
#define occaKernel         extern "C" __global__
#define occaFunction       __device__
#define occaDeviceFunction __device__
//================================================


//---[ Math ]-------------------------------------
__device__ inline float  occaCuda_fabs(const float x){      return fabsf(x); }
__device__ inline double occaCuda_fabs(const double x){     return fabs(x);  }
__device__ inline float  occaCuda_fastFabs(const float x){  return fabsf(x); }
__device__ inline double occaCuda_fastFabs(const double x){ return fabs(x);  }

#define occaFabs       occaCuda_fabs
#define occaFastFabs   occaCuda_fastFabs
#define occaNativeFabs occaFabs

__device__ inline float  occaCuda_sqrt(const float x){      return sqrtf(x);      }
__device__ inline double occaCuda_sqrt(const double x){     return sqrt(x);       }
__device__ inline float  occaCuda_fastSqrt(const float x){  return __fsqrt_rn(x); }
__device__ inline double occaCuda_fastSqrt(const double x){ return __dsqrt_rn(x); }

#define occaSqrt       occaCuda_sqrt
#define occaFastSqrt   occaCuda_fastSqrt
#define occaNativeSqrt occaSqrt

__device__ inline float  occaCuda_cbrt(const float x){      return cbrtf(x); }
__device__ inline double occaCuda_cbrt(const double x){     return cbrt(x);  }
__device__ inline float  occaCuda_fastCbrt(const float x){  return cbrtf(x); }
__device__ inline double occaCuda_fastCbrt(const double x){ return cbrt(x);  }

#define occaCbrt       occaCuda_cbrt
#define occaFastCbrt   occaCuda_fastCbrt
#define occaNativeCbrt occaCbrt

__device__ inline float  occaCuda_sin(const float x){      return sinf(x);   }
__device__ inline double occaCuda_sin(const double x){     return sin(x);    }
__device__ inline float  occaCuda_fastSin(const float x){  return __sinf(x); }
__device__ inline double occaCuda_fastSin(const double x){ return sin(x);    }

#define occaSin       occaCuda_sin
#define occaFastSin   occaCuda_fastSin
#define occaNativeSin occaSin

__device__ inline float  occaCuda_asin(const float x){      return asinf(x); }
__device__ inline double occaCuda_asin(const double x){     return asin(x);  }
__device__ inline float  occaCuda_fastAsin(const float x){  return asinf(x); }
__device__ inline double occaCuda_fastAsin(const double x){ return asin(x);  }

#define occaAsin       occaCuda_asin
#define occaFastAsin   occaCuda_fastAsin
#define occaNativeAsin occaAsin

__device__ inline float  occaCuda_sinh(const float x){      return sinhf(x); }
__device__ inline double occaCuda_sinh(const double x){     return sinh(x);  }
__device__ inline float  occaCuda_fastSinh(const float x){  return sinhf(x); }
__device__ inline double occaCuda_fastSinh(const double x){ return sinh(x);  }

#define occaSinh       occaCuda_sinh
#define occaFastSinh   occaCuda_fastSinh
#define occaNativeSinh occaSinh

__device__ inline float  occaCuda_asinh(const float x){       return asinhf(x); }
__device__ inline double occaCuda_asinh(const double x){      return asinh(x);  }
__device__ inline float  occaCuda_fastAsinh(const float x){   return asinhf(x); }
__device__ inline double occaCuda_fastAsinh(const double x){  return asinh(x);  }

#define occaAsinh       occaCuda_asinh
#define occaFastAsinh   occaCuda_fastAsinh
#define occaNativeAsinh occaAsinh

__device__ inline float  occaCuda_cos(const float x){      return cosf(x);   }
__device__ inline double occaCuda_cos(const double x){     return cos(x);    }
__device__ inline float  occaCuda_fastCos(const float x){  return __cosf(x); }
__device__ inline double occaCuda_fastCos(const double x){ return cos(x);    }

#define occaCos       occaCuda_cos
#define occaFastCos   occaCuda_fastCos
#define occaNativeCos occaCos

__device__ inline float  occaCuda_acos(const float x){      return acosf(x); }
__device__ inline double occaCuda_acos(    const double x){ return acos(x);  }
__device__ inline float  occaCuda_fastAcos(const float x){  return acosf(x); }
__device__ inline double occaCuda_fastAcos(const double x){ return acos(x);  }

#define occaAcos       occaCuda_acos
#define occaFastAcos   occaCuda_fastAcos
#define occaNativeAcos occaAcos

__device__ inline float  occaCuda_cosh(const float x){      return coshf(x); }
__device__ inline double occaCuda_cosh(    const double x){ return cosh(x);  }
__device__ inline float  occaCuda_fastCosh(const float x){  return coshf(x); }
__device__ inline double occaCuda_fastCosh(const double x){ return cosh(x);  }

#define occaCosh       occaCuda_cosh
#define occaFastCosh   occaCuda_fastCosh
#define occaNativeCosh occaCosh

__device__ inline float  occaCuda_acosh(const float x){      return acoshf(x); }
__device__ inline double occaCuda_acosh    (const double x){ return acosh(x);  }
__device__ inline float  occaCuda_fastAcosh(const float x){  return acoshf(x); }
__device__ inline double occaCuda_fastAcosh(const double x){ return acosh(x);  }

#define occaAcosh       occaCuda_acosh
#define occaFastAcosh   occaCuda_fastAcosh
#define occaNativeAcosh occaAcosh

__device__ inline float  occaCuda_tan(const float x){      return tanf(x);   }
__device__ inline double occaCuda_tan(const double x){     return tan(x);    }
__device__ inline float  occaCuda_fastTan(const float x){  return __tanf(x); }
__device__ inline double occaCuda_fastTan(const double x){ return tan(x);    }

#define occaTan       occaCuda_tan
#define occaFastTan   occaCuda_fastTan
#define occaNativeTan occaTan

__device__ inline float  occaCuda_atan(const float x){      return atanf(x); }
__device__ inline double occaCuda_atan(const double x){     return atan(x);  }
__device__ inline float  occaCuda_fastAtan(const float x){  return atanf(x); }
__device__ inline double occaCuda_fastAtan(const double x){ return atan(x);  }

#define occaAtan       occaCuda_atan
#define occaFastAtan   occaCuda_fastAtan
#define occaNativeAtan occaAtan

__device__ inline float  occaCuda_tanh(const float x){      return tanhf(x); }
__device__ inline double occaCuda_tanh(    const double x){ return tanh(x);  }
__device__ inline float  occaCuda_fastTanh(const float x){  return tanhf(x); }
__device__ inline double occaCuda_fastTanh(const double x){ return tanh(x);  }

#define occaTanh       occaCuda_tanh
#define occaFastTanh   occaCuda_fastTanh
#define occaNativeTanh occaTanh

__device__ inline float  occaCuda_atanh(const float x){      return atanhf(x); }
__device__ inline double occaCuda_atanh(const double x){     return atanh(x);  }
__device__ inline float  occaCuda_fastAtanh(const float x){  return atanhf(x); }
__device__ inline double occaCuda_fastAtanh(const double x){ return atanh(x);  }

#define occaAtanh       occaCuda_atanh
#define occaFastAtanh   occaCuda_fastAtanh
#define occaNativeAtanh occaAtanh

__device__ inline float  occaCuda_exp(const float x){      return expf(x);   }
__device__ inline double occaCuda_exp(const double x){     return exp(x);    }
__device__ inline float  occaCuda_fastExp(const float x){  return __expf(x); }
__device__ inline double occaCuda_fastExp(const double x){ return exp(x);    }

#define occaExp       occaCuda_exp
#define occaFastExp   occaCuda_fastExp
#define occaNativeExp occaExp

__device__ inline float  occaCuda_pow(const float x, const float p){      return powf(x,p);   }
__device__ inline double occaCuda_pow(const double x, const float p){     return pow(x,p);    }
__device__ inline float  occaCuda_fastPow(const float x, const float p){  return __powf(x,p); }
__device__ inline double occaCuda_fastPow(const double x, const float p){ return pow(x,p);    }

#define occaPow       occaCuda_pow
#define occaFastPow   occaCuda_fastPow
#define occaNativePow occaPow

__device__ inline float  occaCuda_log2(const float x){      return log2f(x);   }
__device__ inline double occaCuda_log2(    const double x){ return log2(x);    }
__device__ inline float  occaCuda_fastLog2(const float x){  return __log2f(x); }
__device__ inline double occaCuda_fastLog2(const double x){ return log2(x);    }

#define occaLog2       occaCuda_log2
#define occaFastLog2   occaCuda_fastLog2
#define occaNativeLog2 occaLog2

__device__ inline float  occaCuda_log10(const float x){      return log10f(x);   }
__device__ inline double occaCuda_log10(const double x){     return log10(x);    }
__device__ inline float  occaCuda_fastLog10(const float x){  return __log10f(x); }
__device__ inline double occaCuda_fastLog10(const double x){ return log10(x);    }

#define occaLog10       occaCuda_log10
#define occaFastLog10   occaCuda_fastLog10
#define occaNativeLog10 occaLog10
//================================================


//---[ Misc ]-------------------------------------
#define occaParallelFor2
#define occaParallelFor1
#define occaParallelFor0
#define occaParallelFor
// - - - - - - - - - - - - - - - - - - - - - - - -
#define occaUnroll3(N) _Pragma(#N)
#define occaUnroll2(N) occaUnroll3(N)
#define occaUnroll(N)  occaUnroll2(unroll N)
//================================================


//---[ Private ]---------------------------------
#define occaPrivateArray( TYPE , NAME , SIZE ) TYPE NAME[SIZE]
#define occaPrivate( TYPE , NAME )             TYPE NAME
//================================================


//---[ Texture ]----------------------------------
#define occaReadOnly  const
#define occaWriteOnly

#define occaSampler(TEX) __occa__##TEX##__sampler__

#define occaTexture1D(TEX) cudaSurfaceObject_t TEX, int occaSampler(TEX)
#define occaTexture2D(TEX) cudaSurfaceObject_t TEX, int occaSampler(TEX)

#define occaTexGet1D(TEX, TYPE, VALUE, X)    surf1Dread(&(VALUE), TEX, X*sizeof(TYPE)   , (cudaSurfaceBoundaryMode) occaSampler(TEX))
#define occaTexGet2D(TEX, TYPE, VALUE, X, Y) surf2Dread(&(VALUE), TEX, X*sizeof(TYPE), Y, (cudaSurfaceBoundaryMode) occaSampler(TEX))

#define occaTexSet1D(TEX, TYPE, VALUE, X)    surf1Dwrite(VALUE, TEX, X*sizeof(TYPE)   , (cudaSurfaceBoundaryMode) occaSampler(TEX))
#define occaTexSet2D(TEX, TYPE, VALUE, X, Y) surf2Dwrite(VALUE, TEX, X*sizeof(TYPE), Y, (cudaSurfaceBoundaryMode) occaSampler(TEX))
//================================================

#endif
