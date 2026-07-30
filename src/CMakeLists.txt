rock_library(gstreamer
    SOURCES
        memory.cpp
        rtpbin/receiver.cpp
        rtpbin/rtpbin.cpp
    HEADERS
        memory.hpp
        rtpbin/receiver.hpp
        rtpbin/rtpbin.hpp
    DEPS_PKGCONFIG
        gstreamer-1.0
        base-logging
)

