#pragma once
#include <string>
#include <vector>
#include <map>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

class ConverterJSON {
private:
    std::string config_path = "../config.json";
    std::string requests_path = "../requests.json";
    std::string answers_path = "../answers.json";

    bool fileExists(const std::string& path) const;
    void validateConfig(const json& config) const;
    bool validateDocumentContent(const std::string& content, const std::string& path);

public:
    ConverterJSON() = default;

    std::vector<std::string> GetTextDocuments();
    int GetResponsesLimit();
    std::vector<std::string> GetRequests();
    void putAnswers(std::vector<std::vector<std::pair<int, float>>> answers);
    std::map<std::string, std::string> GetConfigInfo();
};