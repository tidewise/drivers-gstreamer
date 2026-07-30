#include <gstreamer/memory.hpp>

#include <gst/gstbin.h>
#include <stdexcept>

using namespace gstreamer;

void memory::PipelineDestructor::operator()(GstBin* pipeline) const
{
    gst_element_set_state(GST_ELEMENT(pipeline), GST_STATE_NULL);
    gst_object_unref(pipeline);
}
