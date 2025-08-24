#include <iostream>
#include <string>
#include <memory>
#include <sstream>
#include <curl/curl.h>
#include <json/json.h>
#include <cstring>

class OpenAIClient {
private:
    std::string api_key_;
    std::string base_url_;
    std::unique_ptr<CURL, decltype(&curl_easy_cleanup)> curl_;

    // Callback function for CURL to write response data
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
        userp->append((char*)contents, size * nmemb);
        return size * nmemb;
    }

    // Callback function for CURL to read request data
    static size_t ReadCallback(void* dest, size_t size, size_t nmemb, std::string* userp) {
        if (userp->empty()) return 0;
        
        size_t len = std::min(size * nmemb, userp->length());
        memcpy(dest, userp->c_str(), len);
        userp->erase(0, len);
        return len;
    }

public:
    OpenAIClient(const std::string& api_key, const std::string& base_url = "https://api.deepseek.com")
        : api_key_(api_key), base_url_(base_url), curl_(curl_easy_init(), curl_easy_cleanup) {
        
        if (!curl_) {
            throw std::runtime_error("Failed to initialize CURL");
        }
    }

    // Chat completion request structure
    struct ChatMessage {
        std::string role;
        std::string content;
    };

    struct ChatCompletionRequest {
        std::string model = "deepseek-chat";
        std::vector<ChatMessage> messages;
        bool stream = false;
        double temperature = 0.7;
        int max_tokens = 1000;
    };

    struct Message {
        std::string role;
        std::string content;
    };

    struct Choice {
        int index;
        Message message;
        std::string finish_reason;
    };

    struct Usage {
        int prompt_tokens;
        int completion_tokens;
        int total_tokens;
    };

    struct ChatCompletionResponse {
        std::string id;
        std::string object;
        int64_t created;
        std::string model;
        std::vector<Choice> choices;
        Usage usage;
    };

    // Send chat completion request
    ChatCompletionResponse chatCompletion(const ChatCompletionRequest& request) {
        // Prepare JSON payload
        Json::Value payload;
        payload["model"] = request.model;
        payload["stream"] = request.stream;
        payload["temperature"] = request.temperature;
        payload["max_tokens"] = request.max_tokens;
        
        // Add messages
        Json::Value messages(Json::arrayValue);
        for (const auto& msg : request.messages) {
            Json::Value message;
            message["role"] = msg.role;
            message["content"] = msg.content;
            messages.append(message);
        }
        payload["messages"] = messages;

        Json::StreamWriterBuilder writer;
        std::string json_str = Json::writeString(writer, payload);
        
        // Set up CURL options
        curl_easy_reset(curl_.get());
        
        // Set URL
        std::string url = base_url_ + "/chat/completions";
        curl_easy_setopt(curl_.get(), CURLOPT_URL, url.c_str());
        
        // Set headers
        struct curl_slist* headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        headers = curl_slist_append(headers, ("Authorization: Bearer " + api_key_).c_str());
        curl_easy_setopt(curl_.get(), CURLOPT_HTTPHEADER, headers);
        
        // Set POST method
        curl_easy_setopt(curl_.get(), CURLOPT_POST, 1L);
        
        // Set request data
        curl_easy_setopt(curl_.get(), CURLOPT_READDATA, &json_str);
        curl_easy_setopt(curl_.get(), CURLOPT_READFUNCTION, ReadCallback);
        curl_easy_setopt(curl_.get(), CURLOPT_POSTFIELDSIZE, json_str.length());
        
        // Set response callback
        std::string response;
        curl_easy_setopt(curl_.get(), CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl_.get(), CURLOPT_WRITEDATA, &response);
        
        // Set SSL options
        curl_easy_setopt(curl_.get(), CURLOPT_SSL_VERIFYPEER, 1L);
        curl_easy_setopt(curl_.get(), CURLOPT_SSL_VERIFYHOST, 2L);
        
        // Set timeout
        curl_easy_setopt(curl_.get(), CURLOPT_TIMEOUT, 30L);
        
        // Perform request
        CURLcode res = curl_easy_perform(curl_.get());
        
        // Clean up headers
        curl_slist_free_all(headers);
        
        if (res != CURLE_OK) {
            throw std::runtime_error("CURL request failed: " + std::string(curl_easy_strerror(res)));
        }
        
        // Get HTTP response code
        long http_code = 0;
        curl_easy_getinfo(curl_.get(), CURLINFO_RESPONSE_CODE, &http_code);
        
        if (http_code != 200) {
            throw std::runtime_error("HTTP request failed with code: " + std::to_string(http_code) + 
                                   "\nResponse: " + response);
        }
        
        // Parse JSON response
        try {
            Json::Value response_json;
            Json::CharReaderBuilder reader;
            std::string errors;
            std::istringstream response_stream(response);
            
            if (!Json::parseFromStream(reader, response_stream, &response_json, &errors)) {
                throw std::runtime_error("Failed to parse JSON response: " + errors);
            }
            
            return parseChatCompletionResponse(response_json);
        } catch (const std::exception& e) {
            throw std::runtime_error("Failed to parse JSON response: " + std::string(e.what()));
        }
    }

private:
    ChatCompletionResponse parseChatCompletionResponse(const Json::Value& j) {
        ChatCompletionResponse response;
        
        response.id = j.get("id", "").asString();
        response.object = j.get("object", "").asString();
        response.created = j.get("created", 0).asInt64();
        response.model = j.get("model", "").asString();
        
        // Parse choices
        if (j.isMember("choices") && j["choices"].isArray()) {
            for (const auto& choice : j["choices"]) {
                Choice c;
                c.index = choice.get("index", 0).asInt();
                c.finish_reason = choice.get("finish_reason", "").asString();
                
                if (choice.isMember("message")) {
                    c.message.role = choice["message"].get("role", "").asString();
                    c.message.content = choice["message"].get("content", "").asString();
                }
                
                response.choices.push_back(c);
            }
        }
        
        // Parse usage
        if (j.isMember("usage")) {
            response.usage.prompt_tokens = j["usage"].get("prompt_tokens", 0).asInt();
            response.usage.completion_tokens = j["usage"].get("completion_tokens", 0).asInt();
            response.usage.total_tokens = j["usage"].get("total_tokens", 0).asInt();
        }
        
        return response;
    }
};

// Helper function to create a simple chat request
std::vector<OpenAIClient::ChatMessage> createSimpleChat(const std::string& system_prompt, const std::string& user_message) {
    return {
        {OpenAIClient::ChatMessage{"system", system_prompt}},
        {OpenAIClient::ChatMessage{"user", user_message}}
    };
}

// Function to print usage information
void printUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [OPTIONS]\n"
              << "Options:\n"
              << "  -k, --api-key KEY     API key for authentication (required)\n"
              << "  -u, --url URL         Base URL for API (default: https://api.deepseek.com)\n"
              << "  -m, --model MODEL     Model to use (default: deepseek-chat)\n"
              << "  -h, --help            Show this help message\n"
              << "  -v, --version         Show version information\n\n"
              << "Examples:\n"
              << "  " << program_name << " -k your-api-key-here\n"
              << "  " << program_name << " -k your-api-key -u https://api.openai.com -m gpt-3.5-turbo\n"
              << "  " << program_name << " --api-key your-key --url https://api.deepseek.com --model deepseek-chat\n";
}

int main(int argc, char* argv[]) {
    // Default values
    std::string api_key;
    std::string base_url = "https://api.deepseek.com";
    std::string model = "deepseek-chat";
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        }
        
        if (arg == "-v" || arg == "--version") {
            std::cout << "ChatGPT Demo v1.0.0\n";
            return 0;
        }
        
        if (arg == "-k" || arg == "--api-key") {
            if (i + 1 < argc) {
                api_key = argv[++i];
            } else {
                std::cerr << "Error: -k/--api-key requires a value\n";
                printUsage(argv[0]);
                return 1;
            }
        }
        
        if (arg == "-u" || arg == "--url") {
            if (i + 1 < argc) {
                base_url = argv[++i];
            } else {
                std::cerr << "Error: -u/--url requires a value\n";
                printUsage(argv[0]);
                return 1;
            }
        }
        
        if (arg == "-m" || arg == "--model") {
            if (i + 1 < argc) {
                model = argv[++i];
            } else {
                std::cerr << "Error: -m/--model requires a value\n";
                printUsage(argv[0]);
                return 1;
            }
        }
    }
    
    // Check if API key is provided
    if (api_key.empty()) {
        std::cerr << "Error: API key is required. Use -k or --api-key to provide it.\n\n";
        printUsage(argv[0]);
        return 1;
    }
    
    try {
        // Initialize CURL globally
        curl_global_init(CURL_GLOBAL_ALL);
        
        // Create client with provided parameters
        OpenAIClient client(api_key, base_url);
        
        // Create chat request
        OpenAIClient::ChatCompletionRequest request;
        request.model = model;
        request.messages = createSimpleChat(
            "You are a helpful assistant.",
            "Hello! Can you tell me a short joke?"
        );
        request.stream = false;
        request.temperature = 0.7;
        request.max_tokens = 100;
        
        // Print configuration
        std::cout << "=== Configuration ===" << std::endl;
        std::cout << "Base URL: " << base_url << std::endl;
        std::cout << "Model: " << model << std::endl;
        std::cout << "API Key: " << std::string(api_key.length() > 8 ? api_key.substr(0, 8) + "..." : api_key) << std::endl;
        std::cout << std::endl;
        
        // Send request
        std::cout << "Sending chat completion request..." << std::endl;
        auto response = client.chatCompletion(request);
        
        // Print response
        std::cout << "\n=== Chat Completion Response ===" << std::endl;
        std::cout << "ID: " << response.id << std::endl;
        std::cout << "Model: " << response.model << std::endl;
        std::cout << "Created: " << response.created << std::endl;
        
        for (const auto& choice : response.choices) {
            std::cout << "\nChoice " << choice.index << ":" << std::endl;
            std::cout << "Role: " << choice.message.role << std::endl;
            std::cout << "Content: " << choice.message.content << std::endl;
            std::cout << "Finish Reason: " << choice.finish_reason << std::endl;
        }
        
        std::cout << "\nUsage:" << std::endl;
        std::cout << "  Prompt tokens: " << response.usage.prompt_tokens << std::endl;
        std::cout << "  Completion tokens: " << response.usage.completion_tokens << std::endl;
        std::cout << "  Total tokens: " << response.usage.total_tokens << std::endl;
        
        // Clean up CURL globally
        curl_global_cleanup();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        curl_global_cleanup();
        return 1;
    }
    
    return 0;
}
