#include "ConverterJSON.h"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sstream>

using namespace std;

bool ConverterJSON::fileExists(const std::string& path) const {
    ifstream file(path);
    return file.good();
}

void ConverterJSON::validateConfig(const json& config) const {
    if (!config.contains("config")) {
        throw runtime_error("config file is empty");
    }

    const auto& config_section = config["config"];

    if (!config_section.contains("name") ||
        !config_section.contains("version") ||
        config_section["name"].empty() ||
        config_section["version"].empty()) {
        throw runtime_error("config file is empty");
    }

    string version = config_section["version"];
    if (version != "0.1") {
        throw runtime_error("config.json has incorrect file version");
    }

    if (config_section.contains("max_responses")) {
        int max_resp = config_section["max_responses"];
        if (max_resp <= 0) {
            throw runtime_error("max_responses must be positive");
        }
    }

    if (config.contains("files")) {
        if (!config["files"].is_array()) {
            throw runtime_error("files must be an array");
        }
    }
}

bool ConverterJSON::validateDocumentContent(const std::string& content, const std::string& path) {
    if (content.length() > 100000) {
        return false;
    }

    stringstream ss(content);
    string word;
    size_t word_count = 0;
    while (ss >> word && word_count <= 1000) {
        if (word.length() > 100) {
            return false;
        }
        word_count++;
    }

    if (word_count > 1000) {
        return false;
    }

    return true;
}

map<string, string> ConverterJSON::GetConfigInfo() {
    if (!fileExists(config_path)) {
        throw runtime_error("config file is missing");
    }

    ifstream config_file(config_path);
    json config_data = json::parse(config_file);
    validateConfig(config_data);

    map<string, string> info;
    const auto& config_section = config_data["config"];
    info["name"] = config_section["name"];
    info["version"] = config_section["version"];

    return info;
}

vector<string> ConverterJSON::GetTextDocuments() {
    if (!fileExists(config_path)) {
        throw runtime_error("config file is missing");
    }

    ifstream config_file(config_path);
    json config_data = json::parse(config_file);
    validateConfig(config_data);

    vector<string> documents;

    if (config_data.contains("files")) {
        for (const auto& file_path : config_data["files"]) {
            string path = file_path;

            if (!fileExists(path)) {
                std::cerr << "File not found: " << path << std::endl;
                continue;
            }

            ifstream document_file(path);
            if (document_file.is_open()) {
                string content((istreambuf_iterator<char>(document_file)),
                              istreambuf_iterator<char>());

                if (validateDocumentContent(content, path)) {
                    documents.push_back(content);
                }
            }
        }
    }

    return documents;
}

int ConverterJSON::GetResponsesLimit() {
    if (!fileExists(config_path)) {
        throw runtime_error("config file is missing");
    }

    ifstream config_file(config_path);
    json config_data = json::parse(config_file);
    validateConfig(config_data);

    const auto& config_section = config_data["config"];
    if (config_section.contains("max_responses")) {
        return config_section["max_responses"];
    }

    return 5;
}

vector<string> ConverterJSON::GetRequests() {
    if (!fileExists(requests_path)) {
        throw runtime_error("requests.json file is missing");
    }

    ifstream requests_file(requests_path);
    json requests_data = json::parse(requests_file);

    vector<string> requests;

    if (requests_data.contains("requests")) {
        if (!requests_data["requests"].is_array()) {
            throw runtime_error("requests must be an array");
        }

        for (const auto& request : requests_data["requests"]) {
            string request_str = request;
            if (request_str.length() > 1000) {
                request_str = request_str.substr(0, 1000);
            }
            requests.push_back(request_str);
        }
    }

    return requests;
}

void ConverterJSON::putAnswers(vector<vector<pair<int, float>>> answers) {
    json answers_json;
    answers_json["answers"] = json::object();

    int max_responses = GetResponsesLimit();

    for (size_t i = 0; i < answers.size(); ++i) {
        string request_id = "request" + string(3 - to_string(i + 1).length(), '0') + to_string(i + 1);

        vector<pair<int, float>> limited_answers;
        if (answers[i].size() > max_responses) {
            limited_answers.assign(answers[i].begin(), answers[i].begin() + max_responses);
        } else {
            limited_answers = answers[i];
        }

        if (limited_answers.empty()) {
            answers_json["answers"][request_id] = {{"result", "false"}};
        } else if (limited_answers.size() == 1) {
            answers_json["answers"][request_id] = {
                {"result", "true"},
                {"docid", limited_answers[0].first},
                {"rank", limited_answers[0].second}
            };
        } else {
            json relevance_array = json::array();
            for (const auto& [docid, rank] : limited_answers) {
                relevance_array.push_back({{"docid", docid}, {"rank", rank}});
            }

            answers_json["answers"][request_id] = {
                {"result", "true"},
                {"relevance", relevance_array}
            };
        }
    }

    ofstream answers_file(answers_path);
    answers_file << answers_json.dump(4) << endl;
    answers_file.close();

    cout << "Answers saved to " << answers_path << endl;
}