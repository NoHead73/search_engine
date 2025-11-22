#include <gtest/gtest.h>
#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"

TEST(InvertedIndexTest, BasicFunctionality) {
    const std::vector<std::string> docs = {
        "london is the capital of great britain",
        "big ben is the nickname for the Great bell of the striking clock"
    };
    
    InvertedIndex idx;
    idx.UpdateDocumentBase(docs);
    
    auto result1 = idx.GetWordCount("london");
    auto result2 = idx.GetWordCount("the");
    
    ASSERT_EQ(result1.size(), 1);
    EXPECT_EQ(result1[0].doc_id, 0);
    EXPECT_EQ(result1[0].count, 1);
    
    ASSERT_EQ(result2.size(), 2);
}

TEST(InvertedIndexTest, MultipleDocuments) {
    const std::vector<std::string> docs = {
        "milk milk milk milk water water water",
        "milk water water",
        "milk milk milk milk milk water water water water water",
        "americano cappuccino"
    };
    
    InvertedIndex idx;
    idx.UpdateDocumentBase(docs);
    
    auto milk_result = idx.GetWordCount("milk");
    auto water_result = idx.GetWordCount("water");
    auto cappuccino_result = idx.GetWordCount("cappuccino");
    
    ASSERT_EQ(milk_result.size(), 3);
    ASSERT_EQ(water_result.size(), 3);
    ASSERT_EQ(cappuccino_result.size(), 1);
}

TEST(SearchServerTest, BasicSearch) {
    const std::vector<std::string> docs = {
        "milk milk milk milk water water water",
        "milk water water",
        "milk milk milk milk milk water water water water water",
        "americano cappuccino"
    };
    
    InvertedIndex idx;
    idx.UpdateDocumentBase(docs);
    SearchServer server(idx);
    
    std::vector<std::string> requests = {"milk water", "sugar"};
    auto results = server.search(requests);
    
    ASSERT_EQ(results.size(), 2);
    ASSERT_FALSE(results[0].empty());
    ASSERT_TRUE(results[1].empty());
}

TEST(SearchServerTest, Ranking) {
    const std::vector<std::string> docs = {
        "moscow is the capital of russia",
        "london is the capital of great britain", 
        "paris is the capital of france"
    };
    
    InvertedIndex idx;
    idx.UpdateDocumentBase(docs);
    SearchServer server(idx);
    
    std::vector<std::string> requests = {"moscow capital"};
    auto results = server.search(requests);
    
    ASSERT_EQ(results.size(), 1);
    ASSERT_EQ(results[0].size(), 1);
    EXPECT_EQ(results[0][0].doc_id, 0);
    EXPECT_FLOAT_EQ(results[0][0].rank, 1.0f);
}

TEST(ConverterJSONTest, ConfigValidation) {
    ConverterJSON converter;
    
    EXPECT_NO_THROW({
        auto config_info = converter.GetConfigInfo();
        EXPECT_FALSE(config_info["name"].empty());
        EXPECT_FALSE(config_info["version"].empty());
    });
}

TEST(ConverterJSONTest, ResponsesLimit) {
    ConverterJSON converter;
    
    int limit = converter.GetResponsesLimit();
    EXPECT_GT(limit, 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}