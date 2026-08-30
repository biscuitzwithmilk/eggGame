#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "csvutils.h"
#include "jsonutils.h"

void readSection::ClearAllLineEffects(readSection::line &currentLine){
    currentLine.effects.clear();
}
void readSection::getEffect(readSection::line &currentLine, std::string name, std::string endName, std::string &output){
    readSection::effect currentEffect;
    int nameLength;
    std::string tagPrefix = name.substr(0,name.length()-1)+"=";

    while(output.find(name.substr(1,name.length()-1))!= std::string::npos){

        if(output.find(endName)==std::string::npos){
            return;
        }
        currentEffect.effectName=name.substr(1,name.length()-2);
        if(output.find(tagPrefix)==std::string::npos){
            currentEffect.startPos=output.find(name);
            nameLength = output.substr(currentEffect.startPos, output.find(">", currentEffect.startPos)-currentEffect.startPos).length()+1;
            output=output.erase(currentEffect.startPos, name.length());
            currentEffect.value=0;
            currentEffect.endPos=output.find(endName);
            output=output.erase(currentEffect.endPos, endName.length());

            if(currentLine.effects.size()!=0){
                for(int i=0; i<currentLine.effects.size(); i++){
                    if(currentLine.effects[i].effectName!=currentEffect.effectName){
                        currentLine.effects[i].endPos-=nameLength;
                        currentLine.effects[i].startPos-=nameLength;
                    }
                }
            }
        }
        else {
            currentEffect.startPos=output.find(tagPrefix);
            nameLength = output.substr(currentEffect.startPos, output.find(">", currentEffect.startPos)-currentEffect.startPos).length()+1;
            output=output.erase(currentEffect.startPos, name.length());
            currentEffect.value=std::stoi(output.substr(currentEffect.startPos, output.find(">", currentEffect.startPos))); 
            output=output.erase(currentEffect.startPos, output.find(">", currentEffect.startPos)-currentEffect.startPos+1);
            currentEffect.endPos=output.find(endName);
            output=output.erase(currentEffect.endPos, endName.length());

            if(currentLine.effects.size()!=0){
                for(int i=0; i<currentLine.effects.size(); i++){
                    if(currentLine.effects[i].effectName!=currentEffect.effectName){
                        currentLine.effects[i].endPos-=nameLength;
                        currentLine.effects[i].startPos-=nameLength;
                    }
                }
            }
        }

        
        currentLine.effects.push_back(currentEffect);
    }
}
bool readSection::getNextSection(std::ifstream &iFile, readSection::section &currentSection){
    std::string output;
    std::string column;
    readSection::line currentLine;
    readSection::effect currentEffect;

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

        readSection::ClearAllLineEffects(currentLine);
        readSection::getEffect(currentLine, "<shakey>", "</shakey>", column);
        readSection::getEffect(currentLine, "<slow>", "</slow>", column);
        readSection::getEffect(currentLine, "<strong>", "</strong>", column);
        
        currentLine.translation = column;
        currentSection.lines.push_back(currentLine);
    };

    if (static_cast<int>(currentSection.lines.size()) == 0){
        std::cerr << "getNextSection : Failed to get to section\n";
        return false;
    }
    return true;
}
bool readSection::getToSection(std::ifstream &iFile, readSection::section &currentSection, const std::string &sectionName){
    if (!iFile.is_open()){
        return false;
    }
    iFile.clear();
    while (currentSection.name != sectionName && iFile.good()){
        if(!readSection::getNextSection(iFile, currentSection)){
            return false;
        }
    }
    return true;


    

}

