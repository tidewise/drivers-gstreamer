#ifndef GSTREAMER_RTPBIN_HPP
#define GSTREAMER_RTPBIN_HPP

#include <gst/gstbin.h>
#include <memory>

#include <gstreamer/memory.hpp>

namespace gstreamer {
    namespace rtpbin {
        struct PipelineMapping {
            int session_id{-1};
            std::string rtp_source;
            std::string rtp_sink;
            std::string rtcp_source;
            std::string rtcp_feedback_sink;

            bool undefined() const;
        };

        void linkWithPipelineSrc(GstBin& pipeline,
            std::string const& pipeline_src_name,
            memory::GstUnrefGuard<GstPad>& rtpbin_sink);

        void linkWithPipelineSink(GstBin& pipeline,
            std::string const& pipeline_sink_name,
            memory::GstUnrefGuard<GstPad>& rtpbin_source);

        std::string recv_rtp_sinkpad(std::string const& session_id);
        std::string send_rtp_sinkpad(std::string const& session_id);
        std::string rtcp_sinkpad(std::string const& session_id);
        std::string rtcp_srcpad(std::string const& session_id);
        std::string fec_sinkpad(std::string const& session_id,
            std::string const& fec_stream_index);
        std::string fec_srcpad(std::string const& session_id,
            std::string const& fec_stream_index);

        /**
         * Common RTCP sender/receiver setup
         */
        void setupRTCP(std::shared_ptr<GstBin> pipeline,
            memory::GstUnrefGuard<GstElement>& rtpbin,
            PipelineMapping const& mapping);
    }
}

#endif