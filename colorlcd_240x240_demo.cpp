#include <math.h>
#include <vector>
#include <string>

#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "gfx3d.h"
#include "models3d.h"

using namespace pimoroni;

const int WIDTH = 240;
const int HEIGHT = 240;

#define MAX_OBJ 12
#define NLINES 240

#define SCR_WD  240
#define SCR_HT  240
#define WD_3D   240
#define HT_3D   240
#define BUTTON_PIN 15

uint16_t frBuf[SCR_WD*NLINES];

ST7789 st7789(WIDTH, HEIGHT, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT));
PicoGraphics_PenRGB565 graphics(st7789.width, st7789.height, frBuf);

int buttonState;
int prevState = true;
long btDebounce    = 30;
long btMultiClick  = 600;
long btLongClick   = 500;
long btLongerClick = 2000;
long btTime = 0, btTime2 = 0;
int clickCnt = 1;

// 0=idle, 1,2,3=click, -1,-2=longclick
int checkButton( bool state)
{
  if( state == false && prevState == true ) { btTime = millis(); prevState = state; return 0; } // button just pressed
  if( state == true && prevState == false ) { // button just released
    prevState = state;
    if( millis()-btTime >= btDebounce && millis()-btTime < btLongClick ) { 
      if( millis()-btTime2<btMultiClick ) clickCnt++; else clickCnt=1;
      btTime2 = millis();
      return clickCnt; 
    } 
  }
  if( state == false && millis()-btTime >= btLongerClick ) { prevState = state; return -2; }
  if( state == false && millis()-btTime >= btLongClick ) { prevState = state; return -1; }
  return 0;
}

int prevButtonState=0;

int handleButton(bool button)
{
  prevButtonState = buttonState;
  buttonState = checkButton(button);
  return buttonState;
}

unsigned int ms, msMin=1000, msMax=0, stats=1, optim=0; // optim=1 for ST7735, 0 for ST7789

void showStats() {
    Pen WHITE   = graphics.create_pen(255, 255, 255);
    Pen YELLOW  = graphics.create_pen(255, 255, 0);
    Pen GREEN   = graphics.create_pen(0, 255, 0);
    Pen MAGENTA = graphics.create_pen(255, 0, 255);
    Pen BLACK   = graphics.create_pen(0, 0, 0);

    char txt[30];
 
    if (ms < msMin) msMin = ms;
    if (ms > msMax) msMax = ms;

    if (optim == 0) {
        snprintf(txt, 30, "%d ms     %d fps ", ms, 1000 / ms);
        graphics.set_pen(YELLOW);
        graphics.text(txt, Point(0, SCR_HT - 32), 240);

        snprintf(txt, 30, "%d-%d ms  %d-%d fps   ", msMin, msMax, 1000 / msMax, 1000 / msMin);
        graphics.set_pen(GREEN);
        graphics.text(txt, Point(0, SCR_HT - 22), 240);

        snprintf(txt, 30, "total/vis %d / %d   ", numPolys, numVisible);
        graphics.set_pen(MAGENTA);
        graphics.text(txt, Point(0, SCR_HT - 12), 240);
    }
    else if (optim == 1) {
        optim = 2;

        graphics.set_pen(YELLOW);
        graphics.text("00 ms     00 fps", Point(0, SCR_HT - 28), 240);

        graphics.set_pen(GREEN);
        graphics.text("00-00 ms  00-00 fps", Point(0, SCR_HT - 18), 240);

        graphics.set_pen(MAGENTA);
        graphics.text("total/vis 000 / 000", Point(0, SCR_HT - 8), 240);
    }
    else {
        graphics.set_pen(YELLOW);
        snprintf(txt, 30, "%2d", ms);
        graphics.text(txt, Point(0, SCR_HT - 28), 240);

        snprintf(txt, 30, "%2d", 1000 / ms);
        graphics.text(txt, Point(60, SCR_HT - 28), 240);  // 10*6

        graphics.set_pen(GREEN);
        snprintf(txt, 30, "%2d-%2d", msMin, msMax);
        graphics.text(txt, Point(0, SCR_HT - 18), 240);

        snprintf(txt, 30, "%2d-%2d", 1000 / msMax, 1000 / msMin);
        graphics.text(txt, Point(60, SCR_HT - 18), 240);

        graphics.set_pen(MAGENTA);
        snprintf(txt, 30, "%3d", numPolys);
        graphics.text(txt, Point(60, SCR_HT - 8), 240);  // 10*6

        snprintf(txt, 30, "%3d", numVisible);
        graphics.text(txt, Point(96, SCR_HT - 8), 240);  // 16*6
    }
}

int main() {
  st7789.set_backlight(255);

  gpio_init(BUTTON_PIN);
  gpio_set_dir(BUTTON_PIN, GPIO_IN);
  gpio_pull_up(BUTTON_PIN); // Use pull-up if button connects to GND

  bool last_button = true;

  
  initStars();

  while(true) {

    bool button = gpio_get(BUTTON_PIN);
    handleButton(button);
    if(buttonState<0) {
        if(buttonState==-1 && prevButtonState>=0 && ++bgMode>4) bgMode=0;
        if(buttonState==-2 && prevButtonState==-1) {
        stats=!stats; 
        if(optim) optim=1;
        }
    } else
    if(buttonState>0) {
        if(++object>MAX_OBJ) object=0;
        msMin=1000;
        msMax=0;
    }

    polyMode = 0;
    orient = 0;
    bfCull = 1;
    lightShade = 0;
    switch(object) {
        case 0:
        numVerts  = numVertsCubeQ;
        verts     = (int16_t*)vertsCubeQ;
        numPolys  = numQuadsCubeQ;
        polys     = (uint8_t*)quadsCubeQ;
        polyColors = (uint16_t*)colsCubeQ;
        break;
        case 1:
        numVerts  = numVertsCubeQ;
        verts     = (int16_t*)vertsCubeQ;
        numPolys  = numQuadsCubeQ;
        polys     = (uint8_t*)quadsCubeQ;
        lightShade = 44000;
        break;
    case 2:
        numVerts  = numVertsCross;
        verts     = (int16_t*)vertsCross;
        numPolys  = numQuadsCross;
        polys     = (uint8_t*)quadsCross;
        polyColors = (uint16_t*)colsCross;
        break;
    case 3:
        numVerts  = numVertsCross;
        verts     = (int16_t*)vertsCross;
        numPolys  = numQuadsCross;
        polys     = (uint8_t*)quadsCross;
        lightShade = 14000;
        break;
    case 4:
        numVerts  = numVerts3;
        verts     = (int16_t*)verts3;
        numPolys  = numQuads3;
        polys     = (uint8_t*)quads3;
        polyColors = (uint16_t*)cols3;
        break;
    case 5:
        numVerts  = numVerts3;
        verts     = (int16_t*)verts3;
        numPolys  = numQuads3;
        polys     = (uint8_t*)quads3;
        lightShade = 20000;
        break;
    case 6:
        numVerts  = numVertsCubes;
        verts     = (int16_t*)vertsCubes;
        numPolys  = numQuadsCubes;
        polys     = (uint8_t*)quadsCubes;
        polyColors = (uint16_t*)colsCubes;
        bfCull    = 0;
        break;
    case 7:
        numVerts  = numVertsCubes;
        verts     = (int16_t*)vertsCubes;
        numPolys  = numQuadsCubes;
        polys     = (uint8_t*)quadsCubes;
        bfCull    = 1;
        lightShade = 14000;
        break;
    case 8:
        numVerts  = numVertsCone;
        verts     = (int16_t*)vertsCone;
        numPolys  = numTrisCone;
        polys     = (uint8_t*)trisCone;
        polyColors = (uint16_t*)colsCone;
        bfCull    = 1;
        orient    = 1;
        polyMode  = 1;
        break;
    case 9:
        numVerts  = numVertsSphere;
        verts     = (int16_t*)vertsSphere;
        numPolys  = numTrisSphere;
        polys     = (uint8_t*)trisSphere;
        //polyColors = (uint16_t*)colsSphere;
        lightShade = 58000;
        bfCull    = 1;
        orient    = 1;
        polyMode  = 1;
        break;
    case 10:
        numVerts  = numVertsTorus;
        verts     = (int16_t*)vertsTorus;
        numPolys  = numTrisTorus;
        polys     = (uint8_t*)trisTorus;
        polyColors = (uint16_t*)colsTorus;
        bfCull    = 1;
        orient    = 1;
        polyMode  = 1;
        break;
    case 11:
        numVerts  = numVertsTorus;
        verts     = (int16_t*)vertsTorus;
        numPolys  = numTrisTorus;
        polys     = (uint8_t*)trisTorus;
        lightShade = 20000;
        bfCull    = 1;
        orient    = 1;
        polyMode  = 1;
        break;
    case 12:
        numVerts  = numVertsMonkey;
        verts     = (int16_t*)vertsMonkey;
        numPolys  = numTrisMonkey;
        polys     = (uint8_t*)trisMonkey;
        //polyColors = (uint16_t*)colsMonkey;
        lightShade = 20000;
        bfCull    = 1;
        orient    = 1;
        polyMode  = 1;
        break;
    }

    ms=millis();
    render3D(polyMode, frBuf);
    ms=millis()-ms;
    if(stats) showStats();

    //graphics.set_pen(WHITE);
    //graphics.text("Hello World - " + std::to_string(object), Point(0, 0), 240);

    // update screen
    st7789.update(&graphics);

    }
    
    return 0;
}

