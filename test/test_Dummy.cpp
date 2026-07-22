#include <boost/test/unit_test.hpp>
#include <gstreamer/Dummy.hpp>

using namespace gstreamer;

BOOST_AUTO_TEST_CASE(it_should_not_crash_when_welcome_is_called)
{
    gstreamer::DummyClass dummy;
    dummy.welcome();
}
