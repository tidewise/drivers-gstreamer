#include <gstreamer/memory.hpp>

#include <gst/gstbin.h>
#include <stdexcept>

using namespace gstreamer;

void memory::PipelineDestructor::operator()(GstElement* pipeline) const
{
    if (!GST_IS_BIN(pipeline)) {
        throw std::invalid_argument("");
    }

    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
}
