#pragma once
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <unordered_map>
#include <mutex>
#include "InvertedIndex.h"

struct RelativeIndex {
    size_t doc_id;
    float rank;

    bool operator ==(const RelativeIndex& other) const {
        float epsilon = 0.0001f;
        return (doc_id == other.doc_id &&
                std::abs(rank - other.rank) < epsilon);
    }
};

class SearchServer {
public:
    SearchServer(InvertedIndex& idx) : _index(idx) { };

    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries_input);

private:
    InvertedIndex& _index;
    std::unordered_map<std::string, std::vector<RelativeIndex>> cache;
    std::mutex cache_mutex;
    size_t max_cache_size = 1000;

    std::vector<RelativeIndex> processSingleQuery(const std::string& query);
    void cleanupCache();
};