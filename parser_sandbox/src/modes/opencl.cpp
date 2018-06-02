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
#include "modes/opencl.hpp"
#include "modes/okl.hpp"
#include "builtins/attributes.hpp"

namespace occa {
  namespace lang {
    namespace okl {
      openclParser::openclParser() {
        addAttribute<attributes::kernel>();
        addAttribute<attributes::outer>();
        addAttribute<attributes::inner>();
        addAttribute<attributes::shared>();
        addAttribute<attributes::exclusive>();

        settings["opencl/extensions/cl_khr_fp64"] = true;
      }

      void openclParser::afterParsing() {
        if (!success) return;
        if (settings.get("okl/validate", true)) {
          checkKernels(root);
        }

        if (!success) return;
        addExtensions();

        if (!success) return;
        addFunctionPrototypes();

        if (!success) return;
        updateConstToConstant();

        if (!success) return;
        addOccaFors();

        if (!success) return;
        setupKernelArgs();

        if (!success) return;
        setupLaunchKernel();
      }

      void openclParser::addExtensions() {
        if (!settings.has("opencl/extensions")) {
          return;
        }

        occa::json &extensions = settings["opencl/extensions"];
        if (!extensions.isObject()) {
          return;
        }

        jsonObject &extensionObj = extensions.object();
        jsonObject::iterator it = extensionObj.begin();
        while (it != extensionObj.end()) {
          const std::string &extension = it->first;
          const bool enabled = it->second;
          if (enabled) {
            root.addFirst(
              *(new pragmaStatement(
                  &root,
                  pragmaToken(root.source->origin,
                              "OPENCL EXTENSION "+ extension + " : enable")
                ))
            );
          }
          ++it;
        }
      }

      void openclParser::addFunctionPrototypes() {
      }

      void openclParser::updateConstToConstant() {
      }

      void openclParser::addOccaFors() {
      }

      void openclParser::setupKernelArgs() {
      }

      void openclParser::setupLaunchKernel() {
      }
    }
  }
}
