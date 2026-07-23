#include <gstreamer/memory.hpp>
#include <gtest/gtest.h>

using namespace gstreamer::memory;

struct MemoryTest : public ::testing::Test {
    MemoryTest()
    {
    }

    inline static bool unreferred{false};
    static void unref(GstElement* e)
    {
        unreferred = true;
    }
};

TEST_F(MemoryTest, it_implements_move_constructor)
{
    GstElement obj;
    GstUnrefGuard<GstElement> first_guard(&obj, unref);
    GstUnrefGuard<GstElement> second_guard(std::move(first_guard));
    ASSERT_EQ(nullptr, first_guard.get());
    ASSERT_EQ(&obj, second_guard.get());
}

TEST_F(MemoryTest, it_implements_move_assignment)
{
    GstElement obj;
    GstUnrefGuard<GstElement> second_guard(nullptr);
    unreferred = false;
    {
        GstUnrefGuard<GstElement> first_guard(&obj, unref);
        second_guard = std::move(first_guard);
        ASSERT_EQ(nullptr, first_guard.get());
    }
    ASSERT_EQ(&obj, second_guard.get());
    ASSERT_EQ(false, unreferred);
}