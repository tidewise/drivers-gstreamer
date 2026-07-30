#ifndef GSTREAMER_RTPBIN_RECEIVER_HPP
#define GSTREAMER_RTPBIN_RECEIVER_HPP

#include <memory>
#include <string>
#include <vector>

#include <gst/gstbin.h>

namespace gstreamer {
    namespace rtpbin {
        namespace receiver {
            /** Maps receiver interface to pipeline element names */
            struct PipelineMapping {
                int session_id{-1};
                std::string rtp_source;
                std::string rtp_sink;
                std::string rtcp_source;
                std::string rtcp_feedback_sink;
                std::string fec_source_0;
                std::string fec_source_1;

                bool undefined() const;
                std::vector<std::string> fec_stream_sources() const;
            };

            struct Context {
                std::weak_ptr<GstBin> pipeline;
                PipelineMapping mapping;
            };

            /** warn that ctx is referenced by callbacks and should be valid for their
             * lifecyle
             */
            void setup(std::string const& rtpbin_name, Context& ctx);

            GstElement* requestAuxReceiverCallback(GstElement* rtpbin,
                guint session,
                void* user_data = nullptr);

            void onNewPadCallback(GstElement* rtpbin,
                GstPad* pad,
                void* pipeline_mapping);
        }
    }
}

#endif