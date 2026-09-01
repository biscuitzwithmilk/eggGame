#include <fstream>
#include "jsonutils.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

json jsonUtils::getJson(std::string filepath, const std::string& keys){
    json jsonfile;
    std::ifstream iFile;
    iFile.open(filepath);
    iFile >> jsonfile;
    return jsonfile[nlohmann::json::json_pointer(keys)];

}