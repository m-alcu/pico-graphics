#include <math.h>
#include <vector>

#include "drivers/st7789/st7789.hpp"
#include "libraries/pico_graphics/pico_graphics.hpp"

using namespace pimoroni;

const int WIDTH = 240;
const int HEIGHT = 240;

#define NLINES 240

#define SCR_WD  240
#define SCR_HT  240
#define WD_3D   240
#define HT_3D   240
#define BUTTON_PIN 15

uint16_t frBuf[SCR_WD*NLINES];

ST7789 st7789(WIDTH, HEIGHT, ROTATE_0, false, get_spi_pins(BG_SPI_FRONT));
PicoGraphics_PenRGB332 graphics(st7789.width, st7789.height, nullptr);

int main() {
  st7789.set_backlight(255);

  gpio_init(BUTTON_PIN);
  gpio_set_dir(BUTTON_PIN, GPIO_IN);
  gpio_pull_up(BUTTON_PIN); // Use pull-up if button connects to GND

  bool moving = true;
  bool last_button = true;

  struct pt {
    float      x;
    float      y;
    uint8_t    r;
    float     dx;
    float     dy;
    Pen      pen;
  };

  std::vector<pt> shapes;
  for(int i = 0; i < 100; i++) {
    pt shape;
    shape.r = (rand() % 10) + 3;
    shape.x = rand() % (graphics.bounds.w - (shape.r * 2));
    shape.y = rand() % (graphics.bounds.h - (shape.r * 2));
    shape.x += shape.r;
    shape.y += shape.r;
    shape.dx = float(rand() % 255) / 64.0f;
    shape.dy = float(rand() % 255) / 64.0f;
    shape.pen = graphics.create_pen(rand() % 255, rand() % 255, rand() % 255);
    shapes.push_back(shape);
  }

  Pen BG = graphics.create_pen(120, 40, 60);
  Pen WHITE = graphics.create_pen(255, 255, 255);

  while(true) {

    // Simple button debounce
    bool button = gpio_get(BUTTON_PIN);
    if(!button && last_button) { // Button pressed
      moving = !moving; // Toggle movement
      sleep_ms(200);    // Debounce delay
    }
    last_button = button;

    graphics.set_pen(BG);
    graphics.clear();

    for(auto &shape : shapes) {

      if(moving) {
        shape.x += shape.dx;
        shape.y += shape.dy;
        if(shape.x < shape.r) shape.dx *= -1;
        if(shape.x >= graphics.bounds.w - shape.r) shape.dx *= -1;
        if(shape.y < shape.r) shape.dy *= -1;
        if(shape.y >= graphics.bounds.h - shape.r) shape.dy *= -1;
      }
      graphics.set_pen(shape.pen);
      graphics.circle(Point(shape.x, shape.y), shape.r);
    }

    graphics.set_pen(WHITE);
    graphics.text("Hello World", Point(0, 0), 240);

    // update screen
    st7789.update(&graphics);
  }

    return 0;
}
