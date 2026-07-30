#include "base-logging/logging/logging_iostream_style.h"
#include "rtpbin.hpp"
#include <regex>

#include <base-logging/Logging.hpp>

#include <gstreamer/memory.hpp>
#include <gstreamer/rtpbin/sender.hpp>

using namespace gstreamer::memory;
using namespace gstreamer::rtpbin;

void sender::setup(std::string const& rtpbin_name, Context& ctx)
{
    auto pipeline = ctx.pipeline.lock();
    GstUnrefGuard rtpbin{gst_bin_get_by_name(pipeline.get(), rtpbin_name.c_str())};

    g_signal_connect(rtpbin.get(),
        "request-aux-sender",
        G_CALLBACK(requestAuxSenderCallback),
        nullptr);
    g_signal_connect(rtpbin.get(),
        "pad-added",
        G_CALLBACK(onNewPadCallback),
        static_cast<gpointer>(&ctx));

    auto id = std::to_string(ctx.mapping.session_id);
    {
        // pipeline rtp src -> rtpbin rtp sink
        GstUnrefGuard sinkpad{gst_element_request_pad_simple(rtpbin.get(),
            rtpbin::send_rtp_sinkpad(id).c_str())};
        rtpbin::linkWithPipelineSrc(*pipeline, ctx.mapping.rtp_source, sinkpad);
    }

    rtpbin::setupRTCP(pipeline, rtpbin, ctx.mapping);
}

GstElement* sender::requestAuxSenderCallback(GstElement* rtpbin,
    guint session,
    void* user_data)
{
    GstUnrefGuard payload_type_map{
        gst_structure_from_string("application/x-rtp-pt-map,96=(uint)97", nullptr)};

    GstUnrefGuard sender{gst_element_factory_make_full("rtprtxsend",
        "payload-type-map",
        payload_type_map.get(),
        NULL)};

    auto id = std::to_string(session);
    std::string aux_send_name = "aux_send_" + id;
    GstUnrefGuard aux_send{gst_bin_new(aux_send_name.c_str())};
    GstUnrefGuard rtp_sink{gst_element_get_static_pad(sender.get(), "sink")};
    GstUnrefGuard rtp_src{gst_element_get_static_pad(sender.get(), "src")};

    gst_bin_add(GST_BIN(aux_send.get()), sender.release());
    gst_element_add_pad(aux_send.get(),
        gst_ghost_pad_new(("sink_" + id).c_str(), rtp_sink.get()));
    gst_element_add_pad(aux_send.get(),
        gst_ghost_pad_new(("src_" + id).c_str(), rtp_src.get()));

    return aux_send.release();
}

void sender::onNewPadCallback(GstElement* rtpbin, GstPad* pad, void* ctx_data)
{
    std::string pad_name;
    {
        GstUnrefGuard pad_name_tmp{gst_pad_get_name(pad)};
        pad_name = pad_name_tmp.get();
    }

    std::regex send_rtp_src("send_rtp_src_\\d+");
    std::regex send_fec_src("send_fec_src_\\d+_(\\d)");
    if (!std::regex_search(pad_name, send_rtp_src) &&
        !std::regex_search(pad_name, send_fec_src)) {
        return;
    }

    LOG_INFO_S << "new pad added " << pad_name << std::endl;

    auto ctx = static_cast<Context*>(ctx_data);
    GstUnrefGuard<GstPad> srcpad{static_cast<GstPad*>(gst_object_ref(pad))};
    auto pipeline = ctx->pipeline.lock();

    if (std::regex_search(pad_name, send_rtp_src)) {
        rtpbin::linkWithPipelineSink(*pipeline, ctx->mapping.rtp_sink, srcpad);
        return;
    }

    std::smatch match;
    if (!std::regex_match(pad_name, match, send_fec_src)) {
        return;
    }

    uint16_t fec_id = std::stoi(match[1]);
    auto fec_sinks = ctx->mapping.fec_interfaces();
    if (fec_id >= fec_sinks.size()) {
        LOG_INFO_S << "there is no fec sink for fec stream " << fec_id << std::endl;
        return;
    }

    LOG_INFO_S << "linking " << pad_name << " with " << fec_sinks[fec_id] << std::endl;
    rtpbin::linkWithPipelineSink(*pipeline, fec_sinks[fec_id], srcpad);
}