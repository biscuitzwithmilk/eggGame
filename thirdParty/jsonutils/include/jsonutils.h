#include "nlohmannJson.hpp"
#include <fstream>
using json = nlohmann::json;
namespace jsonUtils{
    template <typename T>
    void editJson(std::string filepath, const std::string& keys, const T& value){
        json jsonfile;
        std::ifstream iFile;
        iFile.open(filepath);
        iFile >> jsonfile;
        jsonfile[nlohmann::json::json_pointer(keys)] = value;
        std::ofstream oFile(filepath);
        oFile << jsonfile.dump(4);
        oFile.close();
    }
    json getJson(std::string filepath, const std::string& keys);
}