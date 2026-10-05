#include "getSections.h"
void Effect::addDefault(std::string &input){
    std::string_view inputView=input;
    std::string nameDefault=Effect::effectName+"_default";
    size_t openStart = inputView.find('<'+nameDefault);
    if(openStart==std::string::npos) return;
    size_t openEnd = inputView.find('>',openStart);
    std::string textValue=input.substr(openStart+nameDefault.size()+2,openEnd-openStart-nameDefault.size()-2);
    try{
        if(textValue.find('.')==std::string::npos){
            Effect::defaultValue=std::stoi(textValue);
        }
        else if(textValue.find("f")!=std::string::npos){
            Effect::defaultValue = std::stof(textValue);
        }
        else{
            Effect::defaultValue = std::stod(textValue);
        }
    }
    catch (std::exception){
        Effect::defaultValue=textValue;
    }
    input.erase(openStart, openEnd-openStart+1);
    return;
}
void Effect::addEffect(std::string &input){
    std::string_view inputView=input;
    size_t openStart = inputView.find('<'+Effect::effectName);
    if(openStart==std::string::npos){
        std::string in="</";
        size_t closeStart = inputView.find(in+Effect::effectName.c_str());
        //if no Effect with that name is found
        if(closeStart==std::string::npos){
            return;
        }
        Effect::endPos=closeStart;
        size_t closeEnd = input.find('>', closeStart);
        input.erase(closeStart, closeEnd-closeStart+1);
        return;
    }
    Effect::startPos=openStart;
    size_t openEnd = inputView.find('>',openStart);
    size_t begginingValue = inputView.find('<'+Effect::effectName+'=');
    if(begginingValue==std::string::npos){
        return;
    }
    std::string textValue=input.substr(openStart+Effect::effectName.size(),openEnd-openStart);
    try{
        if(textValue.find('.')==std::string::npos){
            Effect::currentValue=std::stoi(textValue);
        }
        else if(textValue.find("f")!=std::string::npos){
            Effect::currentValue = std::stof(textValue);
        }
        else{
            Effect::currentValue = std::stod(textValue);
        }
    }
    catch (std::exception){
        Effect::currentValue=textValue;
    }
    input.erase(openStart, openEnd-openStart+1);
    return;
}
Effect::Effect(const std::string name, std::string &input, std::variant<int, float, double, std::string> defaultValueInput){
    Effect::startPos=0;
    Effect::endPos=0;
    Effect::effectName=name.substr(1,name.size()-2);
    Effect::defaultValue=defaultValueInput;
    Effect::addDefault(input);
    Effect::addEffect(input);
}

void CSVParser::resizeMaps(size_t fullSizeParam){
    CSVParser::effectMap.resize(fullSizeParam);
}
void CSVParser::setCharacterDefaults(std::vector<CSVParser::characterDefault> &charDefaults, const std::string &currentCharName){
    for(auto &charDefault:charDefaults){
        if(charDefault.characterName==currentCharName){
            int i=0;
            for(auto &charDefaultEffect:charDefault.defaultEffects){
                CSVParser::effectNamesAndDefaults.at(charDefaultEffect.first)=charDefaultEffect.second;
                i++;
            }
        }
    }
}
void CSVParser::makeEffectMaps(std::multimap<size_t, std::variant<int, float, double, std::string>>& effectMapParam,
const std::pair<std::string,std::variant<int, float, double, std::string>>& effectNamesAndDefaultsParam,
const std::string& nameToFind){
    size_t name_to_find_pos=CSVParser::inputCopy.find(nameToFind);
    if(name_to_find_pos!=std::string::npos){
        effectMapParam.emplace(name_to_find_pos, effectNamesAndDefaultsParam.first);
        effectMapParam.emplace(name_to_find_pos, effectNamesAndDefaultsParam.second);
    }
    else{
        effectMapParam.emplace(0, effectNamesAndDefaultsParam.first);
        effectMapParam.emplace(0, effectNamesAndDefaultsParam.second);
    }
}
void CSVParser::findEffects(){
    size_t i=0;
    for(auto &effectNameAndDefault:CSVParser::effectNamesAndDefaults){
        std::string endSymbol="</";
        std::string effectStartName=effectNameAndDefault.first.substr(0,effectNameAndDefault.first.length()-1);
        std::string effectEndName=endSymbol+effectNameAndDefault.first.substr(1,effectNameAndDefault.first.length()-1);
        //startPosition
        CSVParser::makeEffectMaps(CSVParser::effectMap[i], effectNameAndDefault, effectStartName);
        //endPosition
        CSVParser::makeEffectMaps(CSVParser::effectMap[i+CSVParser::effectNamesAndDefaults.size()], effectNameAndDefault, effectEndName);
        i++;
    }
}
void CSVParser::sortEffects(){
    std::sort(CSVParser::effectMap.begin(), CSVParser::effectMap.end());
}
void CSVParser::createEffects(std::string &input){
    std::unordered_map<std::string, std::variant<int, float, double, std::string>> defaultsMap(
        CSVParser::effectNamesAndDefaults.begin(), CSVParser::effectNamesAndDefaults.end()
    );

    std::string effectName;

    for (const auto& multimapItem : CSVParser::effectMap) {
        for (const auto& [pos, value] : multimapItem) {
            
            if (const std::string* strVal = std::get_if<std::string>(&value)) {
                auto it = defaultsMap.find(*strVal);
                if (it != defaultsMap.end()) {
                    effectName = *strVal;
                    continue;
                }
            }
            
            if (effectName.empty()) continue;

            std::string_view rawName = std::string_view(effectName).substr(1, effectName.length() - 2);

            // Try to update an existing effect
            bool updated = false;
            for (auto &eff : CSVParser::effects) {
                if (eff.effectName == rawName) {
                    eff.addEffect(input);
                    updated = true;
                    break; 
                }
            }

            if(updated==true) continue;

            CSVParser::effects.emplace_back(effectName, input, value);
        }
    }
}
void CSVParser::parseCSV(std::string &input, const std::string &currentCharName){
    CSVParser::resizeMaps(fullSize);
    std::vector<CSVParser::characterDefault> characterDefaults={{  {"Egg", { {"<color>","WHITE"},{"<fontSize>",22}, {"<speed>",1} }}, 
                                                        {"Man", { {"<color>","BLUE"},{"<fontSize>",20}, {"<speed>",2} }}, }};
    CSVParser::setCharacterDefaults(characterDefaults, currentCharName);
    CSVParser::findEffects();
    CSVParser::sortEffects();
    CSVParser::createEffects(input);
}
CSVParser::CSVParser(const std::unordered_map<std::string,std::variant<int, float, double, std::string>> effectNamesAndDefaultsParam, std::string &input, const std::string &charNameParam){
    CSVParser::effectNamesAndDefaults=effectNamesAndDefaultsParam;
    fullSize=CSVParser::effectNamesAndDefaults.size()*2;
    CSVParser::inputCopy=input;
    CSVParser::CSVParser::parseCSV(input, charNameParam);
}

void CSV::insertOutput(CSV::line &currentLine, std::string output) {
    std::string column;
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
    CSVParser EffectParser({{"<color>","RED"},{"<fontSize>",18}, {"<speed>",3}}, column, currentLine.charName);
    currentLine.effects=EffectParser.effects;
    currentLine.translation = column;
    std::getline(rowStream, column, ',');
}
CSV::CSV(std::string path) {
    CSV::iFile.open(path);
    CSV::getNextSection();
}
void CSV::getNextSection() { 
    CSV::section currentSection;
    std::string output;
    CSV::line currentLine;
    if (!CSV::iFile.is_open()) {
        std::cerr << "getNextSection : iFile failed to open\n";
        return;
    }
    if (!std::getline(iFile, output)) {
        std::cerr << "getNextSection : Failed to get line / cannot read new Section\n";
        return;
    }
    currentSection.name = output;
    while (std::getline(CSV::iFile, output)) {
        if (output.find("//endsection") != std::string::npos) break;
        if (output.empty()) continue;
        
        CSV::insertOutput(currentLine, output);
        currentSection.lines.push_back(currentLine);
    }
    CSV::sections.push_back(currentSection);
}
