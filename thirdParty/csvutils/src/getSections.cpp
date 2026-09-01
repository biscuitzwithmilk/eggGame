#include <exception>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <any>
#include "getSections.h"
#include "jsonutils.h"

void Effect::ClearAllLineEffects(Section::line &currentLine){
    currentLine.effects.clear();
}
void Effect::addEffect(Section::line &currentLine, std::string name, std::string endName, std::string &output){
    Effect::effect currentEffect;
    int nameLength;
    std::string namePrefix = name.substr(0,name.length()-1)+"=";

    while(output.find(name.substr(1,name.length()-1))!= std::string::npos){
        if(output.find(endName)==std::string::npos){
            return;
        }

        size_t openStart = std::string::npos;
        size_t openEnd = std::string::npos;
        currentEffect.effectName=name.substr(1,name.length()-2);

        if(output.find(namePrefix)!=std::string::npos){
            openStart = output.find(namePrefix);
            openEnd = output.find(">", openStart);
            if (openEnd == std::string::npos) return;
            try {
                currentEffect.value = std::stof(output.substr(openStart + namePrefix.length(), openEnd - (openStart + namePrefix.length())));
            } catch (std::exception) {
                currentEffect.value = output.substr(openStart + namePrefix.length(), openEnd - (openStart + namePrefix.length()));
            }
            

        } else {
            openStart = output.find(name);
            if (openStart == std::string::npos) return;
            openEnd = openStart + name.length() - 1;
            // currentEffect.value<int> = 0;
        }
        size_t openTagLen = openEnd - openStart + 1;
        currentEffect.startPos = openStart;

        // Erase opening tag
        output.erase(openStart, openTagLen);

        // Find and erase closing tag
        size_t closeStart = output.find(endName);

        currentEffect.endPos = closeStart;
        size_t closeTagLen = endName.length();
        output.erase(closeStart, closeTagLen);

       // Correct positions of all existing effects based on relative layout
        for (auto &eff : currentLine.effects) {
            // Adjust startPos if it was after erased segments
            if (eff.startPos > openStart) {
                eff.startPos -= (eff.startPos > closeStart) ? (openTagLen + closeTagLen) : openTagLen;
            }
            // Adjust endPos if it was after erased segments
            if (eff.endPos > openStart) {
                eff.endPos -= (eff.endPos > closeStart) ? (openTagLen + closeTagLen) : openTagLen;
            }
        }

        
        currentLine.effects.push_back(currentEffect);
    }
}
void Effect::onEffect(Section::line &currentLine, const std::string &effectName, const std::function<void(std::string)> func, const std::string params){
    for(int i=0; i<currentLine.effects.size(); i++){
        if (currentLine.effects[i].effectName==effectName){
           func(params);
        }
    }
}
void Effect::onFontSize(std::string test){
    std::cout << test << std::endl;
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
        Effect::addEffect(currentLine, "<slow>", "</slow>", column);
        Effect::addEffect(currentLine, "<color>", "</color>", column);
        Effect::addEffect(currentLine, "<bold>", "</bold>", column);
        
        currentLine.translation = column;
        currentSection.lines.push_back(currentLine);
    };

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

