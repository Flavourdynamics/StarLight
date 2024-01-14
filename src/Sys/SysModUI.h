/*
   @title     StarMod
   @file      SysModUI.h
   @date      20240114
   @repo      https://github.com/ewowi/StarMod
   @Authors   https://github.com/ewowi/StarMod/commits/main
   @Copyright (c) 2024 Github StarMod Commit Authors
   @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
   @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
*/

#pragma once
#include <vector>
#include "ArduinoJson.h"
#include "SysModule.h"
#include "SysModPrint.h"

// https://stackoverflow.com/questions/59111610/how-do-you-declare-a-lambda-function-using-typedef-and-then-use-it-by-passing-to
typedef std::function<void(JsonObject)> UFun;
typedef std::function<void(JsonObject, uint8_t)> CFun;
// typedef void(*LoopFun)(JsonObject, uint8_t*); //std::function is crashing...
typedef std::function<void(JsonObject, uint8_t*)> LoopFun;

struct VarLoop {
  JsonObject var;
  LoopFun loopFun;
  size_t bufSize = 100;
  uint16_t interval = 160; //160ms default
  unsigned long lastMillis = 0;
  unsigned long counter = 0;
  unsigned long prevCounter = 0;
};

static uint8_t linearToLogarithm(JsonObject var, uint8_t value) {
  if (value == 0) return 0;

  float minp = var["min"].isNull()?var["min"]:0;
  float maxp = var["max"].isNull()?var["max"]:255;

  // The result should be between 100 an 10000000
  float minv = minp?log(minp):0;
  float maxv = log(maxp);

  // calculate adjustment factor
  float scale = (maxv-minv) / (maxp-minp);

  return round(exp(minv + scale*((float)value-minp)));
}

class SysModUI:public SysModule {

public:
  static bool stageVarChanged;// = false; //tbd: move mechanism to UserModInstances as there it will be used
  static std::vector<UFun> uFunctions; //static because of static functions setChFunAndWs, processJson...

  SysModUI();

  //serve index.htm
  void setup();

  void loop();
  void loop1s();

  JsonObject initModule(JsonObject parent, const char * id, const char * value = nullptr, bool readOnly = false, UFun uiFun = nullptr, CFun chFun = nullptr, LoopFun loopFun = nullptr) {
    return initVarAndUpdate<const char *>(parent, id, "module", value, 0, 0, readOnly, uiFun, chFun, loopFun);
  }

  JsonObject initTable(JsonObject parent, const char * id, const char * value = nullptr, bool readOnly = false, UFun uiFun = nullptr, CFun chFun = nullptr, LoopFun loopFun = nullptr) {
    return initVarAndUpdate<const char *>(parent, id, "table", value, 0, 0, readOnly, uiFun, chFun, loopFun);
  }

  JsonObject initText(JsonObject parent, const char * id, const char * value = nullptr, uint16_t max = 32, bool readOnly = false, UFun uiFun = nullptr, CFun chFun = nullptr, LoopFun loopFun = nullptr, uint8_t count = 0, CFun valueFun = nullptr) {
    return initVarAndUpdate<const char *>(parent, id, "text", value, 0, max, readOnly, uiFun, chFun, loopFun, count, valueFun);
  }

  JsonObject initPassword(JsonObject parent, const char * id, const char * value = nullptr, uint8_t max = 32, bool readOnly = false, UFun uiFun = nullptr, CFun chFun = nullptr, LoopFun loopFun = nullptr) {
    return initVarAndUpdate<const char *>(parent, id, "password", value, 0, 0, readOnly, uiFun, chFun, loopFun);
  }

  JsonObject initNumber(JsonObject parent, const char * id, int value = uint16Max, int min = 0, int max = uint16Max, bool readOnly = false, UFun uiFun = nullptr, CFun chFun = nullptr, LoopFun loopFun = nullptr, uint8_t count = 0, CFun valueFun = nullptr) {
    return initVarAndUpdate<int>(parent, id, "number", value, min, max, readOnly, uiFun, chFun, loopFun, count, valueFun);
  }

  //init a range slider, range between 0 and 255!
  JsonObject initSlider(JsonObject parent, const char * id, int value = uint16Max, int min = 0, int max = 255, bool readOnly = false, UFun uiFun = nullptr, CFun chFun = nullptr, LoopFun loopFun = nullptr) {
    return initVarAndUpdate<int>(parent, id, "range", value, min, max, readOnly, uiFun, chFun, loopFun);
  }

  JsonObject initCanvas(JsonObject parent, const char * id, int value = uint16Max, bool readOnly = false, UFun uiFun = nullptr, CFun chFun = nullptr, LoopFun loopFun = nullptr) {
    return initVarAndUpdate<int>(parent, id, "canvas", value, 0, 0, readOnly, uiFun, chFun, loopFun);
  }

  //supports 3 state value: if uint16Max it is indeterminated
  JsonObject initCheckBox(JsonObject parent, const char * id, int value = uint16Max, bool readOnly = false, UFun uiFun = nullptr, CFun chFun = nullptr, LoopFun loopFun = nullptr, uint8_t count = 0, CFun valueFun = nullptr) {
    return initVarAndUpdate<int>(parent, id, "checkbox", value, 0, 0, readOnly, uiFun, chFun, loopFun, count, valueFun);
  }

  JsonObject initButton(JsonObject parent, const char * id, const char * value = nullptr, bool readOnly = false, UFun uiFun = nullptr, CFun chFun = nullptr, LoopFun loopFun = nullptr) {
    return initVarAndUpdate<const char *>(parent, id, "button", value, 0, 0, readOnly, uiFun, chFun, loopFun);
  }

  JsonObject initSelect(JsonObject parent, const char * id, int value = uint16Max, bool readOnly = false, UFun uiFun = nullptr, CFun chFun = nullptr, LoopFun loopFun = nullptr) {
    return initVarAndUpdate<int>(parent, id, "select", value, 0, 0, readOnly, uiFun, chFun, loopFun);
  }

  JsonObject initTextArea(JsonObject parent, const char * id, const char * value = nullptr, bool readOnly = false, UFun uiFun = nullptr, CFun chFun = nullptr, LoopFun loopFun = nullptr) {
    return initVarAndUpdate<const char *>(parent, id, "textarea", value, 0, 0, readOnly, uiFun, chFun, loopFun);
  }

  JsonObject initURL(JsonObject parent, const char * id, const char * value = nullptr, bool readOnly = false, UFun uiFun = nullptr, CFun chFun = nullptr, LoopFun loopFun = nullptr) {
    return initVarAndUpdate<const char *>(parent, id, "url", value, 0, 0, readOnly, uiFun, chFun, loopFun);
  }

  template <typename Type>
  JsonObject initVarAndUpdate(JsonObject parent, const char * id, const char * type, Type value, int min, int max, bool readOnly = true, UFun uiFun = nullptr, CFun chFun = nullptr, LoopFun loopFun = nullptr, uint8_t count = 0, CFun valueFun = nullptr) {
    JsonObject var = initVar(parent, id, type, readOnly, uiFun, chFun, loopFun);
    bool isPointer = std::is_pointer<Type>::value;

    if (!valueFun) {
      //set a default if not a value yet
      if (var["value"].isNull() && (!isPointer || value)) {
        bool isChar = std::is_same<Type, const char *>::value;
        if (isChar)
          var["value"] = (char *)value; //if char make a copy !!
        else {//if (value != uint16Max)
          var["value"] = value; //if value is a pointer, it needs to have a value
          //workaround
          if (var["value"].as<int>() == uint16Max)
            var.remove("value");
        }
      }
    }
    else {
      bool runValueFun = false;
      if (var["value"].isNull()) {
        runValueFun = true;
        print->printJson("initVarAndUpdate uiFun value is null", var);
      } else if (var["value"].is<JsonArray>()) {
        if (var["value"].as<JsonArray>().size() != count) {
          print->printJson("initVarAndUpdate uiFun value array wrong size", var);
          runValueFun = true;
        }
        //else everything okay, saved array is used
      }
      else {
        print->printJson("initVarAndUpdate uiFun value not array", var);
        runValueFun = true;
      }

      USER_PRINTF("initVarAndUpdate %s count:%d b:%d\n", id, count, runValueFun);

      if (runValueFun) {
        // JsonArray value = web->addResponseA(var["id"], "value");
        for (int rowNr=0;rowNr<count;rowNr++)
          valueFun(var, rowNr);
      }
    }

    if (min) var["min"] = min;
    if (max && max != uint16Max) var["max"] = max;

    //no call of fun for buttons otherwise all buttons will be fired which is highly undesirable
    if (strcmp(type,"button") != 0 && chFun && (!isPointer || value)) { //!isPointer because 0 is also a value then
      USER_PRINTF("initVarAndUpdate chFun init %s v:%s\n", var["id"].as<const char *>(), var["value"].as<String>());
      if (var["value"].is<JsonArray>()) {
        int rowNr = 0;
        for (JsonVariant val:var["value"].as<JsonArray>()) {
          chFun(var, rowNr++);
        }
      }
      else
        chFun(var, uint8Max);
    }
    return var;
  }

  JsonObject initVar(JsonObject parent, const char * id, const char * type, bool readOnly = true, UFun uiFun = nullptr, CFun chFun = nullptr, LoopFun loopFun = nullptr);

  //run the change function and send response to all? websocket clients
  static void setChFunAndWs(JsonObject var, uint8_t rowNr = uint8Max, const char * value = nullptr);

  //interpret json and run commands or set values like deserializeJson / deserializeState / deserializeConfig
  static const char * processJson(JsonVariant &json); //static for setupJsonHandlers

  //called to rebuild selects and tables (tbd: also label and comments is done again, that is not needed)
  void processUiFun(const char * id);

private:
  static bool varLoopsChanged;// = false;

  static int varCounter; //not static crashes ??? (not called async...?)

  static std::vector<CFun> cFunctions; //static because of static functions setChFunAndWs, processJson...
  static std::vector<VarLoop> loopFunctions; //non static crashing ...

};

static SysModUI *ui;