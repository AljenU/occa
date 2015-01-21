#ifndef OCCA_TOOLS_HEADER
#define OCCA_TOOLS_HEADER

#include <iostream>
#include <stdlib.h>
#include <stdint.h>

#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "occaDefines.hpp"
#include "occaParser.hpp"

#if   OCCA_OS == LINUX_OS
#  include <sys/time.h>
#  include <unistd.h>
#elif OCCA_OS == OSX_OS
#  ifdef __clang__
#    include <CoreServices/CoreServices.h>
#    include <mach/mach_time.h>
#  else
#    include <mach/clock.h>
#    include <mach/mach.h>
#  endif
#else
#  undef UNICODE
#  include <windows.h>
#  include <string>
#endif

namespace occa {
  class kernelInfo;

  class mutex_t {
  public:
#if (OCCA_OS == LINUX_OS) || (OCCA_OS == OSX_OS)
    pthread_mutex_t mutexHandle;
#else
    HANDLE mutexHandle;
#endif

    mutex_t();
    void free();

    void lock();
    void unlock();
  };

  double currentTime();

  std::string getFileExtension(const std::string &filename);

  void getFilePrefixAndName(const std::string &fullFilename,
                            std::string &prefix,
                            std::string &filename);

  std::string getMidCachedBinaryName(const std::string &cachedBinary,
                                     const std::string &namePrefix);

  std::string getFileLock(const std::string &filename);

  bool haveFile(const std::string &filename);
  void waitForFile(const std::string &filename);
  void releaseFile(const std::string &filename);

  parsedKernelInfo parseFileForFunction(const std::string &filename,
                                        const std::string &cachedBinary,
                                        const std::string &functionName,
                                        const kernelInfo &info);

  std::string fnv(const std::string &saltedString);

  bool fileExists(const std::string &filename);

  std::string readFile(const std::string &filename);

  void writeToFile(const std::string &filename,
                   const std::string &content);

  std::string getOCCADir();
  std::string getCachePath();

  bool fileNeedsParser(const std::string &filename);

  std::string getCacheHash(const std::string &content,
                           const std::string &salt);

  std::string getCachedName(const std::string &filename,
                            const std::string &salt);

  std::string getContentCachedName(const std::string &content,
                                   const std::string &salt);

  std::string createIntermediateSource(const std::string &filename,
                                       const std::string &cachedBinary,
                                       const kernelInfo &info);
};

#endif
