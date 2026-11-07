#include "skiplist.h"
#include<gtest/gtest.h>

TEST(SkipList, EmptyFind) {
    SkipList s;
    EXPECT_EQ(s.Find("nope"), std::nullopt);
}

TEST(SkipList, InsertThenFind){
    SkipList s;
    s.Insert("b", "2");
    s.Insert("a", "1");
    s.Insert("c", "3");
    EXPECT_EQ(s.Find("a"), std::optional<std::string>("1"));
    EXPECT_EQ(s.Find("b"), std::optional<std::string>("2"));
    EXPECT_EQ(s.Find("c"), std::optional<std::string>("3"));
    EXPECT_EQ(s.Find("z"), std::nullopt);
}


TEST(SkipList, Overwrite) {
    SkipList s;
    s.Insert("k", "v1");
    s.Insert("k", "v2");
    EXPECT_EQ(s.Find("k"), std::optional<std::string>("v2"));
    EXPECT_EQ(s.Size(), 1u);
}


TEST(SkipList, Erase) {
    SkipList s;
    s.Insert("a", "1");
    s.Insert("b", "2");
    EXPECT_TRUE(s.Erase("a"));
    EXPECT_EQ(s.Find("a"), std::nullopt);
    EXPECT_FALSE(s.Erase("a"));            // 再删一次：false
    EXPECT_EQ(s.Find("b"), std::optional<std::string>("2"));
}

TEST(SkipList, ManyKeys) {
    SkipList s;
    for (int i = 0; i < 1000; ++i)
        s.Insert("k" + std::to_string(i), std::to_string(i));
    EXPECT_EQ(s.Size(), 1000u);
    for (int i = 0; i < 1000; ++i)
        EXPECT_EQ(s.Find("k" + std::to_string(i)), std::optional<std::string>(std::to_string(i)));
    EXPECT_EQ(s.Find("k1000"), std::nullopt);
}