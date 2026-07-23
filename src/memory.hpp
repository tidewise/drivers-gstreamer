#ifndef GSTREAMER_MEMORY_HPP
#define GSTREAMER_MEMORY_HPP

#include "gst/gststructure.h"
#include <gst/gstelement.h>
#include <gst/video/gstvideometa.h>

namespace gstreamer {
    namespace memory {
        struct PipelineDestructor {
            void operator()(GstElement* pipeline) const;
        };

        template <typename T> struct GstUnref;
#define ROCK_GSTREAMER_UNREF(GstStruct, gst_unref)                                       \
    template <> struct GstUnref<GstStruct> {                                             \
        static void unref(GstStruct* obj)                                                \
        {                                                                                \
            gst_unref(obj);                                                              \
        }                                                                                \
    };

        ROCK_GSTREAMER_UNREF(GstElement, gst_object_unref);
        ROCK_GSTREAMER_UNREF(GstCaps, gst_caps_unref);
        ROCK_GSTREAMER_UNREF(GstSample, gst_sample_unref);
        ROCK_GSTREAMER_UNREF(GstBuffer, gst_buffer_unref);
        ROCK_GSTREAMER_UNREF(GstMemory, gst_memory_unref);
        ROCK_GSTREAMER_UNREF(GstVideoFrame, gst_video_frame_unmap);
        ROCK_GSTREAMER_UNREF(GstPad, gst_object_unref);
        ROCK_GSTREAMER_UNREF(GstStructure, gst_structure_free);
        ROCK_GSTREAMER_UNREF(gchar, g_free);

        template <typename T> struct GstUnrefGuard {
            T* object;
            typedef void (*Unref)(T*);
            Unref unref = nullptr;

            explicit GstUnrefGuard(T* object, Unref unref = GstUnref<T>::unref)
                : object(object)
                , unref(unref)
            {
            }

            GstUnrefGuard(GstUnrefGuard&& other)
            {
                this->object = other.release();
                this->unref = other.unref;
            }

            GstUnrefGuard(GstUnrefGuard const&) = delete;
            GstUnrefGuard& operator=(GstUnrefGuard const&) = delete;

            GstUnrefGuard& operator=(GstUnrefGuard&& other)
            {
                this->object = other.release();
                this->unref = other.unref;
                return *this;
            }

            ~GstUnrefGuard()
            {
                if (object) {
                    unref(object);
                }
            }
            T* get()
            {
                return object;
            }
            T* release()
            {
                T* ret = object;
                object = nullptr;
                return ret;
            }
        };

        struct GstMemoryUnmapGuard {
            GstMemory* memory;
            GstMapInfo& map_info;
            GstMemoryUnmapGuard(GstMemory* memory, GstMapInfo& map_info)
                : memory(memory)
                , map_info(map_info)
            {
            }
            ~GstMemoryUnmapGuard()
            {
                gst_memory_unmap(memory, &map_info);
            }
        };

    }
}

#endif