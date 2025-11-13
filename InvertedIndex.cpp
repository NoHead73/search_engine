#include "InvertedIndex.h"
#include <sstream>
#include <algorithm>
#include <thread>
#include <unordered_map>

using namespace std;

void InvertedIndex::UpdateDocumentBase(vector<string> input_docs) {
    docs = move(input_docs);
    freq_dictionary.clear();

    const size_t num_threads = std::min<size_t>(thread::hardware_concurrency(), docs.size());
    vector<thread> threads;
    vector<map<string, vector<Entry>>> thread_results(num_threads);

    auto process_docs = [&](size_t start, size_t end, size_t thread_id) {
        for (size_t doc_id = start; doc_id < end; ++doc_id) {
            stringstream ss(docs[doc_id]);
            string word;
            unordered_map<string, size_t> word_count;

            while (ss >> word) {
                transform(word.begin(), word.end(), word.begin(), ::tolower);
                if (word.length() > 100) continue;
                word_count[word]++;
            }

            for (const auto& [word, count] : word_count) {
                thread_results[thread_id][word].push_back({doc_id, count});
            }
        }
    };

    size_t docs_per_thread = (docs.size() + num_threads - 1) / num_threads;
    for (size_t i = 0; i < num_threads; ++i) {
        size_t start = i * docs_per_thread;
        size_t end = min(start + docs_per_thread, docs.size());
        if (start < docs.size()) {
            threads.emplace_back(process_docs, start, end, i);
        }
    }

    for (auto& thread : threads) {
        thread.join();
    }

    lock_guard<mutex> lock(dictionary_mutex);
    for (const auto& thread_result : thread_results) {
        for (const auto& [word, entries] : thread_result) {
            freq_dictionary[word].insert(
                freq_dictionary[word].end(),
                entries.begin(),
                entries.end()
            );
        }
    }
}

vector<Entry> InvertedIndex::GetWordCount(const string& word) {
    string word_lower = word;
    transform(word_lower.begin(), word_lower.end(), word_lower.begin(), ::tolower);

    lock_guard<mutex> lock(dictionary_mutex);
    auto it = freq_dictionary.find(word_lower);
    if (it != freq_dictionary.end()) {
        return it->second;
    }
    
    return {};
}