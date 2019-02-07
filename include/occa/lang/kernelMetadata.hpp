#ifndef OCCA_LANG_KERNELMETADATA_HEADER
#define OCCA_LANG_KERNELMETADATA_HEADER

#include <occa/tools/json.hpp>
#include <occa/dtype.hpp>

namespace occa {
  namespace lang {
    class kernelMetadata;

    typedef std::map<std::string, kernelMetadata> kernelMetadataMap;

    class argumentInfo {
    public:
      bool isConst;
      dtype type;

      argumentInfo(const bool isConst_ = false,
                   const dtype &type_ = dtypes::byte);

      static argumentInfo fromJson(const json &j);
      json toJson() const;
    };

    class kernelMetadata {
    public:
      std::string name;
      std::vector<argumentInfo> arguments;

      kernelMetadata();

      kernelMetadata& operator += (const argumentInfo &argInfo);

      bool argIsConst(const int pos) const;
      bool argMatchesDtype(const int pos,
                           const dtype &type) const;

      static kernelMetadata fromJson(const json &j);
      json toJson() const;
    };

    kernelMetadataMap getBuildFileMetadata(const std::string &filename);
  }
}

#endif
