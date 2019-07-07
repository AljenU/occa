#ifndef OCCA_API_METAL_EVENT_HEADER
#define OCCA_API_METAL_EVENT_HEADER

namespace occa {
  namespace api {
    namespace metal {
      class event_t {
       public:
        void *eventObj;

        event_t(void *eventObj_ = NULL);
        event_t(const event_t &other);

        void free();

        double getTime() const;
      };
    }
  }
}

#endif
