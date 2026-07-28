#ifndef GSTREAMER_RTPBIN_RECEIVER_HPP
#define GSTREAMER_RTPBIN_RECEIVER_HPP

#include <memory>
#include <string>
#include <vector>

#include <gst/gstbin.h>

#include <gstreamer/rtpbin/rtpbin.hpp>

namespace gstreamer {
    namespace rtpbin {
        namespace receiver {
            /** Maps receiver interface to pipeline element names */
            struct PipelineMapping : public rtpbin::PipelineMapping {
                std::string fec_source_0;
                std::string fec_source_1;

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