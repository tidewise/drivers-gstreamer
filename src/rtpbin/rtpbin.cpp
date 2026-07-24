#include <gstreamer/rtpbin/rtpbin.hpp>

#include <stdexcept>

using namespace gstreamer;
using namespace gstreamer::memory;

std::string rtpbin::rtp_sinkpad(std::string const& session_id)
{
    return "recv_rtp_sink_" + session_id;
}

std::string rtpbin::rtcp_sinkpad(std::string const& session_id)
{
    return "recv_rtcp_sink_" + session_id;
}

std::string rtpbin::rtcp_srcpad(std::string const& session_id)
{
    return "send_rtcp_src_" + session_id;
}

std::string rtpbin::fec_sink(std::string const& session_id,
    std::string const& fec_stream_index)
{
    return "recv_fec_sink_" + session_id + "_" + fec_stream_index;
}

void rtpbin::linkWithPipelineSrc(GstBin& pipeline,
    std::string const& pipeline_src_name,
    GstUnrefGuard<GstPad>& rtpbin_sink)
{
    if (!GST_IS_PAD(rtpbin_sink.get())) {
        throw std::runtime_error(
            "invalid rtpbin sinkpad for pipeline source " + pipeline_src_name);
    }

    GstUnrefGuard source{gst_bin_get_by_name(&pipeline, pipeline_src_name.c_str())};
    if (!GST_IS_ELEMENT(source.get())) {
        throw std::runtime_error(
            "could not find element named'" + pipeline_src_name + "'");
    }

    GstUnrefGuard srcpad{gst_element_get_static_pad(source.get(), "src")};
    if (!GST_IS_PAD(srcpad.get())) {
        throw std::runtime_error(
            "invalid source pad 'src' for element " + pipeline_src_name);
    }

    GstPadLinkReturn link = gst_pad_link(srcpad.get(), rtpbin_sink.get());
    if (link != 0) {
        throw std::runtime_error("could not link rtpbin sink pad with element '" +
                                 pipeline_src_name + "' source pad");
    }
}

void rtpbin::linkWithPipelineSink(GstBin& pipeline,
    std::string const& pipeline_src_name,
    GstUnrefGuard<GstPad>& rtpbin_source)
{
    if (!GST_IS_PAD(rtpbin_source.get())) {
        throw std::runtime_error(
            "invalid rtpbin srcpad for pipeline sink " + pipeline_src_name);
    }

    GstUnrefGuard sink{gst_bin_get_by_name(&pipeline, pipeline_src_name.c_str())};
    if (!GST_IS_ELEMENT(sink.get())) {
        throw std::runtime_error(
            "could not find element named'" + pipeline_src_name + "'");
    }

    GstUnrefGuard sinkpad{gst_element_get_static_pad(sink.get(), "sink")};
    if (!GST_IS_PAD(sinkpad.get())) {
        throw std::runtime_error(
            "invalid sink pad 'sink' for element " + pipeline_src_name);
    }

    GstPadLinkReturn link = gst_pad_link(rtpbin_source.get(), sinkpad.get());
    if (link != 0) {
        throw std::runtime_error("could not link rtpbin source pad with element '" +
                                 pipeline_src_name + "' sink pad");
    }
}