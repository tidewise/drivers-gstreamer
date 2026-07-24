#ifndef GSTREAMER_RTPBIN_HPP
#define GSTREAMER_RTPBIN_HPP

#include <gst/gstbin.h>
#include <string>

#include <gstreamer/memory.hpp>

namespace gstreamer {
    namespace rtpbin {
        void linkWithPipelineSrc(GstBin& pipeline,
            std::string const& pipeline_src_name,
            memory::GstUnrefGuard<GstPad>& rtpbin_sink);

        void linkWithPipelineSink(GstBin& pipeline,
            std::string const& pipeline_sink_name,
            memory::GstUnrefGuard<GstPad>& rtpbin_source);

        std::string rtp_sinkpad(std::string const& session_id);
        std::string rtcp_sinkpad(std::string const& session_id);
        std::string rtcp_srcpad(std::string const& session_id);
        std::string fec_sink(std::string const& session_id,
            std::string const& fec_stream_index);
    }
}

#endif