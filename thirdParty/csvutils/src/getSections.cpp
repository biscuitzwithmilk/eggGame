#include <algorithm>
#include <cctype>
#include <exception>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <variant>
#include <vector>
#include "getSections.h"
#include "jsonutils.h"

void Effect::ClearAllLineEffects(Section::line &currentLine){
    currentLine.effects.clear();
}
void Effect::addEffect(Section::line &currentLine, const std::string &name, const std::string &endName, std::string &output){
    if (name.empty() || endName.empty() || output.empty()) {
        return;
    }

    std::string working = output;
    std::string_view nameView = name;
    std::string_view tagName = nameView.substr(1, name.length() - 2);

    std::string namePrefix = std::format("{}=", nameView.substr(0, name.length() - 1));
    std::string defaultPrefix = std::format("{}_default=", nameView.substr(0, name.length() - 1));
    std::string_view namePrefixView = namePrefix;
    std::string_view defaultPrefixView = defaultPrefix;
    basicutils::Param currentValue;
    std::vector<std::string> characterList = (jsonUtils::getJson("game/json/characters.json", "/characterList").get<std::vector<std::string>>());

    while (working.find(tagName) != std::string::npos) {
        if (working.find(endName) == std::string::npos && working.find(defaultPrefixView) == std::string::npos) {
            break;
        }

        size_t openStart = std::string::npos;
        size_t openEnd = std::string::npos;
        size_t openDefStart = std::string::npos;
        size_t openDefEnd = std::string::npos;
        size_t openDefLen = 0;
        Effect::effect currentEffect;
        currentEffect.effectName = std::string(tagName);
        currentEffect.value.clear();

        size_t defaultPos = working.find(defaultPrefixView);
        if (defaultPos != std::string::npos) {
            openDefStart = defaultPos;
            openDefEnd = working.find(">", openDefStart);
            if (openDefEnd == std::string::npos) {
                break;
            }
            openDefLen = openDefEnd - openDefStart + 1;
            std::string defaultValueText = working.substr(openDefStart + defaultPrefixView.length(), openDefEnd - (openDefStart + defaultPrefixView.length()));
            try {
                if(defaultValueText.find(".")==std::string::npos){
                    currentEffect.defaultValue.value = std::stoi(defaultValueText);
                }
                else if(defaultValueText.find("f")!=std::string::npos){
                    currentEffect.defaultValue.value = std::stof(defaultValueText);
                }
                else{
                    currentEffect.defaultValue.value = std::stod(defaultValueText);
                }
            } catch (std::exception) {
                currentEffect.defaultValue.value = defaultValueText;
            }
            working.erase(openDefStart, openDefLen);
        }
        else if(std::find(characterList.begin(), characterList.end(), currentLine.charName)!=characterList.end()){
            const std::string key="/"+currentLine.charName+"/"+std::string(defaultPrefixView.substr(1, defaultPrefixView.length()-2));
            const json jsonValue = jsonUtils::getJson("game/json/characters.json", key);
            if (jsonValue.is_number_integer()) {
                currentEffect.defaultValue.value = jsonValue.get<int>();
            } else if (jsonValue.is_number_float()) {
                currentEffect.defaultValue.value = jsonValue.get<float>();
            } else if (jsonValue.is_string()) {
                currentEffect.defaultValue.value = jsonValue.get<std::string>();
            }
        }
        else {
            const std::string key="/DialogueEffects/"+std::string(defaultPrefixView.substr(1, defaultPrefixView.length()-2));
            const json jsonValue = jsonUtils::getJson("game/json/textSettings.json", key);
            if (jsonValue.is_number_integer()) {
                currentEffect.defaultValue.value = jsonValue.get<int>();
            } else if (jsonValue.is_number_float()) {
                currentEffect.defaultValue.value = jsonValue.get<float>();
            } else if (jsonValue.is_string()) {
                currentEffect.defaultValue.value = jsonValue.get<std::string>();
            }
        }

        size_t valuePos = working.find(namePrefixView);
        if (valuePos != std::string::npos) {
            openStart = valuePos;
            openEnd = working.find(">", openStart);
            if (openEnd == std::string::npos) {
                break;
            }
            std::string valueText = working.substr(openStart + namePrefixView.length(), openEnd - (openStart + namePrefixView.length()));
            try {
                if(valueText.find(".")==std::string::npos){
                    currentValue.value = std::stoi(valueText);
                }
                else if(valueText.find("f")!=std::string::npos){
                    currentValue.value = std::stof(valueText);
                }
                else{
                    currentValue.value = std::stod(valueText);
                }
            } catch (std::exception) {
                currentValue.value = valueText;
            }
        }
        else {
            openStart = working.find(name);
            if (openStart == std::string::npos) {
                currentEffect.startPos = 0;
                currentEffect.endPos = 0;
                currentEffect.effectName = std::string(defaultPrefixView.substr(1, defaultPrefixView.size()-2));
                currentEffect.value.push_back(currentEffect.defaultValue);
                currentLine.effects.push_back(currentEffect);
                output = working;
                return;
            }
            openEnd = openStart + name.length() - 1;
            currentValue = currentEffect.defaultValue;
        }

        size_t openTagLen = openEnd - openStart + 1;
        currentEffect.startPos = static_cast<int>(openStart);
        working.erase(openStart, openTagLen);

        size_t closeStart = working.find(endName);
        if (closeStart == std::string::npos) {
            break;
        }

        currentEffect.endPos = static_cast<int>(closeStart);
        size_t closeTagLen = endName.length();
        working.erase(closeStart, closeTagLen);

        for (size_t effect = 0; effect < currentLine.effects.size(); ++effect) {
            currentLine.effects[effect].value.clear();
            if (currentLine.effects[effect].startPos > static_cast<int>(openStart)) {
                currentLine.effects[effect].startPos -= openTagLen + openDefLen;
                currentLine.effects[effect].endPos -= openTagLen + openDefLen;
                currentLine.effects[effect].value.reserve(openTagLen + closeTagLen + openDefLen);
            }
        }

        for (size_t n = 0; n < working.size(); ++n) {
            if (n < static_cast<size_t>(currentEffect.startPos) || n >= static_cast<size_t>(currentEffect.endPos)) {
                currentEffect.value.push_back(currentEffect.defaultValue);
            } else {
                currentEffect.value.push_back(currentValue);
            }
        }

        currentLine.effects.push_back(currentEffect);
    }

    output = working;
}
bool Section::getNextSection(std::ifstream &iFile, Section::section &currentSection){
    std::string output;
    std::string column;
    Section::line currentLine;
    Effect::effect currentEffect;

    if (!iFile.is_open()){
        std::cerr << "getNextSection : Failed to open file\n";
        return false;
    }

    if (!std::getline(iFile, output)){
        std::cerr << "getNextSection : Failed to get line / cannot read new Section\n";
        return false;
    }

    currentSection.name = output;
    jsonUtils::editJson("game/json/textSettings.json", "/CurrentSection", output);

    while (std::getline(iFile, output)){
        if (output.find("//endsection") != std::string::npos){
            break;
        };
        if (output.empty()){
            continue;
        };
        //separate and insert output
        std::stringstream rowStream(output);
        std::getline(rowStream, column, ',');
        currentLine.key = column;
        std::getline(rowStream, column, ',');
        currentLine.charName = column;
        std::getline(rowStream, column, ',');
        currentLine.context = column;
        std::getline(rowStream, column, ',');
        currentLine.english = column;
        std::getline(rowStream, column, ',');
        Effect::ClearAllLineEffects(currentLine);
        Effect::addEffect(currentLine, "<shakey>", "</shakey>", column);
        Effect::addEffect(currentLine, "<fontSize>", "</fontSize>", column);
        Effect::addEffect(currentLine, "<speed>", "</speed>", column);
        Effect::addEffect(currentLine, "<color>", "</color>", column);
        Effect::addEffect(currentLine, "<bold>", "</bold>", column);
        
        currentLine.translation = column;
        if(jsonUtils::getJson("game/json/debug.json", "/nextSectionDebug") == true){
            std::cout << currentLine.key << ", ";
            std::cout << currentLine.charName << ", ";
            std::cout << currentLine.context << ", ";
            std::cout << currentLine.english << ", ";
            std::cout << currentLine.translation << "\n";
            
        }
        currentSection.lines.push_back(currentLine);
    };
    if(jsonUtils::getJson("game/json/debug.json", "/nextSectionDebug") == true){
        for(int currentLine=0; currentLine<currentSection.lines.size(); currentLine++){
            for(int effect=0; effect<currentSection.lines[currentLine].effects.size();++effect){
                std::cout << "effect : \n" << currentSection.lines[currentLine].translation << ", ";
                std::cout << currentSection.lines[currentLine].effects[effect].effectName << ", ";
                std::cout << currentSection.lines[currentLine].effects[effect].startPos << ", ";
                std::cout << currentSection.lines[currentLine].effects[effect].endPos << ", ";
                std::cout << "int : " << currentSection.lines[currentLine].effects[effect].defaultValue.as<int>();
                std::cout << "float : " << currentSection.lines[currentLine].effects[effect].defaultValue.as<float>();
                std::cout << "string : " << currentSection.lines[currentLine].effects[effect].defaultValue.as<std::string>();
                    
            }
        }
    }

    if (static_cast<int>(currentSection.lines.size()) == 0){
        std::cerr << "getNextSection : Failed to get to section\n";
        return false;
    }
    return true;
}
bool Section::getToSection(std::ifstream &iFile, Section::section &currentSection, const std::string &sectionName){
    if (!iFile.is_open()){
        return false;
    }
    iFile.clear();
    while (currentSection.name != sectionName && iFile.good()){
        if(!Section::getNextSection(iFile, currentSection)){
            return false;
        }
    }
    return true;


    

}

