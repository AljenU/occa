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

#include <occa/defines.hpp>

#if OCCA_CUDA_ENABLED

#include <occa/modes/cuda/kernel.hpp>
#include <occa/modes/cuda/device.hpp>
#include <occa/modes/cuda/utils.hpp>
#include <occa/tools/env.hpp>
#include <occa/io.hpp>
#include <occa/tools/misc.hpp>
#include <occa/tools/sys.hpp>
#include <occa/base.hpp>

namespace occa {
  namespace cuda {
    kernel::kernel(const occa::properties &properties_) :
      occa::kernel_v(properties_) {}

    kernel::~kernel() {}

    void kernel::build(const std::string &filename,
                       const std::string &kernelName,
                       const hash_t hash) {

      name = kernelName;

      const bool verbose = properties.get("verbose", false);

      if (properties.get<std::string>("compilerFlags").find("-arch=sm_") == std::string::npos) {
        cuda::device &dev = *((cuda::device*) dHandle);
        const int major = dev.archMajorVersion;
        const int minor = dev.archMinorVersion;
        std::stringstream ss;
        ss << " -arch=sm_" << major << minor << ' ';
        properties["compilerFlags"].string() += ss.str();
      }

      const std::string sourceFile    = getSourceFilename(filename, hash);
      const std::string binaryFile    = getBinaryFilename(filename, hash);
      const std::string ptxBinaryFile = io::hashDir(filename, hash) + "ptxBinary.o";
      bool foundBinary = true;

      io::lock_t lock(hash, "cuda-kernel");
      if (lock.isMine()) {
        if (sys::fileExists(binaryFile)) {
          lock.release();
        } else {
          foundBinary = false;
        }
      }

      if (foundBinary) {
        if (verbose) {
          std::cout << "Loading cached ["
                    << kernelName
                    << "] from ["
                    << io::shortname(filename)
                    << "] in [" << io::shortname(binaryFile) << "]\n";
        }
        return buildFromBinary(binaryFile, kernelName);
      }

      const std::string cachedSourceFile = (
        io::cacheFile(filename,
                      kc::sourceFile,
                      hash,
                      assembleHeader(properties),
                      properties["footer"])
      );

      if (verbose) {
        std::cout << "Compiling [" << kernelName << "]\n";
      }

      //---[ PTX Check Command ]--------
      std::stringstream command;
      if (properties.has("compilerEnvScript")) {
        command << properties["compilerEnvScript"] << " && ";
      }

      command << properties["compiler"]
              << ' ' << properties["compilerFlags"]
              << " -Xptxas -v,-dlcm=cg"
#if (OCCA_OS == OCCA_WINDOWS_OS)
              << " -D OCCA_OS=OCCA_WINDOWS_OS -D _MSC_VER=1800"
#endif
              << " -I"        << env::OCCA_DIR << "include"
              << " -L"        << env::OCCA_DIR << "lib -locca"
              << " -x cu -c " << cachedSourceFile
              << " -o "       << ptxBinaryFile;

      if (!verbose) {
        command << " > /dev/null 2>&1";
      }
      const std::string &ptxCommand = command.str();
      if (verbose) {
        std::cout << "Compiling [" << kernelName << "]\n" << ptxCommand << "\n";
      }

#if (OCCA_OS & (OCCA_LINUX_OS | OCCA_MACOS_OS))
      ignoreResult( system(ptxCommand.c_str()) );
#else
      ignoreResult( system(("\"" +  ptxCommand + "\"").c_str()) );
#endif
      //================================

      //---[ Compiling Command ]--------
      command.str("");
      command << properties["compiler"]
              << ' ' << properties["compilerFlags"]
              << " -ptx"
#if (OCCA_OS == OCCA_WINDOWS_OS)
              << " -D OCCA_OS=OCCA_WINDOWS_OS -D _MSC_VER=1800"
#endif
              << " -I"        << env::OCCA_DIR << "include"
              << " -L"        << env::OCCA_DIR << "lib -locca"
              << " -x cu " << cachedSourceFile
              << " -o "    << binaryFile;

      if (!verbose) {
        command << " > /dev/null 2>&1";
      }
      const std::string &sCommand = command.str();
      if (verbose) {
        std::cout << sCommand << '\n';
      }

      const int compileError = system(sCommand.c_str());

      if (compileError) {
        lock.release();
        OCCA_FORCE_ERROR("Compilation error");
      }
      //================================

      const CUresult moduleLoadError = cuModuleLoad(&cuModule,
                                                    binaryFile.c_str());

      if (moduleLoadError) {
        lock.release();
        OCCA_CUDA_ERROR("Kernel (" + name + ") : Loading Module",
                        moduleLoadError);
      }

      const CUresult moduleGetFunctionError = cuModuleGetFunction(&cuFunction,
                                                                  cuModule,
                                                                  name.c_str());

      if (moduleGetFunctionError) {
        lock.release();
        OCCA_CUDA_ERROR("Kernel (" + name + ") : Loading Function",
                        moduleGetFunctionError);
      }
    }

    void kernel::buildFromBinary(const std::string &filename,
                                 const std::string &kernelName) {

      name = kernelName;

      OCCA_CUDA_ERROR("Kernel (" + kernelName + ") : Loading Module",
                      cuModuleLoad(&cuModule, filename.c_str()));

      OCCA_CUDA_ERROR("Kernel (" + kernelName + ") : Loading Function",
                      cuModuleGetFunction(&cuFunction, cuModule, kernelName.c_str()));
    }

    int kernel::maxDims() const {
      return 3;
    }

    dim kernel::maxOuterDims() const {
      return dim(-1, -1, -1);
    }

    dim kernel::maxInnerDims() const {
      static dim innerDims(0);
      if (innerDims.x == 0) {
        int maxSize;
        OCCA_CUDA_ERROR("Kernel: Getting Maximum Inner-Dim Size",
                        cuFuncGetAttribute(&maxSize,
                                           CU_FUNC_ATTRIBUTE_MAX_THREADS_PER_BLOCK,
                                           cuFunction));

        innerDims.x = maxSize;
      }
      return innerDims;
    }

    void kernel::run() const {
      if (launcherKernel) {
        return launcherRun();
      }

      const int totalArgCount = kernelArg::argumentCount(arguments);
      if (vArgs.size() < totalArgCount) {
        vArgs.resize(totalArgCount);
      }

      const int kArgCount = (int) arguments.size();

      int argc = 0;
      for (int i = 0; i < kArgCount; ++i) {
        kArgVector &iArgs = arguments[i].args;
        const int argCount = (int) iArgs.size();
        if (!argCount) {
          continue;
        }
        for (int ai = 0; ai < argCount; ++ai) {
          vArgs[argc++] = iArgs[ai].ptr();
        }
      }

      OCCA_CUDA_ERROR("Launching Kernel",
                      cuLaunchKernel(cuFunction,
                                     outer.x, outer.y, outer.z,
                                     inner.x, inner.y, inner.z,
                                     0, *((CUstream*) dHandle->currentStream),
                                     &(vArgs[0]), 0));
    }

    void kernel::launcherRun() const {
      launcherKernel->arguments = arguments;
      launcherKernel->arguments.insert(
        launcherKernel->arguments.begin(),
        &(cuKernels[0]),
      );

      int kernelCount = (int) cuKernels.size();
      for (int i = 0; i < kernelCount; ++i) {
        cuKernels[i]->arguments = arguments;
      }

      launcherKernel->run();
    }

    void kernel::free() {
      if (cuModule) {
        OCCA_CUDA_ERROR("Kernel (" + name + ") : Unloading Module",
                        cuModuleUnload(cuModule));
        cuModule = NULL;
      }
    }
  }
}

#endif
