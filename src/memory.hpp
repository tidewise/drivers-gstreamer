#ifndef GSTREAMER_MEMORY_HPP
#define GSTREAMER_MEMORY_HPP

#include <gst/gstelement.h>

namespace gstreamer {
    namespace memory {
        struct PipelineDestructor {
            void operator()(GstElement* pipeline) const;
        };
    }
}

#endif