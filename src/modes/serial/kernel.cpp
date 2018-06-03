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

#include "occa/modes/serial/kernel.hpp"
#include "occa/tools/env.hpp"
#include "occa/tools/io.hpp"
#include "occa/base.hpp"

namespace occa {
  namespace serial {
    kernel::kernel(const occa::properties &properties_) :
      occa::kernel_v(properties_) {
      dlHandle = NULL;
      handle   = NULL;
    }

    kernel::~kernel() {}

    void kernel::build(const std::string &filename,
                       const std::string &kernelName,
                       const hash_t hash) {

      name = kernelName;

      const bool verbose = properties.get("verbose", false);

      const std::string sourceFile = getSourceFilename(filename, hash);
      const std::string binaryFile = getBinaryFilename(filename, hash);
      const std::string sourceBasename = kc::sourceFile;
      bool foundBinary = true;

      const std::string hashTag = "serial-kernel";
      if (!io::haveHash(hash, hashTag)) {
        io::waitForHash(hash, hashTag);
      } else if (sys::fileExists(binaryFile)) {
        io::releaseHash(hash, hashTag);
      } else {
        foundBinary = false;
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
                      sourceBasename,
                      hash,
                      assembleHeader(properties),
                      properties["footer"].string())
      );

      std::stringstream command;
      const std::string &compilerEnvScript = properties["compilerEnvScript"].string();
      if (compilerEnvScript.size()) {
        command << compilerEnvScript << " && ";
      }

#if (OCCA_OS & (OCCA_LINUX_OS | OCCA_MACOS_OS))
      command << properties["compiler"].string()
              << ' '    << properties["compilerFlags"].string()
              << ' '    << cachedSourceFile
              << " -o " << binaryFile
              << " -I"  << env::OCCA_DIR << "include"
              << " -L"  << env::OCCA_DIR << "lib -locca"
              << std::endl;
#else
      command << properties["compiler"]
              << " /D MC_CL_EXE"
              << " /D OCCA_OS=OCCA_WINDOWS_OS"
              << " /EHsc"
              << " /wd4244 /wd4800 /wd4804 /wd4018"
              << ' '       << properties["compilerFlags"]
              << " /I"     << env::OCCA_DIR << "/include"
              << ' '       << sourceFile
              << " /link " << env::OCCA_DIR << "lib/libocca.lib",
              << " /OUT:"  << binaryFile
              << std::endl;
#endif

      const std::string &sCommand = command.str();

      if (verbose) {
        std::cout << "Compiling [" << kernelName << "]\n" << sCommand << "\n";
      }

#if (OCCA_OS & (OCCA_LINUX_OS | OCCA_MACOS_OS))
      const int compileError = system(sCommand.c_str());
#else
      const int compileError = system(("\"" +  sCommand + "\"").c_str());
#endif

      if (compileError) {
        io::releaseHash(hash, hashTag);
        OCCA_ERROR("Compilation error", compileError);
      }

      dlHandle = sys::dlopen(binaryFile, hash, hashTag);
      handle   = sys::dlsym(dlHandle, kernelName, hash, hashTag);

      io::releaseHash(hash, hashTag);
    }

    void kernel::buildFromBinary(const std::string &filename,
                                 const std::string &kernelName) {

      name = kernelName;

      dlHandle = sys::dlopen(filename);
      handle   = sys::dlsym(dlHandle, kernelName);
    }

    int kernel::maxDims() const {
      return 3;
    }

    dim kernel::maxOuterDims() const {
      return dim(-1,-1,-1);
    }

    dim kernel::maxInnerDims() const {
      return dim(-1,-1,-1);
    }

    void kernel::runFromArguments(const int kArgc, const kernelArg *kArgs) const {
      int argc = 0;

      for (int i = 0; i < kArgc; ++i) {
        const int argCount = (int) kArgs[i].args.size();
        if (argCount) {
          const kernelArgData *kArgs_i = &(kArgs[i].args[0]);
          for (int j = 0; j < argCount; ++j) {
            vArgs[argc++] = kArgs_i[j].ptr();
          }
        }
      }

      sys::runFunction(handle, argc, vArgs);
    }

    void kernel::free() {
      if (dlHandle) {
        sys::dlclose(dlHandle);
        dlHandle = NULL;
      }
    }
  }
}
