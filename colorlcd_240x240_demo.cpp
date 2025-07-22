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

int main() {
  st7789.set_backlight(255);

  gpio_init(BUTTON_PIN);
  gpio_set_dir(BUTTON_PIN, GPIO_IN);
  gpio_pull_up(BUTTON_PIN); // Use pull-up if button connects to GND

  bool last_button = true;

  Pen WHITE = graphics.create_pen(255, 255, 255);

  initStars();

  while(true) {

    // Simple button debounce
    bool button = gpio_get(BUTTON_PIN);
    if(!button && last_button) { // Button pressed
      sleep_ms(200);    // Debounce delay
      if(++object>MAX_OBJ) object=0;
    }
    last_button = button;

    switch(object) {
      case 0:
        numVerts  = numVertsCubeQ;
        verts     = (int16_t*)vertsCubeQ;
        numPolys  = numQuadsCubeQ;
        polys     = (uint8_t*)quadsCubeQ;
        polyColors = (uint16_t*)colsCubeQ;
        lightShade = 0;
        bfCull    = 1;
        orient    = 0;
        polyMode  = 0;
        break;
      case 1:
        numVerts  = numVertsCubeQ;
        verts     = (int16_t*)vertsCubeQ;
        numPolys  = numQuadsCubeQ;
        polys     = (uint8_t*)quadsCubeQ;
        lightShade = 44000;
        bfCull    = 1;
        orient    = 0;
        polyMode  = 0;
        break;
    case 2:
        numVerts  = numVertsCross;
        verts     = (int16_t*)vertsCross;
        numPolys  = numQuadsCross;
        polys     = (uint8_t*)quadsCross;
        polyColors = (uint16_t*)colsCross;
        lightShade = 0;
        bfCull    = 1;
        orient    = 0;
        polyMode  = 0;
        break;
    case 3:
        numVerts  = numVertsCross;
        verts     = (int16_t*)vertsCross;
        numPolys  = numQuadsCross;
        polys     = (uint8_t*)quadsCross;
        lightShade = 14000;
        bfCull    = 1;
        orient    = 0;
        polyMode  = 0;
        break;
    case 4:
        numVerts  = numVerts3;
        verts     = (int16_t*)verts3;
        numPolys  = numQuads3;
        polys     = (uint8_t*)quads3;
        polyColors = (uint16_t*)cols3;
        lightShade = 0;
        bfCull    = 1;
        orient    = 0;
        polyMode  = 0;
        break;
    case 5:
        numVerts  = numVerts3;
        verts     = (int16_t*)verts3;
        numPolys  = numQuads3;
        polys     = (uint8_t*)quads3;
        lightShade = 20000;
        bfCull    = 1;
        orient    = 0;
        polyMode  = 0;
        break;
    case 6:
        numVerts  = numVertsCubes;
        verts     = (int16_t*)vertsCubes;
        numPolys  = numQuadsCubes;
        polys     = (uint8_t*)quadsCubes;
        polyColors = (uint16_t*)colsCubes;
        lightShade = 0;
        bfCull    = 0;
        orient    = 0;
        polyMode  = 0;
        break;
    case 7:
        numVerts  = numVertsCubes;
        verts     = (int16_t*)vertsCubes;
        numPolys  = numQuadsCubes;
        polys     = (uint8_t*)quadsCubes;
        bfCull    = 1;
        orient    = 0;
        lightShade = 14000;
        polyMode  = 0;
        break;
    case 8:
        numVerts  = numVertsCone;
        verts     = (int16_t*)vertsCone;
        numPolys  = numTrisCone;
        polys     = (uint8_t*)trisCone;
        polyColors = (uint16_t*)colsCone;
        lightShade = 0;
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
        lightShade = 0;
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

    render3D(polyMode, frBuf);

    graphics.set_pen(WHITE);
    graphics.text("Hello World - " + std::to_string(object), Point(0, 0), 240);

    // update screen
    st7789.update(&graphics);
  }

    return 0;
}
