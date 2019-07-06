#include <occa/defines.hpp>

#if OCCA_METAL_ENABLED
#  ifndef OCCA_MODES_METAL_MEMORY_HEADER
#  define OCCA_MODES_METAL_MEMORY_HEADER

#include <occa/core/memory.hpp>
#include <occa/modes/metal/headers.hpp>

namespace occa {
  namespace metal {
    class device;

    class memory : public occa::modeMemory_t {
      friend class metal::device;

    private:
      metalBuffer_t metalBuffer;

    public:
      memory(modeDevice_t *modeDevice_,
             udim_t size_,
             const occa::properties &properties_ = occa::properties());
      ~memory();

      kernelArg makeKernelArg() const;

      modeMemory_t* addOffset(const dim_t offset);

      void* getPtr(const occa::properties &props);

      void copyTo(void *dest,
                  const udim_t bytes,
                  const udim_t destOffset = 0,
                  const occa::properties &props = occa::properties()) const;

      void copyFrom(const void *src,
                    const udim_t bytes,
                    const udim_t offset = 0,
                    const occa::properties &props = occa::properties());

      void copyFrom(const modeMemory_t *src,
                    const udim_t bytes,
                    const udim_t destOffset = 0,
                    const udim_t srcOffset = 0,
                    const occa::properties &props = occa::properties());
      void detach();
    };
  }
}

#  endif
#endif
