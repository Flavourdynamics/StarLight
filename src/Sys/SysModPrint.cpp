/*
   @title     StarMod
   @file      SysModPrint.h
   @date      20240114
   @repo      https://github.com/ewowi/StarMod
   @Authors   https://github.com/ewowi/StarMod/commits/main
   @Copyright (c) 2024 Github StarMod Commit Authors
   @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
   @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
*/

#include "SysModule.h"
#include "SysModPrint.h"
#include "SysModUI.h"
#include "SysModModel.h"
#include "SysModWeb.h"

SysModPrint::SysModPrint() :SysModule("Print") {
  // print("%s %s\n", __PRETTY_FUNCTION__, name);

  Serial.begin(115200);
  delay(5000); //if (!Serial) doesn't seem to work, check with SoftHack007

  // print("%s %s %s\n",__PRETTY_FUNCTION__,name, success?"success":"failed");
};

void SysModPrint::setup() {
  SysModule::setup();

  // print("%s %s\n", __PRETTY_FUNCTION__, name);

  parentVar = ui->initModule(parentVar, name);

  ui->initSelect(parentVar, "pOut", 1, false, [](JsonObject var) { //uiFun default 1 (Serial)
    web->addResponse(var["id"], "label", "Output");
    web->addResponse(var["id"], "comment", "System log to Serial or Net print (WIP)");

    JsonArray rows = web->addResponseA(var["id"], "options");
    rows.add("No");
    rows.add("Serial");
    rows.add("UI");

    web->clientsToJson(rows, true); //ip only
  });

  ui->initTextArea(parentVar, "log", "WIP", true, [](JsonObject var) { //uiFun
    web->addResponse(var["id"], "comment", "Show the printed log");
  });

  // print("%s %s %s\n",__PRETTY_FUNCTION__,name, success?"success":"failed");
}

void SysModPrint::loop() {
  // Module::loop();
  if (!setupsDone) setupsDone = true;
}

size_t SysModPrint::print(const char * format, ...) {
  va_list args;

  va_start(args, format);

  Serial.print(strncmp(pcTaskGetTaskName(NULL), "loopTask", 8) == 0?"λ":"α"); //looptask / asyncTCP task

  for (size_t i = 0; i < strlen(format); i++) 
  {
    if (format[i] == '%') 
    {
      switch (format[i+1]) 
      {
        case 's':
          Serial.print(va_arg(args, const char *));
          break;
        case 'u':
          Serial.print(va_arg(args, unsigned int));
          break;
        case 'c':
          Serial.print(va_arg(args, int));
          break;
        case 'd':
          Serial.print(va_arg(args, int));
          break;
        case 'f':
          Serial.print(va_arg(args, double));
          break;
        case '%':
          Serial.print("%"); // in case of %%
          break;
        default:
          va_arg(args, int);
        // logFile.print(x);
          Serial.print(format[i]);
          Serial.print(format[i+1]);
      }
      i++;
    } 
    else 
    {
      Serial.print(format[i]);
    }
  }

  va_end(args);
  return 1;
}

size_t SysModPrint::println(const __FlashStringHelper * x) {
  return Serial.println(x);
}

void SysModPrint::printVar(JsonObject var) {
  char sep[3] = "";
  for (JsonPair pair: var) {
    print("%s%s: %s", sep, pair.key(), pair.value().as<String>().c_str());
    strcpy(sep, ", ");
  }
}

size_t SysModPrint::printJson(const char * text, JsonVariantConst source) {
  print("%s ", text);
  size_t size = serializeJson(source, Serial); //for the time being
  Serial.println();
  return size;
}

size_t SysModPrint::fFormat(char * buf, size_t size, const char * format, ...) {
  va_list args;
  va_start(args, format);

  size_t len = vsnprintf(buf, size, format, args);

  va_end(args);

  // Serial.printf("fFormat %s (%d)\n", buf, size);

  return len;
}

void SysModPrint::printJDocInfo(const char * text, DynamicJsonDocument source) {
  uint8_t percentage;
  if (source.capacity() == 0)
    percentage = 100;
  else
    percentage = 100 * source.memoryUsage() / source.capacity();
  print("%s  %u / %u (%u%%) (%u %u %u)\n", text, source.memoryUsage(), source.capacity(), percentage, source.size(), source.overflowed(), source.nesting());
}

void SysModPrint::printClient(const char * text, AsyncWebSocketClient * client) {
  print("%s client: %d ...%d q:%d l:%d s:%d (#:%d)\n", text, client?client->id():-1, client?client->remoteIP()[3]:-1, client->queueIsFull(), client->queueLength(), client->status(), client->server()->count());
  //status: { WS_DISCONNECTED, WS_CONNECTED, WS_DISCONNECTING }
}

