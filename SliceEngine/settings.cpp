#include <cctype>
#include <fstream>
#include <stdio.h>
#include "utils/logoutput.h"

#include "settings.h"

#define LTRIM_STRING(s) do { while(((s).length() > 0) && isspace((s)[0])) { (s).erase(0, 1); } } while(0)
#define RTRIM_STRING(s) do { while(((s).length() > 0) && isspace((s)[(s).length() - 1])) { (s).erase((s).length() - 1); } } while(0)
#define TRIM_STRING(s) do { LTRIM_STRING(s); RTRIM_STRING(s); } while(0)
#define STRINGIFY(_s) #_s
#define SETTING(name, default) do { _index.push_back(_ConfigSettingIndex(STRINGIFY(name), &name)); name = (default); } while(0)
#define SETTING2(name, altname, default) do { _index.push_back(_ConfigSettingIndex(STRINGIFY(name), &name)); _index.push_back(_ConfigSettingIndex(STRINGIFY(altname), &name)); name = (default); } while(0)

ConfigSettings::ConfigSettings()
{
    SETTING(layerThickness, 500);
    SETTING(initialLayerThickness, 0);
    SETTING(filamentDiameter, 1500);
    SETTING(filamentFlow, 100);
    SETTING(layer0extrusionWidth, 1800);
    SETTING(extrusionWidth, 800);
    SETTING(insetCount,2);
    SETTING(downSkinCount,6);
    SETTING(upSkinCount,6);
    SETTING(sparseInfillLineDistance, 10 * extrusionWidth / 20);
    SETTING(infillOverlap,0);
    SETTING(skirtDistance,6000);
    SETTING(skirtLineCount,1);
    SETTING(skirtMinLength,0);

    SETTING(skirtDistance,0);
    SETTING(skirtLineCount,0);
    SETTING(skirtMinLength,0);

    SETTING(initialSpeedupLayers, 4);
    SETTING(initialLayerSpeed, 20);
    SETTING(printSpeed, 30);
    SETTING(infillSpeed, 30);
    SETTING(inset0Speed, 30);
    SETTING(insetXSpeed, 30);
    SETTING(moveSpeed, 150);
    SETTING(fanFullOnLayerNr, 2);

    SETTING(supportType,0);
    SETTING(supportAngle, -1);
    SETTING(supportEverywhere,0);
    SETTING(supportLineDistance, sparseInfillLineDistance);
    SETTING(supportXYDistance,700);
    SETTING(supportZDistance,150);
    SETTING(supportExtruder,-1);

    SETTING(retractionAmount,4500);
    SETTING(retractionSpeed,45);
    SETTING(retractionAmountExtruderSwitch,14500);
    SETTING(retractionMinimalDistance,1500);
    SETTING(minimalExtrusionBeforeRetraction, 100);
    SETTING(retractionZHop,2500);    //printchooclate

    SETTING(enableCombing,1);
    SETTING(enableOozeShield, 0);
    SETTING(wipeTowerSize, 0);
    SETTING(multiVolumeOverlap,0);
//    SETTING2(objectPosition.X, posx, 102500);
//    SETTING2(objectPosition.Y, posy, 102500);
    SETTING2(objectPosition.X, posx, 90000);
    SETTING2(objectPosition.Y, posy, 90000);
    SETTING(objectSink, 0);

    SETTING(raftMargin, 5000);
    SETTING(raftLineSpacing, 1000);

    SETTING(raftBaseThickness, 0);
    SETTING(raftBaseLinewidth, 0);
    SETTING(raftInterfaceThickness,0);
    SETTING(raftInterfaceLinewidth,0);

    SETTING(minimalLayerTime,5);
    SETTING(minimalFeedrate,10);
    SETTING(coolHeadLift, 0);
    SETTING(fanSpeedMin,100);
    SETTING(fanSpeedMax,100);

    SETTING(fixHorrible, 0);
    SETTING(spiralizeMode, 0);
    SETTING(gcodeFlavor,GCODE_FLAVOR_REPRAP);//设置G-code种类

    memset(extruderOffset, 0, sizeof(extruderOffset));
    SETTING(extruderOffset[1].X, 0);
    SETTING(extruderOffset[1].Y, 0);
    SETTING(extruderOffset[2].X, 0);
    SETTING(extruderOffset[2].Y, 0);
    SETTING(extruderOffset[3].X, 0);
    SETTING(extruderOffset[3].Y, 0);

    //设置挤出机温度并等待
    //毫米为单位
    //设置成据对定位
    //移动到远点
    //        "M109 S210  "            "G28           ;Home \n"
//            "G92 E0        ;zero the extruded length\n";
            "M220 S50 \n";
    startCode =
        "G90           ;absolute positioning\n"
        "M4 P160 \n"
        "G28  X Y\n";


    endCode =

        "G28 X Y\n "
        "M4 P0\n"
        ";end";
}
void ConfigSettings::setSliceConfig(std::string speed,std::string excluder,std::string zvalue,std::string temps)
{

    startCode += speed;
    startCode += excluder;
    startCode += zvalue;
    startCode += temps;
    std::string po = "G1 Z8 F2000 \n";
    startCode += po;
}
void ConfigSettings::setPosConfig(int w,int h)
{
    SETTING2(objectPosition.X, posx, w);
    SETTING2(objectPosition.Y, posy, h);
}
void ConfigSettings::setPrintConfig(int rectA,int rectS,int flimant,int widthE)
{
    SETTING(filamentDiameter, flimant);
    SETTING(extrusionWidth, widthE);
    SETTING(retractionAmount,rectA);
    SETTING(retractionSpeed,rectS);
}
#undef STRINGIFY
#undef SETTING

bool ConfigSettings::setSetting(const char* key, const char* value)
{
    for(unsigned int n=0; n < _index.size(); n++)
    {
        if (strcasecmp(key, _index[n].key) == 0)
        {
            *_index[n].ptr = atoi(value);
            return true;
        }
    }
    if (strcasecmp(key, "startCode") == 0)
    {
        this->startCode = value;
        return true;
    }
    if (strcasecmp(key, "endCode") == 0)
    {
        this->endCode = value;
        return true;
    }
    return false;
}

bool ConfigSettings::readSettings(void) {
    return readSettings(DEFAULT_CONFIG_PATH);
}

bool ConfigSettings::readSettings(const char* path) {
    std::ifstream config(path);
    std::string line;
    size_t line_number = 0;

    if(!config.good()) return false;

    while(config.good()) {
        size_t pos = std::string::npos;
        std::getline(config, line);
        line_number += 1;

        // De-comment and trim, skipping anything that shows up empty
        pos = line.find_first_of('#');
        if(pos != std::string::npos) line.erase(pos);
        TRIM_STRING(line);
        if(line.length() == 0) continue;

        // Split into key = val
        std::string key(""), val("");
        pos = line.find_first_of('=');
        if(pos != std::string::npos && line.length() > (pos + 1))
        {
            key = line.substr(0, pos);
            val = line.substr(pos + 1);
            TRIM_STRING(key);
            TRIM_STRING(val);
        }
        // Are we about to read a multiline string?
        if(val == CONFIG_MULTILINE_SEPARATOR) {
            val = "";
            bool done_multiline = false;

            while(config.good() && !done_multiline) {
                std::getline(config, line);
                line_number += 1;

                // We RTRIM the line for two reasons:
                //
                // 1) Make sure that a direct == comparison with '"""' works without
                //    worrying about trailing space.
                // 2) Nobody likes trailing whitespace anyway
                RTRIM_STRING(line);

                // Either accumuliate or terminate
                if(line == CONFIG_MULTILINE_SEPARATOR) {
                    done_multiline = true;
                    // Make sure we don't add an extra trailing newline
                    // to the parsed value
                    RTRIM_STRING(val);
                }
                else {
                    line += "\n";
                    val += line;
                }
            }

            // If we drop out but didn't finish reading, something failed
            if(!done_multiline) {
                logError("Config(%s):L%zd: Failed while reading multiline string.\n", path, line_number);
                return false;
            }

        }

        // Fail if we don't get a key and val
        if(key.length() == 0 || val.length() == 0) {
            logError("Config(%s): Line %zd: No key value pair found\n", path, line_number);
            return false;
        }

        // Set a config setting for the current K=V
        if(!setSetting(key.c_str(), val.c_str())) {
            logError("Config(%s):L%zd: Failed to set '%s' to '%s'\n", path, line_number, key.c_str(), val.c_str());
            return false;
        }
    }

    return true;
}
