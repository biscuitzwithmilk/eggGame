#include <any>
#include <fstream>
#include <vector>
#include <functional>
#include <iostream>
#include <string>
#include <variant>
#include "windowManager.h"
#include "getSections.h"
#include "readLanguages.h"
#include "jsonutils.h"
#include "inputMap.h"
using namespace std;
namespace rlib{
    #include <raylib.h>
};

struct PictureAsset{
    rlib::Image image;
    rlib::Texture2D texture;
};
struct BackgroundImage{
    PictureAsset combined;
    PictureAsset depth;
};
PictureAsset LoadImageGeneral(const std::string& name){
    PictureAsset asset;

    std::string image = name+".png";
    asset.image = rlib::LoadImage(image.c_str());
    asset.texture = LoadTextureFromImage(asset.image);
    
    return asset;
};
BackgroundImage LoadBackground(const std::string& name){
    BackgroundImage background;

    std::string depth = name+"_depth";
    background.combined = LoadImageGeneral(name);
    background.depth = LoadImageGeneral(depth);

    return background;
};

void onWindowMove(window &window1, PictureAsset asset){

    window1.transform.scale.Width = rlib::GetScreenWidth();
    window1.transform.scale.Height = rlib::GetScreenHeight();

    window1.toImageRatio.X=asset.image.width/window1.transform.scale.Width; //add current image
    window1.toImageRatio.Y=asset.image.height/window1.transform.scale.Height;
};

int main(){  
    ifstream iFile;
    iFile.open(jsonUtils::getJson("game/json/textSettings.json", "/CurrentLanguage/path"));
    Section::section currentSection;
    Section::getToSection(iFile, currentSection, jsonUtils::getJson("game/json/textSettings.json", "/CurrentSection"));
    int totLang;
    int currentLine=jsonUtils::getJson("game/json/textSettings.json", "/CurrentLine");
    int currentEffect=0;
    int currentlanguage = jsonUtils::getJson("game/json/textSettings.json", "/CurrentLanguage/key");
    window window1;
    int codepointSize = 0;
    int codepoint;
    vector<float> currentFontSize;
    vector<rlib::Color> currentColors;
    float defaultFontSize;
    rlib::Color defaultColor; 
    rlib::Color currentColor;
    vector<rlib::Vector2> currentSpecialPos;
    rlib::SetConfigFlags(rlib::FLAG_BORDERLESS_WINDOWED_MODE | rlib::FLAG_VSYNC_HINT);
    rlib::SetTargetFPS(window1.targetFPS);
    rlib::InitWindow(window1.transform.scale.Width,window1.transform.scale.Height, "test game");

    while(!input::isQuitting()){
        rlib::BeginDrawing();
        rlib::ClearBackground(rlib::BLACK);
        if(input::isRClickPressed()){
            getLang::getLanguages();
            totLang = jsonUtils::getJson("game/json/textSettings.json", "/TotalLanguages").get<int>();
            currentlanguage+=1;
            if(currentlanguage>totLang-1){
                currentlanguage=0;
            }
            getLang::changeLanguage(currentSection, currentlanguage);
            // Setup per-codepoint arrays (do not draw here)
            currentSpecialPos.clear();

            const char *textPtr = currentSection.lines[currentLine].translation.c_str();
            size_t textLen = currentSection.lines[currentLine].translation.size();
            for(size_t n=0; n<textLen; n++){
                rlib::Vector2 v = {0.0f, 220.0f};
                currentSpecialPos.push_back(v);
            }
            if(jsonUtils::getJson("game/json/debug.json", "/leftClickDebug") == true){
                std::cout << currentSection.lines[currentLine].key << ", ";
                std::cout << currentSection.lines[currentLine].charName << ", ";
                std::cout << currentSection.lines[currentLine].context << ", ";
                std::cout << currentSection.lines[currentLine].english << ", ";
                std::cout << currentSection.lines[currentLine].translation << "\n";
                for(int i; i<=currentSection.lines[currentLine].effects.size();i++){
                    std::cout << currentSection.lines[currentLine].effects[i].effectName << ", ";
                    std::cout << currentSection.lines[currentLine].effects[i].effectedText << ", ";
                    std::cout << currentSection.lines[currentLine].effects[i].startPos << ", ";
                    std::cout << currentSection.lines[currentLine].effects[i].endPos << "\n";
                }
            }
            
            
        }
        if(input::isLClickPressed()){
            if(currentLine>=currentSection.lines.size()-1){
                currentLine=0;
            }
            currentLine+=1;
            jsonUtils::editJson("game/json/textSettings.json", "/CurrentLine", currentLine);

            // Setup per-codepoint arrays (do not draw here)
            currentSpecialPos.clear();

            const char *textPtr = currentSection.lines[currentLine].translation.c_str();
            size_t textLen = currentSection.lines[currentLine].translation.size();
            for(size_t n=0; n<textLen; n++){

                rlib::Vector2 v = {0.0f, 220.0f};
                currentSpecialPos.push_back(v);
            }
            if(jsonUtils::getJson("game/json/debug.json", "/leftClickDebug") == true){
                std::cout << currentSection.lines[currentLine].key << ", ";
                std::cout << currentSection.lines[currentLine].charName << ", ";
                std::cout << currentSection.lines[currentLine].context << ", ";
                std::cout << currentSection.lines[currentLine].english << ", ";
                std::cout << currentSection.lines[currentLine].translation << "\n";
                std::cout << currentSection.lines[currentLine].effects.size() << "\n";
                for(int i=0; i<currentSection.lines[currentLine].effects.size();i++){
                    std::cout << currentSection.lines[currentLine].effects[i].effectName << ", ";
                    std::cout << currentSection.lines[currentLine].effects[i].effectedText << ", ";
                    std::cout << currentSection.lines[currentLine].effects[i].startPos << ", ";
                    std::cout << currentSection.lines[currentLine].effects[i].endPos << "\n";
                }
            }
        }

        const char *textPtr = currentSection.lines[currentLine].translation.c_str();
        size_t textLen = currentSection.lines[currentLine].translation.size();
        int cpIndex = 0;
        float xPos = 180.0f;

        
        // }
        // for(size_t n=0; n<textLen; ){
            
        //     // codepointSize = 0;
        //     // codepoint = rlib::GetCodepoint(&textPtr[n], &codepointSize);
        //     // if(codepointSize <= 0) break;

        //     // // guard against mismatched lengths
        //     // // if(cpIndex >= (int)currentFontSize.size() || cpIndex >= (int)currentSpecialPos.size() || cpIndex >= (int)currentColors.size()) break;

        //     // // set position for this glyph, draw, then advance xPos for the next glyph
        //     // currentSpecialPos[cpIndex].x = xPos;
        //     // rlib::DrawTextCodepoint(rlib::GetFontDefault(), codepoint, currentSpecialPos[cpIndex], currentFontSize[cpIndex], rlib::RED);

        //     // int glyphIndex = rlib::GetGlyphIndex(rlib::GetFontDefault(), codepoint);
        //     // float advance = 0.0f;
        //     // if(rlib::GetFontDefault().glyphs[glyphIndex].advanceX == 0){
        //     //     advance = (float)rlib::GetFontDefault().recs[glyphIndex].width * (currentFontSize[cpIndex] / rlib::GetFontDefault().baseSize) + 2.0f;
        //     // }else{
        //     //     advance = (float)rlib::GetFontDefault().glyphs[glyphIndex].advanceX * (currentFontSize[cpIndex] / rlib::GetFontDefault().baseSize) + 2.0f;
        //     // }

        //     // xPos += advance;

        //     // n += (size_t)codepointSize;
        //     // cpIndex++;
        // }

        
        // rlib::DrawText(rlib::TextFormat(currentSection.lines[currentLine].charName.c_str()), 0, 0, 20, rlib::RED);
        // rlib::DrawText(rlib::TextFormat(currentSection.lines[currentLine].translation.c_str()), 0, 20, 20, rlib::RED);
        rlib::EndDrawing();
    }
    rlib::CloseWindow();
    // rlib::SetWindowState(window1.State);

    // BackgroundImage eggImage1;
    // eggImage1 = LoadBackground("egg");


    // Shader shader = LoadShader(NULL, "depthfocus.fs");

    // int subColorLoc = GetShaderLocation(shader, "subtractColor");
    // int spread = GetShaderLocation(shader, "spread");
    // float valueToMultiply = 1.0f;
    // float colorToSubtract[4] = {0.5f, 0.5f, 0.5f, 0.5f};
    // while (isQuitting() == false)
    // {
    //     if(window1.isMoving()){
    //         onWindowMove(window1, eggImage1.depth);
    //     }
        
    //     Color currentColor = GetImageColor(eggImage1.depth.image, GetMouseX()*window1.toImageRatio.X, GetMouseY()*window1.toImageRatio.Y);
    //     colorToSubtract[0] = currentColor.r/255.0f;
    //     colorToSubtract[1] = currentColor.g/255.0f;
    //     colorToSubtract[2] = currentColor.b/255.0f;
        
    //     SetShaderValue(shader, subColorLoc, colorToSubtract, SHADER_UNIFORM_VEC4);
    //     SetShaderValue(shader, spread, &valueToMultiply, SHADER_UNIFORM_FLOAT);
    //     Rectangle sourceRec = {0.0f, 0.0f, float(eggImage1.combined.texture.width), float(eggImage1.combined.texture.height)};
    //     Rectangle destRec = {0.0f,0.0f,float(window1.transform.scale.Width),float(window1.transform.scale.Height)};

    //     BeginDrawing();
    //     ClearBackground(BLACK);
    //     BeginShaderMode(shader);
    //     DrawTexturePro(eggImage1.depth.texture, sourceRec, destRec, {0.0f,0.0f}, 0.0f, WHITE);
    //     EndShaderMode();


    //     EndDrawing();
    // }
    // CloseWindow();
    
    return 0;
}
