#include <filesystem>
#include <fstream>
#include <string>
#include <iostream>
#include "readLanguages.h"
#include "jsonutils.h"
namespace fs = std::filesystem;

getLang::languages getLang::getLanguages(){
    json languageConfig;
    getLang::languages translation;
    std::string text = "";
    for (auto const& dir_entry : fs::recursive_directory_iterator("game/languages")){

        if(dir_entry.path().extension() == ".csv"){

            text += dir_entry.path().string()+"\n";
            translation.nbrLanguages++;
        }
    }
    jsonUtils::editJson("game/json/textSettings.json", "/TotalLanguages", translation.nbrLanguages);
    translation.languageList = new getLang::language[translation.nbrLanguages];
    for(int i = 0; i<translation.nbrLanguages; i++){
        std::string currentPath = text.substr(0, text.find("\n"));
        translation.languageList[i].path = currentPath;
        text = text.substr(text.find("\n")+1);
    }
    return translation;
}

bool getLang::changeLanguage(readSection::section &currentSection, int language){
    std::ifstream iFile;
    getLang::languages translationList = getLang::getLanguages();
    if (language < 0 || language >= translationList.nbrLanguages){
        std::cerr << "Invalid language index: " << language << "\n";
        return false;
    }

    std::string goalsection = currentSection.name;

    if(iFile.is_open()){
        iFile.close();
    }
    currentSection.lines.clear();
    iFile.open(translationList.languageList[language].path);
    if (!iFile.is_open()){
        std::cerr << "Failed to open CSV file: " << translationList.languageList[language].path << "\n";
        return false;
    }
    jsonUtils::editJson("game/json/textSettings.json", "/CurrentLanguage/path", translationList.languageList[language].path);
    jsonUtils::editJson("game/json/textSettings.json", "/CurrentLanguage/key", language);
    readSection::getNextSection(iFile, currentSection);
    readSection::getToSection(iFile, currentSection, goalsection);

    if (currentSection.name != goalsection){
        std::cerr << "Failed to find section: " << goalsection << " in language file\n";
        return false;
    }
    return true;
}
