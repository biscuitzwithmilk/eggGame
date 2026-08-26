#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "csvutils.h"
#include "jsonutils.h"

void readSection::ClearAllLineEffects(readSection::line &currentLine){
    currentLine.effects.clear();
}
void readSection::getEffect(readSection::line &currentLine, std::string tag, std::string endTag, std::string &output){
    readSection::effect currentEffect;
    std::string text;
    while(output.find(tag)!= std::string::npos){
        currentEffect.effectName=tag.substr(1,tag.length()-2);
        currentEffect.startPos=output.find(tag);
        currentEffect.endPos=output.find(endTag);
        text=output.substr(0,currentEffect.startPos);
        text+=output.substr(currentEffect.startPos+tag.length(),currentEffect.endPos-currentEffect.startPos-tag.length());
        text+=output.substr(currentEffect.endPos+endTag.length());
        currentEffect.effectedText=output.substr(currentEffect.startPos+tag.length(),currentEffect.endPos-currentEffect.startPos-tag.length());
        output = text;
        currentEffect.endPos -= (tag.length());
        if(currentLine.effects.size()!=0){
            for(int i=0; i<currentLine.effects.size(); i++){
                if(currentLine.effects[i].effectName!=currentEffect.effectName){
                    currentLine.effects[i].endPos-=tag.length();
                    currentLine.effects[i].startPos-=tag.length();
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

