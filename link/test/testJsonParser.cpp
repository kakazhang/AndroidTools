#include <fstream>
#include <json/reader.h>

using namespace std;

int parseJsonFromFile(const char* filename) {
    Json::Reader reader;
    Json::Value root;

    ifstream is;
    is.open(filename, std::ios::binary);
    if (reader.parse(is, root)) {
        if (!root["weatherinfo"].isNull()) {
            string city = root["weatherinfo"]["city"].asString();
            string time = root["weatherinfo"]["time"].asString();
            printf("city:%s, time:%s\n", city.c_str(), time.c_str());
        }
    }
    is.close();
    return 0;
}

int main() {
    parseJsonFromFile("/etc/test.json");
    return 0;
}
