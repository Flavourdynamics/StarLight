/*
   @title     StarLight
   @file      LedProjections.h
   @date      20240228
   @repo      https://github.com/MoonModules/StarLight
   @Authors   https://github.com/MoonModules/StarLight/commits/main
   @Copyright © 2024 Github StarLight Commit Authors
   @license   GNU GENERAL PUBLIC LICENSE Version 3, 29 June 2007
   @license   For non GPL-v3 usage, commercial licenses must be purchased. Contact moonmodules@icloud.com
*/

//should not contain variables/bytes to keep mem as small as possible!!

class NoneProjection: public Projection {
  const char * name() {return "None";}
  //uint8_t dim() {return _1D;} // every projection should work for all D
  const char * tags() {return "💫";}

  void controls(Leds &leds, JsonObject parentVar) {
  }
}; //NoneProjection


class DefaultProjection: public Projection {
  const char * name() {return "Default";}
  //uint8_t dim() {return _1D;} // every projection should work for all D
  const char * tags() {return "💫";}

}; //DefaultProjection

class MultiplyProjection: public Projection {
  const char * name() {return "Multiply";}
  const char * tags() {return "💫";}

  public:

  void adjustSizeAndPixel(Leds &leds, Coord3D &sizeAdjusted, Coord3D &pixelAdjusted, Coord3D &midPosAdjusted) {
    // UI Variables
    leds.projectionData.begin();
    Coord3D proMulti = leds.projectionData.read<Coord3D>();
    bool    mirror   = leds.projectionData.read<bool>();

    proMulti = proMulti.maximum(Coord3D{1, 1, 1}); // {1, 1, 1} is the minimum value
    if (proMulti == Coord3D{1, 1, 1}) return;      // No need to adjust if proMulti is {1, 1, 1}
    
    sizeAdjusted = (sizeAdjusted + proMulti - Coord3D({1,1,1})) / proMulti; // Round up
    midPosAdjusted /= proMulti;

    if (mirror) {
      Coord3D mirrors = pixelAdjusted / sizeAdjusted; // Place the pixel in the right quadrant
      pixelAdjusted = pixelAdjusted % sizeAdjusted;
      if (mirrors.x %2 != 0) pixelAdjusted.x = sizeAdjusted.x - 1 - pixelAdjusted.x;
      if (mirrors.y %2 != 0) pixelAdjusted.y = sizeAdjusted.y - 1 - pixelAdjusted.y;
      if (mirrors.z %2 != 0) pixelAdjusted.z = sizeAdjusted.z - 1 - pixelAdjusted.z;
    }
    else pixelAdjusted = pixelAdjusted % sizeAdjusted;
  }

  void controls(Leds &leds, JsonObject parentVar) {
    leds.projectionData.reset();
    Coord3D *proMulti = leds.projectionData.write<Coord3D>({2,2,1});
    bool *mirror = leds.projectionData.write<bool>(false);
    ui->initCoord3D(parentVar, "proMulti", proMulti, 0, 10, false, [&leds](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
      case onChange:
        if (rowNr < leds.fixture->listOfLeds.size()) {
          leds.fixture->listOfLeds[rowNr]->triggerMapping();
        }
        return true;
      default: return false;
    }});
    ui->initCheckBox(parentVar, "mirror", mirror, false, [&leds](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
      case onChange:
        if (rowNr < leds.fixture->listOfLeds.size()) {
          leds.fixture->listOfLeds[rowNr]->triggerMapping();
        }
        return true;
      default: return false;
    }});
  }
}; //MultiplyProjection

class TiltPanRollProjection: public Projection {
  const char * name() {return "TiltPanRoll";}
  //uint8_t dim() {return _1D;} // every projection should work for all D
  const char * tags() {return "💫";}

  public: //to use in Preset1Projection

  void adjustXYZ(Leds &leds, Coord3D &pixel) {
    #ifdef STARBASE_USERMOD_MPU6050
      if (leds.proGyro) {
        pixel = trigoTiltPanRoll.tilt(pixel, leds.size/2, mpu6050->gyro.x);
        pixel = trigoTiltPanRoll.pan(pixel, leds.size/2, mpu6050->gyro.y);
        pixel = trigoTiltPanRoll.roll(pixel, leds.size/2, mpu6050->gyro.z);
      }
      else 
    #endif
    {
      if (leds.proTiltSpeed) pixel = trigoTiltPanRoll.tilt(pixel, leds.size/2, sys->now * 5 / (255 - leds.proTiltSpeed));
      if (leds.proPanSpeed) pixel = trigoTiltPanRoll.pan(pixel, leds.size/2, sys->now * 5 / (255 - leds.proPanSpeed));
      if (leds.proRollSpeed) pixel = trigoTiltPanRoll.roll(pixel, leds.size/2, sys->now * 5 / (255 - leds.proRollSpeed));
      if (leds.fixture->fixSize.z == 1) pixel.z = 0; // 3d effects will be flattened on 2D fixtures
    }
  }

  void controls(Leds &leds, JsonObject parentVar) {
    //tbd: implement variable by reference for rowNrs
    #ifdef STARBASE_USERMOD_MPU6050
      ui->initCheckBox(parentVar, "proGyro", false, false, [&leds](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
        case onChange:
          if (rowNr < leds.fixture->listOfLeds.size())
            leds.fixture->listOfLeds[rowNr]->proGyro = mdl->getValue(var, rowNr);
          return true;
        default: return false;
      }});
    #endif
    ui->initSlider(parentVar, "proTilt", 128, 0, 254, false, [&leds](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
      case onChange:
        if (rowNr < leds.fixture->listOfLeds.size())
          leds.fixture->listOfLeds[rowNr]->proTiltSpeed = mdl->getValue(var, rowNr);
        return true;
      default: return false;
    }});
    ui->initSlider(parentVar, "proPan", 128, 0, 254, false, [&leds](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
      case onChange:
        if (rowNr < leds.fixture->listOfLeds.size())
          leds.fixture->listOfLeds[rowNr]->proPanSpeed = mdl->getValue(var, rowNr);
        return true;
      default: return false;
    }});
    ui->initSlider(parentVar, "proRoll", 128, 0, 254, false, [&leds](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
      case onUI:
        ui->setLabel(var, "Roll speed");
        return true;
      case onChange:
        if (rowNr < leds.fixture->listOfLeds.size())
          leds.fixture->listOfLeds[rowNr]->proRollSpeed = mdl->getValue(var, rowNr);
        return true;
      default: return false;
    }});
  }
}; //TiltPanRollProjection

class DistanceFromPointProjection: public Projection {
  const char * name() {return "Distance ⌛";}
  //uint8_t dim() {return _1D;} // every projection should work for all D
  const char * tags() {return "💫";}

  public: //to use in Preset1Projection

  void postProcessing(Leds &leds, uint16_t &indexV) {
    if (leds.projectionDimension == _2D) { //2D2D: inverse mapping
      Trigo trigo(leds.size.x-1); // 8 bits trigo with period leds.size.x-1 (currentl Float trigo as same performance)
      float minDistance = 10;
      // ppf("checking indexV %d\n", indexV);
      for (forUnsigned16 x=0; x<leds.size.x && minDistance > 0.5f; x++) {
        // float xFactor = x * TWO_PI / (float)(leds.size.x-1); //between 0 .. 2PI

        float xNew = trigo.sin(leds.size.x, x);
        float yNew = trigo.cos(leds.size.y, x);

        for (forUnsigned16 y=0; y<leds.size.y && minDistance > 0.5f; y++) {

          // float yFactor = (leds.size.y-1.0f-y) / (leds.size.y-1.0f); // between 1 .. 0
          float yFactor = 1 - y / (leds.size.y-1.0f); // between 1 .. 0

          float x2New = round((yFactor * xNew + leds.size.x) / 2.0f); // 0 .. size.x
          float y2New = round((yFactor * yNew + leds.size.y) / 2.0f); //  0 .. size.y

          // ppf(" %d,%d->%f,%f->%f,%f", x, y, sinf(x * TWO_PI / (float)(size.x-1)), cosf(x * TWO_PI / (float)(size.x-1)), xNew, yNew);

          //this should work (better) but needs more testing
          // float distance = abs(indexV - xNew - yNew * size.x);
          // if (distance < minDistance) {
          //   minDistance = distance;
          //   indexV = x+y*size.x;
          // }

          // if the new XY i
          if (indexV == leds.XY(x2New, y2New)) { //(unsigned8)xNew + (unsigned8)yNew * size.x) {
            // ppf("  found one %d => %d=%d+%d*%d (%f+%f*%d) [%f]\n", indexV, x+y*size.x, x,y, size.x, xNew, yNew, size.x, distance);
            indexV = leds.XY(x, y);

            if (indexV%10 == 0) ppf("."); //show some progress as this projection is slow (Need S007 to optimize ;-)
                                        
            minDistance = 0.0f; // stop looking further
          }
        }
      }
      if (minDistance > 0.5f) indexV = UINT16_MAX;
    }
  }

  void controls(Leds &leds, JsonObject parentVar) {
    // ui->initCoord3D(parentVar, "proCenter", {8,8,8}, 0, NUM_LEDS_Max, false, [&leds](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
    //   case onUI:
    //     ui->setLabel(var, "Center");
    //     return true;
    //   case onChange:
    //     //initiate projectAndMap
    //     ppf("proCenter %d %d\n", rowNr, leds.fixture->listOfLeds.size());
    //     if (rowNr < leds.fixture->listOfLeds.size()) {
    //       leds.fixture->listOfLeds[rowNr]->triggerMapping(); //Guru Meditation Error: Core  1 panic'ed (StoreProhibited). Exception was unhandled.
    //     }
    //     // ui->setLabel(var, "Size");
    //     return true;
    //   default: return false;
    // }});
  }
}; //DistanceFromPointProjection

class Preset1Projection: public Projection {
  const char * name() {return "Preset1";}
  //uint8_t dim() {return _1D;} // every projection should work for all D
  const char * tags() {return "💫";}

  void adjustSizeAndPixel(Leds &leds, Coord3D &sizeAdjusted, Coord3D &pixelAdjusted, Coord3D &midPosAdjusted) {
    MultiplyProjection mp;
    mp.adjustSizeAndPixel(leds, sizeAdjusted, pixelAdjusted, midPosAdjusted);
  }

  void adjustXYZ(Leds &leds, Coord3D &pixel) {
    TiltPanRollProjection tp;
    tp.adjustXYZ(leds, pixel);
  }

  void controls(Leds &leds, JsonObject parentVar) {
    DistanceFromPointProjection dp;
    dp.controls(leds, parentVar);
    MultiplyProjection mp;
    mp.controls(leds, parentVar);
    TiltPanRollProjection tp;
    tp.controls(leds, parentVar);
  }
}; //Preset1Projection

class RandomProjection: public Projection {
  const char * name() {return "Random";}
  //uint8_t dim() {return _1D;} // every projection should work for all D
  const char * tags() {return "💫";}

  void controls(Leds &leds, JsonObject parentVar) {
  }
}; //RandomProjection

class ReverseProjection: public Projection {
  const char * name() {return "Reverse";}
  const char * tags() {return "💡";}

  public:

  void adjustSizeAndPixel(Leds &leds, Coord3D &sizeAdjusted, Coord3D &pixelAdjusted, Coord3D &midPosAdjusted) {
    // UI Variables
    leds.projectionData.begin();
    bool reverseX = leds.projectionData.read<bool>();
    bool reverseY = leds.projectionData.read<bool>();
    bool reverseZ = leds.projectionData.read<bool>();

    if (reverseX) pixelAdjusted.x = sizeAdjusted.x - pixelAdjusted.x - 1;
    if (reverseY) pixelAdjusted.y = sizeAdjusted.y - pixelAdjusted.y - 1;
    if (reverseZ) pixelAdjusted.z = sizeAdjusted.z - pixelAdjusted.z - 1;
  }

  void controls(Leds &leds, JsonObject parentVar) {
    leds.projectionData.reset();
    bool *reverseX = leds.projectionData.write<bool>(false);
    bool *reverseY = leds.projectionData.write<bool>(false);
    bool *reverseZ = leds.projectionData.write<bool>(false);

    ui->initCheckBox(parentVar, "reverse X", reverseX, false, [&leds](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
      case onChange:
        leds.fixture->listOfLeds[rowNr]->triggerMapping();
        return true;
      default: return false;
    }});
    if (leds.effectDimension >= _2D) {
      ui->initCheckBox(parentVar, "reverse Y", reverseY, false, [&leds](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
        case onChange:
          leds.fixture->listOfLeds[rowNr]->triggerMapping();
          return true;
        default: return false;
      }});
    }
    if (leds.effectDimension == _3D) {
      ui->initCheckBox(parentVar, "reverse Z", reverseZ, false, [&leds](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
        case onChange:
          leds.fixture->listOfLeds[rowNr]->triggerMapping();
          return true;
        default: return false;
      }});
    }
  }
}; //ReverseProjection

class MirrorProjection: public Projection {
  const char * name() {return "Mirror";}
  const char * tags() {return "💡";}

  public:

  void adjustSizeAndPixel(Leds &leds, Coord3D &sizeAdjusted, Coord3D &pixelAdjusted, Coord3D &midPosAdjusted) {
    // UI Variables
    leds.projectionData.begin();
    bool mirrorX = leds.projectionData.read<bool>();
    bool mirrorY = leds.projectionData.read<bool>();
    bool mirrorZ = leds.projectionData.read<bool>();

    if (mirrorX) {
      if (pixelAdjusted.x >= sizeAdjusted.x / 2) pixelAdjusted.x = sizeAdjusted.x - 1 - pixelAdjusted.x;
      sizeAdjusted.x = (sizeAdjusted.x + 1) / 2;
    }
    if (mirrorY) {
      if (pixelAdjusted.y >= sizeAdjusted.y / 2) pixelAdjusted.y = sizeAdjusted.y - 1 - pixelAdjusted.y;
      sizeAdjusted.y = (sizeAdjusted.y + 1) / 2;
    }
    if (mirrorZ) {
      if (pixelAdjusted.z >= sizeAdjusted.z / 2) pixelAdjusted.z = sizeAdjusted.z - 1 - pixelAdjusted.z;
      sizeAdjusted.z = (sizeAdjusted.z + 1) / 2;
    }
}

  void controls(Leds &leds, JsonObject parentVar) {
    leds.projectionData.reset();
    bool *mirrorX = leds.projectionData.write<bool>(false);
    bool *mirrorY = leds.projectionData.write<bool>(false);
    bool *mirrorZ = leds.projectionData.write<bool>(false);
    ui->initCheckBox(parentVar, "mirror X", mirrorX, false, [&leds](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
      case onChange:
        leds.fixture->listOfLeds[rowNr]->triggerMapping();
        return true;
      default: return false;
    }});
    if (leds.projectionDimension >= _2D) {
      ui->initCheckBox(parentVar, "mirror Y", mirrorY, false, [&leds](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
        case onChange:
          leds.fixture->listOfLeds[rowNr]->triggerMapping();
          return true;
        default: return false;
      }});
    }
    if (leds.projectionDimension == _3D) {
      ui->initCheckBox(parentVar, "mirror Z", mirrorZ, false, [&leds](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
        case onChange:
          leds.fixture->listOfLeds[rowNr]->triggerMapping();
          return true;
        default: return false;
      }});
    }
  }
}; //MirrorProjection

class GroupingProjection: public Projection {
  const char * name() {return "Grouping";}
  const char * tags() {return "💡";}

  public:

  void adjustSizeAndPixel(Leds &leds, Coord3D &sizeAdjusted, Coord3D &pixelAdjusted, Coord3D &midPosAdjusted) {
    // UI Variables
    leds.projectionData.begin();
    Coord3D grouping = leds.projectionData.read<Coord3D>();
    grouping = grouping.maximum(Coord3D{1, 1, 1}); // {1, 1, 1} is the minimum value
    if (grouping == Coord3D{1, 1, 1}) return;

    midPosAdjusted /= grouping;
    pixelAdjusted /= grouping;

    sizeAdjusted = (sizeAdjusted + grouping - Coord3D{1,1,1}) / grouping; // round up
  }

  void controls(Leds &leds, JsonObject parentVar) {
    leds.projectionData.reset();
    Coord3D *grouping = leds.projectionData.write<Coord3D>({1,1,1});
    ui->initCoord3D(parentVar, "Grouping", grouping, 0, 100, false, [&leds](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
      case onChange:
        leds.fixture->listOfLeds[rowNr]->triggerMapping();
        return true;
      default: return false;
    }});
  }
}; //GroupingProjection

class SpacingProjection: public Projection {
  const char * name() {return "Spacing WIP";}
  //uint8_t dim() {return _1D;} // every projection should work for all D
  const char * tags() {return "💡";}

  void controls(Leds &leds, JsonObject parentVar) {
  }
}; //SpacingProjection

class TransposeProjection: public Projection {
  const char * name() {return "Transpose";}
  const char * tags() {return "💡";}

  public:

  void adjustSizeAndPixel(Leds &leds, Coord3D &sizeAdjusted, Coord3D &pixelAdjusted, Coord3D &midPosAdjusted) {
    // UI Variables
    leds.projectionData.begin();
    bool transposeXY = leds.projectionData.read<bool>();
    bool transposeXZ = leds.projectionData.read<bool>();
    bool transposeYZ = leds.projectionData.read<bool>();

    if (transposeXY) { int temp = pixelAdjusted.x; pixelAdjusted.x = pixelAdjusted.y; pixelAdjusted.y = temp; }
    if (transposeXZ) { int temp = pixelAdjusted.x; pixelAdjusted.x = pixelAdjusted.z; pixelAdjusted.z = temp; }
    if (transposeYZ) { int temp = pixelAdjusted.y; pixelAdjusted.y = pixelAdjusted.z; pixelAdjusted.z = temp; }
  }

  void controls(Leds &leds, JsonObject parentVar) {
    leds.projectionData.reset();
    bool *transposeXY = leds.projectionData.write<bool>(false);
    bool *transposeXZ = leds.projectionData.write<bool>(false);
    bool *transposeYZ = leds.projectionData.write<bool>(false);

    ui->initCheckBox(parentVar, "transpose XY", transposeXY, false, [&leds](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
      case onChange:
        leds.fixture->listOfLeds[rowNr]->triggerMapping();
        return true;
      default: return false;
    }});
    if (leds.effectDimension == _3D) {
      ui->initCheckBox(parentVar, "transpose XZ", transposeXZ, false, [&leds](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
        case onChange:
          leds.fixture->listOfLeds[rowNr]->triggerMapping();
          return true;
        default: return false;
      }});
      ui->initCheckBox(parentVar, "transpose YZ", transposeYZ, false, [&leds](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
        case onChange:
          leds.fixture->listOfLeds[rowNr]->triggerMapping();
          return true;
        default: return false;
      }});
    }
  }
}; //TransposeProjection

class KaleidoscopeProjection: public Projection {
  const char * name() {return "Kaleidoscope WIP";}
  //uint8_t dim() {return _1D;} // every projection should work for all D
  const char * tags() {return "💫";}

  void controls(Leds &leds, JsonObject parentVar) {
  }
}; //KaleidoscopeProjection

class PinwheelProjection: public Projection {
  // Currently 1D to 2D/3D May be possible to make 2D to 2D/3D
  const char * name() {return "Pinwheel";}
  const char * tags() {return "💫";}

  public:

  void adjustSizeAndPixel(Leds &leds, Coord3D &sizeAdjusted, Coord3D &pixelAdjusted, Coord3D &midPosAdjusted) {
    if (leds.size != Coord3D{0,0,0}) return; // Adjust only on first call
    leds.projectionData.begin();
    int petals = leds.projectionData.read<uint16_t>();
    petals = max(1, petals); // Petals cannot be < 1
    if (leds.projectionDimension > _1D && leds.effectDimension > _1D) {
      sizeAdjusted.y = sqrt(sq(max(sizeAdjusted.x - midPosAdjusted.x, midPosAdjusted.x)) + 
                            sq(max(sizeAdjusted.y - midPosAdjusted.y, midPosAdjusted.y))) + 1; // Adjust y before x
      sizeAdjusted.x = min(72, petals);
      sizeAdjusted.z = 1;
    }
    else {
      sizeAdjusted.x = petals;
      sizeAdjusted.y = 1;
      sizeAdjusted.z = 1;
    }
  }

  void adjustMapped(Leds &leds, Coord3D &mapped, Coord3D sizeAdjusted, Coord3D pixelAdjusted, Coord3D midPosAdjusted) {
    // UI Variables
    leds.projectionData.begin();
    int petals     = leds.projectionData.read<uint16_t>();
    const int swirlVal   = leds.projectionData.read<uint8_t>() - 30; // SwirlVal range -30 to 30
    const bool reverse   = leds.projectionData.read<bool>();
    const int angleRange = max(int(leds.projectionData.read<uint16_t>()), 1);
    const int zTwist     = leds.projectionData.read<uint8_t>() - 42; // zTwist range -42 to 42

    if (leds.effectDimension > _1D && leds.projectionDimension > _1D) petals = min(72, petals); // Limit 2D/3D grid.x to 72
    petals = max(1, petals); // Petals cannot be < 1
         
    const int dx = pixelAdjusted.x - midPosAdjusted.x;
    const int dy = pixelAdjusted.y - midPosAdjusted.y;
    const int swirlFactor = swirlVal == 0 ? 0 : hypot(dy, dx) * abs(swirlVal); // Only calculate if swirlVal != 0
    int angle       = degrees(atan2(dy, dx)) + 180;  // 0 - 360
    
    if (swirlVal < 0) angle = 360 - angle; // Reverse Swirl

    int value = angle + swirlFactor + (zTwist * pixelAdjusted.z);
    float petalWidth = angleRange / float(petals);
    value /= petalWidth;
    value %= petals;

    if (reverse) value = petals - value - 1; // Reverse Movement

    mapped.x = value;
    mapped.y = 0;
    if (leds.effectDimension > _1D && leds.projectionDimension > _1D) {
      mapped.y = int(sqrt(sq(dx) + sq(dy))); // Round produced blank pixel
    }
    mapped.z = 0;

    // if (pixelAdjusted.x == 0 && pixelAdjusted.y == 0 && pixelAdjusted.z == 0) ppf("Pinwheel  Center: (%d, %d) SwirlVal: %d angleRange: %d Petals: %d zTwist: %d\n", midPosAdjusted.x, midPosAdjusted.y, swirlVal, angleRange, petals, zTwist);
    // ppf("pixelAdjusted %d,%d,%d -> %d,%d,%d angle %d\n", pixelAdjusted.x, pixelAdjusted.y, pixelAdjusted.z, mapped.x, mapped.y, mapped.z, angle);
  }
  void controls(Leds &leds, JsonObject parentVar) {
    leds.projectionData.reset();

    uint16_t *petals     = leds.projectionData.write<uint16_t>(60); // Initalize petal first for adjustSizeAndPixel
    uint8_t  *swirlVal   = leds.projectionData.write<uint8_t>(30);
    bool     *reverse    = leds.projectionData.write<bool>(false);
    uint16_t *angleRange = leds.projectionData.write<uint16_t>(360);
    uint8_t  *zTwist     = leds.projectionData.write<uint8_t>(42);

    ui->initSlider(parentVar, "swirl", swirlVal, 0, 60, false, [&leds](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
      case onChange:
        leds.fixture->listOfLeds[rowNr]->triggerMapping();
        return true;
      default: return false;
    }});
    ui->initCheckBox(parentVar, "reverse", reverse, false, [&leds](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
      case onChange:
        leds.fixture->listOfLeds[rowNr]->triggerMapping();
        return true;
      default: return false;
    }});
    // Testing zTwist range -42 to 42 arbitrary values for testing. Hide if not 3D fixture. Select pinwheel while using 3D fixture.
    if (leds.projectionDimension == _3D) {
      ui->initSlider(parentVar, "zTwist", zTwist, 0, 82, false, [&leds](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
        case onChange:
          leds.fixture->listOfLeds[rowNr]->triggerMapping();
          return true;
        default: return false;
      }});
    }
    // Angle range 0 - angleRange. For testing purposes
    ui->initNumber(parentVar, "angleRange", angleRange, 1, 720, false, [&leds](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
      case onChange:
        leds.fixture->listOfLeds[rowNr]->triggerMapping();
        return true;
      default: return false;
    }});
    // Naming petals, arms, blades, rays? Controls virtual strip length.
    ui->initNumber(parentVar, "petals", petals, 1, 360, false, [&leds](JsonObject var, unsigned8 rowNr, unsigned8 funType) { switch (funType) { //varFun
      case onChange:
        leds.fixture->listOfLeds[rowNr]->triggerMapping();
        return true;
      default: return false;
    }});
  }
}; //PinwheelProjection

class TestProjection: public Projection {
  const char * name() {return "Test";}
  //uint8_t dim() {return _1D;} // every projection should work for all D
  const char * tags() {return "💡";}

  void controls(Leds &leds, JsonObject parentVar) {
  }
}; //TestProjection