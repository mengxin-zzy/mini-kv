#include<gtest/gtest.h>
#include "db.h"

TEST(KV, PutThenGet){
	KV db;
	EXPECT_TRUE(db.Put("name", "alice"));
	EXPECT_EQ(db.Get("name"), std::optional<std::string>("alice"));
}

TEST(KV, Overwrite){
	KV db;
	db.Put("k", "v1");
	db.Put("k", "v2");
	EXPECT_EQ(db.Get("k"), std::optional<std::string>("v2"));
}

TEST(KV, DeleteThenGet){
	KV db;
	db.Put("k", "v");
	EXPECT_TRUE(db.Delete("k"));
	EXPECT_EQ(db.Get("k"), std::nullopt);
}

TEST(KV, EmptyKeyRejected){
	KV db;
	EXPECT_FALSE(db.Put("", "v"));
}

TEST(KV, ScanRange) {
    KV db;
    for (auto k : {"a", "b", "c", "d", "e"})
        db.Put(k, std::string(k) + "!");
    auto rows = db.Scan("b", "d");
    ASSERT_EQ(rows.size(), 2u);
    EXPECT_EQ(rows[0].first, "b");
    EXPECT_EQ(rows[1].first, "c");
}