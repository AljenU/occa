#include "occaBase.hpp"

#define LIBOCCA_C_EXPORTS
#include "occaCBase.hpp"

#  ifdef __cplusplus
extern "C" {
#  endif

  // [-] Keep [int type] as the first entry
  struct occaMemory_t {
    int type;
    occa::memory mem;
  };

  // [-] Keep [int type] as the first entry
  struct occaType_t {
    int type;
    occa::kernelArg_t value;
  };

  struct occaArgumentList_t {
    int argc;
    occaMemory argv[100];
  };

  occaKernelInfo occaNoKernelInfo = NULL;

  const uintptr_t occaAutoSize = 0;
  const uintptr_t occaNoOffset = 0;

  const uintptr_t occaTypeSize[OCCA_TYPE_COUNT] = {
    sizeof(void*),
    sizeof(int),
    sizeof(unsigned int),
    sizeof(char),
    sizeof(unsigned char),
    sizeof(short),
    sizeof(unsigned short),
    sizeof(long),
    sizeof(unsigned long),
    sizeof(float),
    sizeof(double),
    sizeof(char *)
  };

  //---[ TypeCasting ]------------------
  occaType LIBOCCA_CALLINGCONV occaInt(int value){
    occaType_t *type = new occaType_t;

    type->type       = OCCA_TYPE_INT;
    type->value.int_ = value;

    return (occaType) type;
  }

  occaType LIBOCCA_CALLING_CONV occaUInt(unsigned int value){
    occaType_t *type = new occaType_t;

    type->type        = OCCA_TYPE_UINT;
    type->value.uint_ = value;

    return (occaType) type;
  }

  occaType LIBOCCA_CALLING_CONV occaChar(char value){
    occaType_t *type = new occaType_t;

    type->type        = OCCA_TYPE_CHAR;
    type->value.char_ = value;

    return (occaType) type;
  }

  occaType LIBOCCA_CALLING_CONV occaUChar(unsigned char value){
    occaType_t *type = new occaType_t;

    type->type         = OCCA_TYPE_UCHAR;
    type->value.uchar_ = value;

    return (occaType) type;
  }

  occaType LIBOCCA_CALLING_CONV occaShort(short value){
    occaType_t *type = new occaType_t;

    type->type         = OCCA_TYPE_SHORT;
    type->value.short_ = value;

    return (occaType) type;
  }

  occaType LIBOCCA_CALLING_CONV occaUShort(unsigned short value){
    occaType_t *type = new occaType_t;

    type->type          = OCCA_TYPE_USHORT;
    type->value.ushort_ = value;

    return (occaType) type;
  }

  occaType LIBOCCA_CALLING_CONV occaLong(long value){
    occaType_t *type = new occaType_t;

    type->type        = OCCA_TYPE_LONG;
    type->value.long_ = value;

    return (occaType) type;
  }

  occaType LIBOCCA_CALLING_CONV occaULong(unsigned long value){
    occaType_t *type = new occaType_t;

    type->type          = OCCA_TYPE_ULONG;
    type->value.uintptr_t_ = value;

    return (occaType) type;
  }

  occaType LIBOCCA_CALLING_CONV occaFloat(float value){
    occaType_t *type = new occaType_t;

    type->type         = OCCA_TYPE_FLOAT;
    type->value.float_ = value;

    return (occaType) type;
  }

  occaType LIBOCCA_CALLING_CONV occaDouble(double value){
    occaType_t *type = new occaType_t;

    type->type          = OCCA_TYPE_DOUBLE;
    type->value.double_ = value;

    return (occaType) type;
  }

  occaType LIBOCCA_CALLING_CONV occaString(char *value){
    occaType_t *type = new occaType_t;

    type->type        = OCCA_TYPE_STRING;
    type->value.void_ = value;

    return (occaType) type;
  }
  //====================================


  //---[ Device ]-----------------------
  const char* LIBOCCA_CALLING_CONV occaDeviceMode(occaDevice device){
    occa::device &device_ = *((occa::device*) device);

    return device_.mode().c_str();
  }

  void LIBOCCA_CALLING_CONV occaDeviceSetCompiler(occaDevice device,
                                                  const char *compiler){
    occa::device &device_ = *((occa::device*) device);
    device_.setCompiler(compiler);
  }

  void LIBOCCA_CALLING_CONV occaDeviceSetCompilerFlags(occaDevice device,
                                                       const char *compilerFlags){
    occa::device &device_ = *((occa::device*) device);
    device_.setCompilerFlags(compilerFlags);
  }


  occaDevice LIBOCCA_CALLING_CONV occaGetDevice(const char *mode,
                                                int arg1, int arg2){
    occa::device *device = new occa::device();

    device->setup(mode, arg1, arg2);

    return (occaDevice) device;
  }

  occaKernel LIBOCCA_CALLING_CONV occaBuildKernelFromSource(occaDevice device,
                                                            const char *filename,
                                                            const char *functionName,
                                                            occaKernelInfo info){
    occa::device &device_  = *((occa::device*) device);

    occa::kernel *kernel = new occa::kernel();

    if(info != occaNoKernelInfo){
      occa::kernelInfo &info_ = *((occa::kernelInfo*) info);

      *kernel = device_.buildKernelFromSource(filename,
                                              functionName,
                                              info_);
    }
    else{
      *kernel = device_.buildKernelFromSource(filename,
                                              functionName);
    }

    return (occaKernel) kernel;
  }

  occaKernel LIBOCCA_CALLING_CONV occaBuildKernelFromBinary(occaDevice device,
                                                            const char *filename,
                                                            const char *functionName){
    occa::device &device_ = *((occa::device*) device);

    occa::kernel *kernel = new occa::kernel();

    *kernel = device_.buildKernelFromBinary(filename, functionName);

    return (occaKernel) kernel;
  }

  occaKernel LIBOCCA_CALLING_CONV occaBuildKernelFromLoopy(occaDevice device,
                                                           const char *filename,
                                                           const char *functionName,
                                                           const char *pythonCode){
    occa::device &device_  = *((occa::device*) device);

    occa::kernel *kernel = new occa::kernel();

    *kernel = device_.buildKernelFromLoopy(filename,
                                           functionName,
                                           pythonCode);

    return (occaKernel) kernel;
  }

  occaMemory LIBOCCA_CALLING_CONV occaDeviceMalloc(occaDevice device,
                                                   uintptr_t bytes,
                                                   void *source){
    occa::device &device_ = *((occa::device*) device);

    occaMemory_t *memory = new occaMemory_t();

    memory->type = OCCA_TYPE_MEMORY;
    memory->mem = device_.malloc(bytes, source);

    return (occaMemory) memory;
  }

  void LIBOCCA_CALLING_CONV occaDeviceFlush(occaDevice device){
    occa::device &device_ = *((occa::device*) device);

    device_.flush();
  }

  void LIBOCCA_CALLING_CONV occaDeviceFinish(occaDevice device){
    occa::device &device_ = *((occa::device*) device);

    device_.finish();
  }

  occaStream LIBOCCA_CALLING_CONV occaDeviceGenStream(occaDevice device){
    occa::device &device_ = *((occa::device*) device);

    return (occaStream) device_.genStream();
  }

  occaStream LIBOCCA_CALLING_CONV occaDeviceGetStream(occaDevice device){
    occa::device &device_ = *((occa::device*) device);

    return (occaStream) device_.getStream();
  }

  void LIBOCCA_CALLING_CONV occaDeviceSetStream(occaDevice device, occaStream stream){
    occa::device &device_ = *((occa::device*) device);
    occa::stream &stream_ = *((occa::stream*) stream);

    device_.setStream(stream_);
  }

  occaTag LIBOCCA_CALLING_CONV occaDeviceTagStream(occaDevice device){
    occa::device &device_ = *((occa::device*) device);

    occa::tag oldTag = device_.tagStream();
    occaTag newTag;

    ::memcpy(&newTag, &oldTag, sizeof(oldTag));

    return newTag;
  }

  double LIBOCCA_CALLING_CONV occaDeviceTimeBetweenTags(occaDevice device,
                                                        occaTag startTag, occaTag endTag){
    occa::device &device_ = *((occa::device*) device);

    occa::tag startTag_, endTag_;

    ::memcpy(&startTag_, &startTag, sizeof(startTag_));
    ::memcpy(&endTag_  , &endTag  , sizeof(endTag_));

    return device_.timeBetween(startTag_, endTag_);
  }

  void LIBOCCA_CALLING_CONV occaDeviceStreamFree(occaDevice device, occaStream stream){
    occa::device &device_ = *((occa::device*) device);
    occa::stream &stream_ = *((occa::stream*) stream);

    device_.free(stream_);
  }

  void LIBOCCA_CALLING_CONV occaDeviceFree(occaDevice device){
    occa::device &device_ = *((occa::device*) device);

    device_.free();

    delete (occa::device*) device;
  }
  //====================================


  //---[ Kernel ]-----------------------
  occaDim LIBOCCA_CALLING_CONV occaGenDim(uintptr_t x, uintptr_t y, uintptr_t z){
    occaDim ret;

    ret.x = x;
    ret.y = y;
    ret.z = z;

    return ret;
  }

  const char* LIBOCCA_CALLING_CONV occaKernelMode(occaKernel kernel){
    occa::kernel &kernel_ = *((occa::kernel*) kernel);

    return kernel_.mode().c_str();
  }

  int LIBOCCA_CALLING_CONV occaKernelPreferredDimSize(occaKernel kernel){
    occa::kernel &kernel_ = *((occa::kernel*) kernel);

    return kernel_.preferredDimSize();
  }

  void LIBOCCA_CALLING_CONV occaKernelSetWorkingDims(occaKernel kernel,
                                                     int dims,
                                                     occaDim items,
                                                     occaDim groups){
    occa::kernel &kernel_ = *((occa::kernel*) kernel);

    kernel_.setWorkingDims(dims,
                           occa::dim(items.x, items.y, items.z),
                           occa::dim(groups.x, groups.y, groups.z));
  }

  void LIBOCCA_CALLING_CONV occaKernelSetAllWorkingDims(occaKernel kernel,
                                                        int dims,
                                                        uintptr_t itemsX, uintptr_t itemsY, uintptr_t itemsZ,
                                                        uintptr_t groupsX, uintptr_t groupsY, uintptr_t groupsZ){
    occa::kernel &kernel_ = *((occa::kernel*) kernel);

    kernel_.setWorkingDims(dims,
                           occa::dim(itemsX, itemsY, itemsZ),
                           occa::dim(groupsX, groupsY, groupsZ));
  }


  double LIBOCCA_CALLING_CONV occaKernelTimeTaken(occaKernel kernel){
    occa::kernel &kernel_ = *((occa::kernel*) kernel);

    return kernel_.timeTaken();
  }

  occaArgumentList LIBOCCA_CALLING_CONV occaGenArgumentList(){
    occaArgumentList_t *list = new occaArgumentList_t();
    list->argc = 0;

    return (occaArgumentList) list;
  }

  void LIBOCCA_CALLING_CONV occaArgumentListClear(occaArgumentList list){
    occaArgumentList_t &list_ = *((occaArgumentList_t*) list);

    for(int i = 0; i < list_.argc; ++i){
      occaType_t &type_ = *((occaType_t*) list_.argv[i]);

      if(type_.type != OCCA_TYPE_MEMORY)
        delete (occaType_t*) list_.argv[i];
    }

    list_.argc = 0;
  }

  void LIBOCCA_CALLING_CONV occaArgumentListFree(occaArgumentList list){
    delete (occaArgumentList_t*) list;
  }

  void LIBOCCA_CALLING_CONV occaArgumentListAddArg(occaArgumentList list,
                                                   int argPos,
                                                   void * type){
    occaArgumentList_t &list_ = *((occaArgumentList_t*) list);

    if(list_.argc < (argPos + 1)){
      OCCA_CHECK(argPos < OCCA_MAX_ARGS);

      list_.argc = (argPos + 1);
    }

    list_.argv[argPos] = (occaMemory_t*) type;
  }

  // Note the _
  //   Macro that is called > API function that is never seen
  void LIBOCCA_CALLING_CONV occaKernelRun_(occaKernel kernel,
                                           occaArgumentList list){
    occa::kernel &kernel_     = *((occa::kernel*) kernel);
    occaArgumentList_t &list_ = *((occaArgumentList_t*) list);

    kernel_.clearArgumentList();

    for(int i = 0; i < list_.argc; ++i){
      occaMemory_t &memory_ = *((occaMemory_t*) list_.argv[i]);

      if(memory_.type == OCCA_TYPE_MEMORY){
        kernel_.addArgument(i, occa::kernelArg(memory_.mem));
      }
      else{
        occaType_t &type_ = *((occaType_t*) list_.argv[i]);

        kernel_.addArgument(i, occa::kernelArg(type_.value,
                                               occaTypeSize[type_.type],
                                               false));
      }
    }

    kernel_.runFromArguments();
  }

  OCCA_C_KERNEL_RUN_DEFINITIONS;

  void LIBOCCA_CALLING_CONV occaKernelFree(occaKernel kernel){
    occa::kernel &kernel_ = *((occa::kernel*) kernel);

    kernel_.free();

    delete (occa::kernel*) kernel;
  }

  occaKernelInfo LIBOCCA_CALLING_CONV occaGenKernelInfo(){
    occa::kernelInfo *info = new occa::kernelInfo();

    return (occaKernelInfo) info;

  }

  void LIBOCCA_CALLING_CONV occaKernelInfoAddDefine(occaKernelInfo info,
                                                    const char *macro,
                                                    occaType value){
    occa::kernelInfo &info_   = *((occa::kernelInfo*) info);
    occa::kernelArg_t &value_ = ((occaType_t*) value)->value;
    const int valueType       = ((occaType_t*) value)->type;

    switch(valueType){
    case OCCA_TYPE_INT    : info_.addDefine(macro, value_.int_);    break;
    case OCCA_TYPE_UINT   : info_.addDefine(macro, value_.uint_);   break;
    case OCCA_TYPE_CHAR   : info_.addDefine(macro, value_.char_);   break;
    case OCCA_TYPE_UCHAR  : info_.addDefine(macro, value_.uchar_);  break;
    case OCCA_TYPE_SHORT  : info_.addDefine(macro, value_.short_);  break;
    case OCCA_TYPE_USHORT : info_.addDefine(macro, value_.ushort_); break;
    case OCCA_TYPE_LONG   : info_.addDefine(macro, value_.long_);   break;
    case OCCA_TYPE_ULONG  : info_.addDefine(macro, value_.uintptr_t_); break;

    case OCCA_TYPE_FLOAT  : info_.addDefine(macro, value_.float_);  break;
    case OCCA_TYPE_DOUBLE : info_.addDefine(macro, value_.double_); break;

    case OCCA_TYPE_STRING : info_.addDefine(macro, std::string((char*) value_.void_)); break;
    default:
      std::cout << "Wrong type input in [occaKernelInfoAddDefine]\n";
    }
  }

  void LIBOCCA_CALLING_CONV occaKernelInfoFree(occaKernelInfo info){
    delete (occa::kernelInfo*) info;
  }
  //====================================


  //---[ Memory ]-----------------------
  const char* LIBOCCA_CALLING_CONV occaMemoryMode(occaMemory memory){
    occa::memory &memory_ = memory->mem;

    return memory_.mode().c_str();
  }

  void LIBOCCA_CALLING_CONV occaCopyMemToMem(occaMemory dest, occaMemory src,
                                             const uintptr_t bytes,
                                             const uintptr_t destOffset,
                                             const uintptr_t srcOffset){
    occa::memory &src_  = src->mem;
    occa::memory &dest_ = dest->mem;

    memcpy(dest_, src_, bytes, destOffset, srcOffset);
  }

  void LIBOCCA_CALLING_CONV occaCopyPtrToMem(occaMemory dest, const void *src,
                                             const uintptr_t bytes,
                                             const uintptr_t offset){
    occa::memory &dest_ = dest->mem;

    memcpy(dest_, src, bytes, offset);
  }

  void LIBOCCA_CALLING_CONV occaCopyMemToPtr(void *dest, occaMemory src,
                                             const uintptr_t bytes,
                                             const uintptr_t offset){
    occa::memory &src_ = src->mem;

    memcpy(dest, src_, bytes, offset);
  }

  void LIBOCCA_CALLING_CONV occaAsyncCopyMemToMem(occaMemory dest, occaMemory src,
                                                  const uintptr_t bytes,
                                                  const uintptr_t destOffset,
                                                  const uintptr_t srcOffset){
    occa::memory &src_  = src->mem;
    occa::memory &dest_ = dest->mem;

    asyncMemcpy(dest_, src_, bytes, destOffset, srcOffset);
  }

  void LIBOCCA_CALLING_CONV occaAsyncCopyPtrToMem(occaMemory dest, const void * src,
                                                  const uintptr_t bytes,
                                                  const uintptr_t offset){
    occa::memory &dest_ = dest->mem;

    asyncMemcpy(dest_, src, bytes, offset);
  }

  void LIBOCCA_CALLING_CONV occaAsyncCopyMemToPtr(void *dest, occaMemory src,
                                                  const uintptr_t bytes,
                                                  const uintptr_t offset){
    occa::memory &src_ = src->mem;

    asyncMemcpy(dest, src_, bytes, offset);
  }

  void LIBOCCA_CALLING_CONV occaMemorySwap(occaMemory memoryA, occaMemory memoryB){
    occa::memory &memoryA_ = memoryA->mem;
    occa::memory &memoryB_ = memoryB->mem;

    memoryA_.swap(memoryB_);
  }


  void LIBOCCA_CALLING_CONV occaMemoryFree(occaMemory memory){
    memory->mem.free();

    delete memory;
  }
  //====================================

#  ifdef __cplusplus
}
#  endif
