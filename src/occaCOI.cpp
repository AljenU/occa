#if OCCA_COI_ENABLED

#include "occaCOI.hpp"

namespace occa {
  //---[ Kernel ]---------------------
  template <>
  kernel_t<COI>::kernel_t(){
    data = NULL;
    dev  = NULL;

    functionName = "";

    dims  = 1;
    inner = occa::dim(1,1,1);
    outer = occa::dim(1,1,1);

    preferredDimSize_ = 0;

    startTime = (void*) new coiEvent;
    endTime   = (void*) new coiEvent;
  }

  template <>
  kernel_t<COI>::kernel_t(const kernel_t<COI> &k){
    data = k.data;
    dev  = k.dev;

    functionName = k.functionName;

    dims  = k.dims;
    inner = k.inner;
    outer = k.outer;

    preferredDimSize_ = k.preferredDimSize_;

    startTime = k.startTime;
    endTime   = k.endTime;
  }

  template <>
  kernel_t<COI>& kernel_t<COI>::operator = (const kernel_t<COI> &k){
    data = k.data;
    dev  = k.dev;

    functionName = k.functionName;

    dims  = k.dims;
    inner = k.inner;
    outer = k.outer;

    preferredDimSize_ = k.preferredDimSize_;

    *((coiEvent*) startTime) = *((coiEvent*) k.startTime);
    *((coiEvent*) endTime)   = *((coiEvent*) k.endTime);

    return *this;
  }

  template <>
  kernel_t<COI>::~kernel_t(){}

  template <>
  kernel_t<COI>* kernel_t<COI>::buildFromSource(const std::string &filename,
                                                          const std::string &functionName_,
                                                          const kernelInfo &info_){
    functionName = functionName_;

    kernelInfo info = info_;
    info.addDefine("OCCA_USING_CPU", 1);
    info.addDefine("OCCA_USING_COI", 1);

    info.addOCCAKeywords(occaCOIDefines);

    std::stringstream salt;
    salt << "COI"
         << info.salt()
         << functionName;

    std::string cachedBinary = binaryIsCached(filename, salt.str());

    struct stat buffer;
    bool fileExists = (stat(cachedBinary.c_str(), &buffer) == 0);

    if(fileExists){
      std::cout << "Found cached binary of [" << filename << "] in [" << cachedBinary << "]\n";
      return buildFromBinary(cachedBinary, functionName);
    }

    data = new COIKernelData_t;

    std::string iCachedBinary = createIntermediateSource(filename,
                                                         cachedBinary,
                                                         info);

    std::stringstream command;

    command << dev->dHandle->compiler
            << " -o " << cachedBinary
            << " -x c++ -w -fPIC -shared"
            << ' '    << dev->dHandle->compilerFlags
            << ' '    << info.flags
            << ' '    << iCachedBinary;

    const std::string &sCommand = command.str();

    std::cout << sCommand << '\n';

    system(sCommand.c_str());

    OCCA_EXTRACT_DATA(COI, Kernel);

    COILIBRARY outLibrary;

    OCCA_COI_CHECK("Kernel: Loading Kernel To Chief",
                   COIProcessLoadLibraryFromFile(data_.chiefID,
                                                 NULL,
                                                 cachedBinary.c_str(),
                                                 NULL,
                                                 &outLibrary));

    return this;
  }

  template <>
  kernel_t<COI>* kernel_t<COI>::buildFromBinary(const std::string &filename,
                                                          const std::string &functionName_){
    data = new COIKernelData_t;

    OCCA_EXTRACT_DATA(COI, Kernel);

    functionName = functionName_;

    COILIBRARY outLibrary;

    OCCA_COI_CHECK("Kernel: Loading Kernel To Chief",
                   COIProcessLoadLibraryFromFile(data_.chiefID,
                                                 NULL,
                                                 cachedBinary.c_str(),
                                                 NULL,
                                                 &outLibrary));

    return this;
  }

  // [-] Missing
  template <>
  int kernel_t<COI>::preferredDimSize(){
    preferredDimSize_ = 1;

    return preferredDimSize_;
  }

  OCCA_COI_KERNEL_OPERATOR_DEFINITIONS;

  template <>
  double kernel_t<COI>::timeTaken(){
    const double &start = *((double*) startTime);
    const double &end   = *((double*) endTime);

    return 1.0e3*(end - start);
  }

  template <>
  void kernel_t<COI>::free(){
  }
  //==================================


  //---[ Memory ]---------------------
  template <>
  memory_t<COI>::memory_t(){
    handle = NULL;
    dev    = NULL;
    size = 0;
  }

  template <>
  memory_t<COI>::memory_t(const memory_t<COI> &m){
    handle = m.handle;
    dev    = m.dev;
    size   = m.size;
  }

  template <>
  memory_t<COI>& memory_t<COI>::operator = (const memory_t<COI> &m){
    handle = m.handle;
    dev    = m.dev;
    size   = m.size;

    return *this;
  }

  template <>
  memory_t<COI>::~memory_t(){}

  template <>
  void memory_t<COI>::copyFrom(const void *source,
                               const size_t bytes,
                               const size_t offset){
    coiStream &stream = *((coiStream*) dev->currentStream);

    const size_t bytes_ = (bytes == 0) ? size : bytes;

    OCCA_CHECK((bytes_ + offset) <= size);

    coiEvent lastEvent = stream.lastEvent;

    OCCA_COI_CHECK("Memory: Copy From",
                   COIBufferCopy(*((coiMemory*) handle),
                                 offset,
                                 source,
                                 bytes,
                                 COI_COPY_UNSPECIFIED,
                                 lastEvent != NULL,
                                 &lastEvent,
                                 &(stream.lastEvent)) );
  }

  template <>
  void memory_t<COI>::copyFrom(const memory_v *source,
                               const size_t bytes,
                               const size_t destOffset,
                               const size_t srcOffset){
    coiStream &stream = *((coiStream*) dev->currentStream);

    const size_t bytes_ = (bytes == 0) ? size : bytes;

    OCCA_CHECK((bytes_ + offset) <= size);

    coiEvent lastEvent = stream.lastEvent;

    OCCA_COI_CHECK("Memory: Copy From",
                   COIBufferCopy(*((coiMemory*) handle),
                                 *((coiMemory*) source->handle),
                                 destOffset,
                                 srcOffset,
                                 bytes,
                                 COI_COPY_UNSPECIFIED,
                                 lastEvent != NULL,
                                 &lastEvent,
                                 &(stream.lastEvent)) );
  }

  template <>
  void memory_t<COI>::copyTo(void *dest,
                             const size_t bytes,
                             const size_t offset){
    coiStream &stream = *((coiStream*) dev->currentStream);

    const size_t bytes_ = (bytes == 0) ? size : bytes;

    OCCA_CHECK((bytes_ + offset) <= size);

    coiEvent lastEvent = stream.lastEvent;

    OCCA_COI_CHECK("Memory: Copy From",
                   COIBufferCopy(*((coiMemory*) handle),
                                 offset,
                                 dest,
                                 bytes,
                                 COI_COPY_UNSPECIFIED,
                                 lastEvent != NULL,
                                 &lastEvent,
                                 &(stream.lastEvent)) );
  }

  template <>
  void memory_t<COI>::copyTo(memory_v *dest,
                             const size_t bytes,
                             const size_t destOffset,
                             const size_t srcOffset){
    coiStream &stream = *((coiStream*) dev->currentStream);

    const size_t bytes_ = (bytes == 0) ? size : bytes;

    OCCA_CHECK((bytes_ + offset) <= size);

    coiEvent lastEvent = stream.lastEvent;

    OCCA_COI_CHECK("Memory: Copy From",
                   COIBufferCopy(*((coiMemory*) dest->handle),
                                 *((coiMemory*) handle),
                                 destOffset,
                                 srcOffset,
                                 bytes,
                                 COI_COPY_UNSPECIFIED,
                                 lastEvent != NULL,
                                 &lastEvent,
                                 &(stream.lastEvent)) );
  }

  template <>
  void memory_t<COI>::asyncCopyFrom(const void *source,
                                    const size_t bytes,
                                    const size_t offset){
    coiStream &stream = *((coiStream*) dev->currentStream);

    const size_t bytes_ = (bytes == 0) ? size : bytes;

    OCCA_CHECK((bytes_ + offset) <= size);

    OCCA_COI_CHECK("Memory: Copy From",
                   COIBufferCopy(*((coiMemory*) handle),
                                 offset,
                                 source,
                                 bytes,
                                 COI_COPY_UNSPECIFIED,
                                 0, NULL, NULL) );
  }

  template <>
  void memory_t<COI>::asyncCopyFrom(const memory_v *source,
                                    const size_t bytes,
                                    const size_t destOffset,
                                    const size_t srcOffset){
    coiStream &stream = *((coiStream*) dev->currentStream);

    const size_t bytes_ = (bytes == 0) ? size : bytes;

    OCCA_CHECK((bytes_ + offset) <= size);

    coiEvent lastEvent = stream.lastEvent;

    OCCA_COI_CHECK("Memory: Copy From",
                   COIBufferCopy(*((coiMemory*) handle),
                                 *((coiMemory*) source->handle),
                                 destOffset,
                                 srcOffset,
                                 bytes,
                                 COI_COPY_UNSPECIFIED,
                                 0, NULL, NULL) );
  }

  template <>
  void memory_t<COI>::asyncCopyTo(void *dest,
                                  const size_t bytes,
                                  const size_t offset){
    coiStream &stream = *((coiStream*) dev->currentStream);

    const size_t bytes_ = (bytes == 0) ? size : bytes;

    OCCA_CHECK((bytes_ + offset) <= size);

    coiEvent lastEvent = stream.lastEvent;

    OCCA_COI_CHECK("Memory: Copy From",
                   COIBufferCopy(*((coiMemory*) handle),
                                 offset,
                                 dest,
                                 bytes,
                                 COI_COPY_UNSPECIFIED,
                                 0, NULL, NULL) );
  }

  template <>
  void memory_t<COI>::asyncCopyTo(memory_v *dest,
                                  const size_t bytes,
                                  const size_t destOffset,
                                  const size_t srcOffset){
    coiStream &stream = *((coiStream*) dev->currentStream);

    const size_t bytes_ = (bytes == 0) ? size : bytes;

    OCCA_CHECK((bytes_ + offset) <= size);

    coiEvent lastEvent = stream.lastEvent;

    OCCA_COI_CHECK("Memory: Copy From",
                   COIBufferCopy(*((coiMemory*) dest->handle),
                                 *((coiMemory*) handle),
                                 destOffset,
                                 srcOffset,
                                 bytes,
                                 COI_COPY_UNSPECIFIED,
                                 0, NULL, NULL) );
  }

  template <>
  void memory_t<COI>::free(){
    OCCA_COI_CHECK("Memory: free",
                   COIBufferDestroy( *((coiMemory*) handle) ) );

    ::free(handle);
  }
  //==================================


  //---[ Device ]---------------------
  template <>
  device_t<COI>::device_t() :
    memoryUsed(0) {
    data = NULL;

    getEnvironmentVariables();
  }

  template <>
  device_t<COI>::device_t(int platform, int device) :
    memoryUsed(0) {
    data = NULL;

    getEnvironmentVariables();
  }

  template <>
  device_t<COI>::device_t(const device_t<COI> &d){
    data       = d.data;
    memoryUsed = d.memoryUsed;

    compilerFlags = d.compilerFlags;
  }

  template <>
  device_t<COI>& device_t<COI>::operator = (const device_t<COI> &d){
    data       = d.data;
    memoryUsed = d.memoryUsed;

    compilerFlags = d.compilerFlags;

    return *this;
  }

  template <>
  void device_t<COI>::setup(const int device, const int memoryAllocated){
    data = ::malloc(sizeof(COIDeviceData_t));

    OCCA_EXTRACT_DATA(COI, Device);

    uint32_t deviceCount;
    OCCA_COI_CHECK("Device: Get Count",
                   COIEngineGetCount(COI_ISA_MIC, &deviceCount));

    OCCA_CHECK(device < deviceCount);

    OCCA_COI_CHECK("Device: Get Handle",
                   COIEngineGetHandle(COI_ISA_MIC, device, &data_.deviceID) );

    // [-] Need a simple basic main(argc, argv) binary

    OCCA_COI_CHECK("Device: Initializing",
                   COIProcessCreateFromFile(data_.deviceID,
                                            SINK_NAME,
                                            0    , NULL,
                                            false, NULL,
                                            true , NULL,
                                            memoryAllocated ? memoryAllocated : (4 << 30), // 4 GB
                                            NULL,
                                            &data_.chiefID) );
  }

  template <>
  void device_t<COI>::getEnvironmentVariables(){
    char *c_compilerFlags = getenv("OCCA_COI_COMPILER_FLAGS");
    if(c_compilerFlags != NULL)
      compilerFlags = std::string(c_compilerFlags);
  }

  template <>
  void device_t<COI>::setCompiler(const std::string &compiler_){
    compiler = compiler_;
  }

  template <>
  void device_t<COI>::setCompilerFlags(const std::string &compilerFlags_){
    compilerFlags = compilerFlags_;
  }

  template <>
  void device_t<COI>::flush(){}

  template <>
  void device_t<COI>::finish(){
    coiStream &stream = *((coiStream*) dev->currentStream);

    if(stream.lastEvent != NULL)
      OCCA_COI_CHECK("Device: Waiting for Event",
                     COIEventWait(1, stream.lastEvent,
                                  -1, true, NULL, NULL) );
  }

  template <>
  stream device_t<COI>::genStream(){
    OCCA_EXTRACT_DATA(COI, Device);

    coiStream *retStream = (coiStream*) ::malloc(sizeof(coiStream));

    OCCA_COI_CHECK("Device: Generating a Stream",
                   COIPipelineCreate(data_.chiefID,
                                     NULL, 0,
                                     &(retStream->handle)) );

    retStream->lastEvent = NULL;

    return retStream;
  }

  template <>
  void device_t<COI>::freeStream(stream s){
    coiStream *stream = (coiStream*) s;

    COIPipelineDestroy("Device: Freeing a Stream",
                       stream.handle);

    ::free(stream);
  }

  template <>
  kernel_v* device_t<COI>::buildKernelFromSource(const std::string &filename,
                                                 const std::string &functionName,
                                                 const kernelInfo &info_){
    OCCA_EXTRACT_DATA(COI, Device);

    kernel_v *k = new kernel_t<COI>;

    k->dev  = dev;
    k->data = ::malloc(sizeof(COIKernelData_t));

    COIKernelData_t &kData_ = *((COIKernelData_t*) k->data);

    kData_.chiefID = data_.chiefID;

    k->buildFromSource(filename, functionName, info_);
    return k;
  }

  template <>
  kernel_v* device_t<COI>::buildKernelFromBinary(const std::string &filename,
                                                 const std::string &functionName){
    OCCA_EXTRACT_DATA(COI, Device);

    kernel_v *k = new kernel_t<COI>;

    k->dev  = dev;
    k->data = ::malloc(sizeof(COIKernelData_t));

    COIKernelData_t &kData_ = *((COIKernelData_t*) k->data);

    kData_.chiefID = data_.chiefID;

    k->buildFromBinary(filename, functionName);
    return k;
  }

  template <>
  memory_v* device_t<COI>::malloc(const size_t bytes,
                                  void *source){
    OCCA_EXTRACT_DATA(COI, Device);

    memory_v *mem = new memory_t<COI>;
    cl_int error;

    mem->dev    = dev;
    mem->handle = ::malloc(sizeof(coiMemory));
    mem->size   = bytes;

    OCCA_COI_CHECK("Device: Malloc",
                   COIBufferCreate(bytes,
                                   COI_BUFFER_NORMAL,
                                   COI_SINK_MEMORY,
                                   source,
                                   1,
                                   &(data_.chiefID),
                                   (coiMemory*) mem->handle) );

    return mem;
  }

  template <>
  void device_t<COI>::free(){
    OCCA_EXTRACT_DATA(COI, Device);

    OCCA_COI_CHECK( COIProcessDestroy(data_.chiefID,
                                      -1,
                                      false,
                                      NULL,
                                      NULL ));

    ::free(data);
  }

  template <>
  int device_t<COI>::simdWidth(){
    simdWidth_ = 16; // [-] AVX-512

    return 16;
  }
  //==================================


  //---[ Error Handling ]-------------
  std::string coiError(int e){
    return std::string( COIResultGetName(e) );
  }
  //==================================
};

#endif
