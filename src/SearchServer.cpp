#include "SearchServer.h"
#include <algorithm>
#include <sstream>
#include <map>
#include <set>

using namespace std;

vector<RelativeIndex> SearchServer::processSingleQuery(const string& query) {
    string query_lower = query;
    transform(query_lower.begin(), query_lower.end(), query_lower.begin(), ::tolower);

    {
        lock_guard<mutex> lock(cache_mutex);
        auto it = cache.find(query_lower);
        if (it != cache.end()) {
            return it->second;
        }
    }

    if (query.empty()) {
        return {};
    }

    stringstream ss(query);
    string word;
    vector<string> words;
    map<string, size_t> word_frequency;

    while (ss >> word) {
        transform(word.begin(), word.end(), word.begin(), ::tolower);
        if (find(words.begin(), words.end(), word) == words.end()) {
            words.push_back(word);
            auto entries = _index.GetWordCount(word);
            word_frequency[word] = entries.size();
        }
    }

    if (words.empty()) {
        return {};
    }

    sort(words.begin(), words.end(), [&](const string& a, const string& b) {
        return word_frequency[a] < word_frequency[b];
    });

    set<size_t> relevant_docs;
    for (const auto& word : words) {
        auto entries = _index.GetWordCount(word);
        set<size_t> current_docs;
        for (const auto& entry : entries) {
            current_docs.insert(entry.doc_id);
        }

        if (relevant_docs.empty()) {
            relevant_docs = current_docs;
        } else {
            set<size_t> intersection;
            set_intersection(
                relevant_docs.begin(), relevant_docs.end(),
                current_docs.begin(), current_docs.end(),
                inserter(intersection, intersection.begin())
            );
            relevant_docs = intersection;
        }

        if (relevant_docs.empty()) break;
    }

    if (relevant_docs.empty()) {
        return {};
    }

    map<size_t, float> doc_relevance;
    float max_relevance = 0.0f;

    for (size_t doc_id : relevant_docs) {
        float relevance = 0.0f;
        for (const auto& word : words) {
            auto entries = _index.GetWordCount(word);
            for (const auto& entry : entries) {
                if (entry.doc_id == doc_id) {
                    relevance += static_cast<float>(entry.count);
                    break;
                }
            }
        }
        doc_relevance[doc_id] = relevance;
        if (relevance > max_relevance) {
            max_relevance = relevance;
        }
    }

    vector<RelativeIndex> query_result;
    for (const auto& [doc_id, relevance] : doc_relevance) {
        float relative_rank = (max_relevance > 0) ? relevance / max_relevance : 0.0f;
        query_result.push_back({doc_id, relative_rank});
    }

    sort(query_result.begin(), query_result.end(),
        [](const RelativeIndex& a, const RelativeIndex& b) {
            if (abs(a.rank - b.rank) < 0.0001f) {
                return a.doc_id < b.doc_id;
            }
            return a.rank > b.rank;
        });

    lock_guard<mutex> lock(cache_mutex);
    if (cache.size() >= max_cache_size) {
        cleanupCache();
    }
    cache[query_lower] = query_result;

    return query_result;
}

void SearchServer::cleanupCache() {
    if (cache.size() > max_cache_size / 2) {
        auto it = cache.begin();
        advance(it, cache.size() / 2);
        cache.erase(cache.begin(), it);
    }
}

vector<vector<RelativeIndex>> SearchServer::search(const vector<string>& queries_input) {
    vector<vector<RelativeIndex>> result;

    for (const auto& query : queries_input) {
        result.push_back(processSingleQuery(query));
    }
    
    return result;
}