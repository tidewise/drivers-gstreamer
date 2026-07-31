#include <regex>

#include <gst/gstbin.h>

#include <base-logging/Logging.hpp>

#include <gstreamer/memory.hpp>
#include <gstreamer/rtpbin/receiver.hpp>
#include <gstreamer/rtpbin/rtpbin.hpp>
#include <string>

using namespace gstreamer::memory;
using namespace gstreamer::rtpbin;
using namespace gstreamer::rtpbin::receiver;

void receiver::setup(std::string const& rtpbin_name, Context& ctx)
{

    auto [pipeline, rtpbin] = acquirePipelineAndRPTBin(rtpbin_name, ctx);

    g_signal_connect(rtpbin.get(),
        "request-aux-receiver",
        G_CALLBACK(requestAuxReceiverCallback),
        nullptr);
    g_signal_connect(rtpbin.get(),
        "pad-added",
        G_CALLBACK(onNewPadCallback),
        static_cast<gpointer>(&ctx));

    std::string id = std::to_string(ctx.mapping.session_id);
    {
        // pipeline rtp src -> rtpbin rtp sink
        GstUnrefGuard sinkpad{gst_element_request_pad_simple(rtpbin.get(),
            rtpbin::recv_rtp_sinkpad(id).c_str())};
        rtpbin::linkWithPipelineSrc(*pipeline, ctx.mapping.rtp_source, sinkpad);
    }

    rtpbin::setupRTCP(pipeline, rtpbin, ctx.mapping);

    auto fec_streams = ctx.mapping.fec_interfaces();
    for (std::size_t i = 0; i < fec_streams.size(); i++) {
        GstUnrefGuard fec_sinkpad{gst_element_request_pad_simple(rtpbin.get(),
            rtpbin::fec_sinkpad(id, std::to_string(i)).c_str())};
        rtpbin::linkWithPipelineSrc(*pipeline, fec_streams[i], fec_sinkpad);
    }
}

GstElement* receiver::requestAuxReceiverCallback(GstElement* rtpbin,
    guint session,
    void* user_data)
{
    GstUnrefGuard payload_type_map{
        gst_structure_from_string("application/x-rtp-pt-map,96=(uint)97", nullptr)};

    GstUnrefGuard receiver{gst_element_factory_make_full("rtprtxreceive",
        "payload-type-map",
        payload_type_map.get(),
        NULL)};

    auto id = std::to_string(session);
    std::string aux_recv_name = "aux_recv_" + id;
    GstUnrefGuard aux_recv{gst_bin_new(aux_recv_name.c_str())};
    GstUnrefGuard rtp_sink{gst_element_get_static_pad(receiver.get(), "sink")};
    GstUnrefGuard rtp_src{gst_element_get_static_pad(receiver.get(), "src")};

    gst_bin_add(GST_BIN(aux_recv.get()), receiver.release());
    gst_element_add_pad(aux_recv.get(),
        gst_ghost_pad_new(("sink_" + id).c_str(), rtp_sink.get()));
    gst_element_add_pad(aux_recv.get(),
        gst_ghost_pad_new(("src_" + id).c_str(), rtp_src.get()));

    return aux_recv.release();
}

void receiver::onNewPadCallback(GstElement* rtpbin, GstPad* pad, void* ctx_data)
{
    std::string pad_name;
    {
        GstUnrefGuard pad_name_tmp{gst_pad_get_name(pad)};
        pad_name = pad_name_tmp.get();
    }

    LOG_INFO_S << "new pad added " << pad_name << std::endl;

    std::regex recv_rtp_src("recv_rtp_src_\\d+_\\d+_\\d+");
    if (!std::regex_search(pad_name, recv_rtp_src)) {
        return;
    }

    auto ctx = static_cast<Context*>(ctx_data);
    GstUnrefGuard<GstPad> srcpad{static_cast<GstPad*>(gst_object_ref(pad))};
    auto pipeline = ctx->pipeline.lock();
    rtpbin::linkWithPipelineSink(*pipeline, ctx->mapping.rtp_sink, srcpad);
}