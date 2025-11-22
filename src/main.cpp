#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif
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

int main(int argc, char **argv) {
    setupConsole();

    SimpleLogger logger("search_engine.log");
    logger.log("Application started");

    if (argc > 1 && std::string(argv[1]) == "--test") {
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