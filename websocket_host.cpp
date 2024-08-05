#include <iostream>
#include <vector>
#include <App.h>
#include <random>

std::vector<char> serializeFloatVector(const std::vector<float>& data) {
    const char* bytes = reinterpret_cast<const char*>(data.data());
    return std::vector<char>(bytes, bytes + sizeof(float) * data.size());
}

std::vector<char> white_noise_gen() {
    static int id = 0;
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
    std::vector<float> dataVec(512);

    for (int i = 0; i < 512; ++i) {
        dataVec[i] = dis(gen);
    }

    if (id == 469) {
        std::cout << "469 Buffers sent!" << std::endl;
    }
    id++;

    return serializeFloatVector(dataVec);
}

struct PerSocketData {};

int main() {
    uWS::App().ws<PerSocketData>("/fetch", {
        .open = [](auto* ws) {
            std::cout << "Connection Opened" << std::endl;
        },
        .message = [](auto* ws, std::string_view message, uWS::OpCode opCode) {
            if (message == "fetch") {
                std::vector<char> serializedData = white_noise_gen();

                ws->send(std::string_view(serializedData.data(), serializedData.size()), uWS::OpCode::BINARY);
            } else {
                std::cout << "Undefined Message received: " << message << std::endl;
            }
        },
        .close = [](auto* ws, int code, std::string_view message) {
            std::cout << "Received Message: " << message << " and code: " << code << std::endl;
        }
    }).listen(9001, [](auto* token) {
        if (token)
            std::cout << "Listening on port 9001" << std::endl;
    }).run();

    return 0;
}

