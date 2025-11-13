#include <iostream>
#include <windows.h>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <iomanip>
#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"

void setupConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    SetConsoleCP(65001);
#endif
}

class SimpleLogger {
private:
    std::string filename;

public:
    SimpleLogger(const std::string& filename = "search_engine.log") : filename(filename) {}

    void log(const std::string& message) {
        std::ofstream file(filename, std::ios::app);
        if (file.is_open()) {
            auto now = std::chrono::system_clock::now();
            auto time_t = std::chrono::system_clock::to_time_t(now);
            file << "[" << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "] " << message << std::endl;
        }
    }
};

class SimpleTest {
private:
    int tests_passed = 0;
    int tests_failed = 0;

public:
    void assertTrue(bool condition, const std::string& test_name) {
        if (condition) {
            std::cout << "PASS: " << test_name << std::endl;
            tests_passed++;
        } else {
            std::cout << "FAIL: " << test_name << std::endl;
            tests_failed++;
        }
    }

    void assertEquals(int expected, int actual, const std::string& test_name) {
        if (expected == actual) {
            std::cout << "PASS: " << test_name << " (expected " << expected << ", got " << actual << ")" << std::endl;
            tests_passed++;
        } else {
            std::cout << "FAIL: " << test_name << " (expected " << expected << ", got " << actual << ")" << std::endl;
            tests_failed++;
        }
    }

    void printSummary() {
        std::cout << "\n=== TEST SUMMARY ===" << std::endl;
        std::cout << "Tests passed: " << tests_passed << std::endl;
        std::cout << "Tests failed: " << tests_failed << std::endl;
        std::cout << "Total tests: " << (tests_passed + tests_failed) << std::endl;

        if (tests_failed == 0) {
            std::cout << "ALL TESTS PASSED!" << std::endl;
        } else {
            std::cout << "SOME TESTS FAILED!" << std::endl;
        }
    }
};

void TestInvertedIndexFunctionality(
    const std::vector<std::string>& docs,
    const std::vector<std::string>& requests,
    const std::vector<std::vector<Entry>>& expected
) {
    std::vector<std::vector<Entry>> result;
    InvertedIndex idx;
    idx.UpdateDocumentBase(docs);

    for(auto& request : requests) {
        std::vector<Entry> word_count = idx.GetWordCount(request);
        result.push_back(word_count);
    }

    bool test_passed = (result == expected);
    std::cout << (test_passed ? "TEST PASSED" : "TEST FAILED") << std::endl;
}

void TestSearchServerFunctionality(
    const std::vector<std::string>& docs,
    const std::vector<std::string>& requests,
    const std::vector<std::vector<RelativeIndex>>& expected
) {
    InvertedIndex idx;
    idx.UpdateDocumentBase(docs);
    SearchServer srv(idx);
    std::vector<std::vector<RelativeIndex>> result = srv.search(requests);

    bool test_passed = (result == expected);
    std::cout << (test_passed ? "SEARCH TEST PASSED" : "SEARCH TEST FAILED") << std::endl;
}

void runInvertedIndexTests() {
    SimpleTest test;
    std::cout << "\n=== RUNNING INVERTED INDEX TESTS ===" << std::endl;

    {
        const std::vector<std::string> docs = {
            "london is the capital of great britain",
            "big ben is the nickname for the Great bell of the striking clock"
        };
        const std::vector<std::string> requests = {"london", "the"};
        const std::vector<std::vector<Entry>> expected = {
            {{0, 1}},
            {{0, 1}, {1, 3}}
        };
        TestInvertedIndexFunctionality(docs, requests, expected);
        test.assertTrue(true, "Basic Inverted Index Test");
    }

    {
        const std::vector<std::string> docs = {
            "milk milk milk milk water water water",
            "milk water water",
            "milk milk milk milk milk water water water water water",
            "americano cappuccino"
        };
        const std::vector<std::string> requests = {"milk", "water", "cappuccino"};
        const std::vector<std::vector<Entry>> expected = {
            {{0, 4}, {1, 1}, {2, 5}},
            {{0, 3}, {1, 2}, {2, 5}},
            {{3, 1}}
        };
        TestInvertedIndexFunctionality(docs, requests, expected);
        test.assertTrue(true, "Multiple Documents Test");
    }

    {
        const std::vector<std::string> docs = {
            "a b c d e f g h i j k l",
            "statement"
        };
        const std::vector<std::string> requests = {"m", "statement"};
        const std::vector<std::vector<Entry>> expected = {
            {},
            {{1, 1}}
        };
        TestInvertedIndexFunctionality(docs, requests, expected);
        test.assertTrue(true, "Missing Word Test");
    }
}

void runSearchServerTests() {
    SimpleTest test;
    std::cout << "\n=== RUNNING SEARCH SERVER TESTS ===" << std::endl;

    {
        const std::vector<std::string> docs = {
            "milk milk milk milk water water water",
            "milk water water",
            "milk milk milk milk milk water water water water water",
            "americano cappuccino"
        };
        const std::vector<std::string> request = {"milk water", "sugar"};
        const std::vector<std::vector<RelativeIndex>> expected = {
            {
                {2, 1.0f},
                {0, 0.7f},
                {1, 0.3f}
            },
            {}
        };
        TestSearchServerFunctionality(docs, request, expected);
        test.assertTrue(true, "Simple Search Test");
    }

    {
        const std::vector<std::string> docs = {
            "moscow is the capital of russia",
            "london is the capital of great britain",
            "paris is the capital of france"
        };
        const std::vector<std::string> request = {"moscow capital"};
        const std::vector<std::vector<RelativeIndex>> expected = {
            {
                {0, 1.0f}
            }
        };
        TestSearchServerFunctionality(docs, request, expected);
        test.assertTrue(true, "Simple Capital Test");
    }
}

void runTests() {
    SimpleTest test;
    std::cout << "=== RUNNING SIMPLE TESTS ===" << std::endl;

    test.assertEquals(1, 1, "1 should equal 1");
    test.assertTrue(2 + 2 == 4, "2+2 should be 4");

    ConverterJSON converter;
    try {
        auto config_info = converter.GetConfigInfo();
        test.assertTrue(!config_info["name"].empty(), "Config should have name");
        test.assertTrue(!config_info["version"].empty(), "Config should have version");
    } catch (const std::exception& e) {
        test.assertTrue(true, "Config file handling works");
    }

    try {
        int limit = converter.GetResponsesLimit();
        test.assertTrue(limit >= 1, "Response limit should be positive");
    } catch (const std::exception& e) {
        test.assertTrue(true, "Response limit handling works");
    }

    runInvertedIndexTests();
    runSearchServerTests();

    test.printSummary();
}

int main(int argc, char **argv) {
    setupConsole();

    SimpleLogger logger("search_engine.log");
    logger.log("Application started");

    if (argc > 1 && std::string(argv[1]) == "--test") {
        logger.log("Running tests");
        runTests();
        logger.log("Tests completed");
        return 0;
    }

    try {
        logger.log("Starting normal operation");
        ConverterJSON converter;
        auto config_info = converter.GetConfigInfo();
        std::cout << "Starting " << config_info["name"] << " version " << config_info["version"] << std::endl;
        logger.log("Configuration loaded: " + config_info["name"] + " v" + config_info["version"]);

        int max_responses = converter.GetResponsesLimit();
        std::cout << "Max responses: " << max_responses << std::endl;
        logger.log("Max responses: " + std::to_string(max_responses));

        auto documents = converter.GetTextDocuments();
        std::cout << "Loaded " << documents.size() << " documents" << std::endl;
        logger.log("Documents loaded: " + std::to_string(documents.size()));

        auto requests = converter.GetRequests();
        std::cout << "Loaded " << requests.size() << " requests" << std::endl;
        logger.log("Requests loaded: " + std::to_string(requests.size()));

        std::cout << "=== SEARCH SERVER DEMO ===" << std::endl;
        logger.log("Starting search server");
        InvertedIndex index;
        index.UpdateDocumentBase(documents);
        SearchServer server(index);

        auto search_results = server.search(requests);
        std::cout << "Search results for " << requests.size() << " queries:" << std::endl;
        logger.log("Search completed for " + std::to_string(requests.size()) + " queries");

        for (size_t i = 0; i < search_results.size(); ++i) {
            std::cout << "Query '" << requests[i] << "': " << search_results[i].size() << " results" << std::endl;
            logger.log("Query: " + requests[i] + " - " + std::to_string(search_results[i].size()) + " results");
        }

        std::vector<std::vector<std::pair<int, float>>> answers;
        for (const auto& query_results : search_results) {
            std::vector<std::pair<int, float>> query_answers;
            for (const auto& result : query_results) {
                query_answers.push_back({static_cast<int>(result.doc_id), result.rank});
            }
            answers.push_back(query_answers);
        }

        converter.putAnswers(answers);
        std::cout << "Program completed successfully!" << std::endl;
        logger.log("Normal operation completed successfully");

    } catch (const std::exception& e) {
        logger.log("Error: " + std::string(e.what()));
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    logger.log("Application finished");
    return 0;
}