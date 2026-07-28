#ifndef GSTREAMER_RTPBIN_SENDER_HPP
#define GSTREAMER_RTPBIN_SENDER_HPP

#include <memory>
#include <vector>

#include <gst/gstbin.h>

#include <gstreamer/rtpbin/rtpbin.hpp>

namespace gstreamer {
    namespace rtpbin {
        namespace sender {
            struct PipelineMapping : public rtpbin::PipelineMapping {
                std::string fec_sink_0;
                std::string fec_sink_1;

                std::vector<std::string> fec_stream_sinks() const;
            };

            struct Context {
                std::weak_ptr<GstBin> pipeline;
                PipelineMapping mapping;
            };

            void setup(std::string const& rtpbin_name, Context& ctx);

            GstElement* requestAuxSenderCallback(GstElement* rtpbin,
                guint session,
                void* user_data = nullptr);

            /**
             * Callback connected to the "pad-added" signal, it connects the 'sometimes'
             * pads for the output rtp and fec streams to their respective pipeline sinks
             * described by @param ctx_data
             */
            void onNewPadCallback(GstElement* rtpbin, GstPad* pad, void* ctx_data);
        }
    }
}

#endif