#include "basicutils.h"
#include <iostream>
#include <sstream>
#include <string>

int basicutils::getRGBChannelFromHex(std::string hex, std::string rgbChannel){
    int color;
    std::stringstream colorstream;
    if(rgbChannel=="RED"){
        colorstream << hex.substr(0,2);
    }
    else if(rgbChannel=="GREEN"){
        colorstream << hex.substr(2,2);
    }
    else if(rgbChannel=="BLUE"){
        colorstream << hex.substr(4,2);
    }else {
        std::cerr << "Please input a valid color channel" << "\n";
        return 0;
    }
    colorstream >> std::hex >> color;
    return color;
};
void basicutils::getRGBColorFromHex(basicutils::color currentColor, std::string hex){
    hex=hex.substr(1);
    currentColor.red = basicutils::getRGBChannelFromHex(hex, "RED");
    currentColor.green = basicutils::getRGBChannelFromHex(hex, "GREEN");
    currentColor.blue = basicutils::getRGBChannelFromHex(hex, "BLUE");
}