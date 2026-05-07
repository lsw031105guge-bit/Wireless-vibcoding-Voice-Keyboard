#include <Arduino.h>
#include <U8g2lib.h>

static constexpr uint8_t kOledSclPin = D5;
static constexpr uint8_t kOledSdaPin = D4;

static constexpr int kOledX0 = 31;
static constexpr int kOledY0 = 16;
static constexpr int kOledW = 64;
static constexpr int kOledH = 48;

static U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, kOledSclPin, kOledSdaPin, U8X8_PIN_NONE);

static inline int16_t ax(int16_t x) { return (int16_t)(kOledX0 + x); }
static inline int16_t ay(int16_t y) { return (int16_t)(kOledY0 + y); }

static void drawCheckIcon(int16_t x, int16_t y) {
  u8g2.drawFrame(ax(x), ay(y), 10, 10);
  u8g2.drawLine(ax(x + 2), ay(y + 5), ax(x + 4), ay(y + 7));
  u8g2.drawLine(ax(x + 4), ay(y + 7), ax(x + 8), ay(y + 2));
}

static void drawHourglassIcon(int16_t x, int16_t y) {
  u8g2.drawFrame(ax(x), ay(y), 10, 10);
  u8g2.drawLine(ax(x + 2), ay(y + 2), ax(x + 7), ay(y + 2));
  u8g2.drawLine(ax(x + 2), ay(y + 7), ax(x + 7), ay(y + 7));
  u8g2.drawLine(ax(x + 2), ay(y + 2), ax(x + 7), ay(y + 7));
  u8g2.drawLine(ax(x + 7), ay(y + 2), ax(x + 2), ay(y + 7));
}

void oled_init() {
  u8g2.begin();
}

void oled_show_receipt(uint32_t totalTokens) {
  char buf[16];
  snprintf(buf, sizeof(buf), "%lu", (unsigned long)totalTokens);

  u8g2.clearBuffer();

  drawCheckIcon(0, 0);
  u8g2.setFont(u8g2_font_6x10_tr);
  u8g2.drawStr(ax(12), ay(9), "THANKS");

  u8g2.setFont(u8g2_font_5x8_tr);
  u8g2.drawStr(ax(0), ay(22), "for using");

  drawHourglassIcon(0, 30);
  u8g2.setFont(u8g2_font_5x8_tr);
  u8g2.drawStr(ax(12), ay(36), "TOK");

  const int16_t w = u8g2.getStrWidth(buf);
  const int16_t x = (int16_t)(kOledW - w);
  u8g2.drawStr(ax(x), ay(46), buf);

  u8g2.sendBuffer();
}

static void drawWaitingScene() {
  u8g2.clearBuffer();

  const int16_t hx = 23;
  const int16_t hy = 0;
  const int16_t hw = 18;
  const int16_t hh = 18;

  u8g2.drawFrame(ax(hx), ay(hy), hw, hh);
  u8g2.drawBox(ax(hx), ay(hy), hw, 2);
  u8g2.drawBox(ax(hx), ay(hy + hh - 2), hw, 2);
  u8g2.drawBox(ax(hx), ay(hy), 2, hh);
  u8g2.drawBox(ax(hx + hw - 2), ay(hy), 2, hh);

  u8g2.drawLine(ax(hx + 3), ay(hy + 3), ax(hx + hw - 4), ay(hy + hh - 4));
  u8g2.drawLine(ax(hx + hw - 4), ay(hy + 3), ax(hx + 3), ay(hy + hh - 4));
  u8g2.drawBox(ax(hx + 7), ay(hy + 4), 4, 3);
  u8g2.drawBox(ax(hx + 7), ay(hy + hh - 7), 4, 3);

  const int16_t headX = 18;
  const int16_t headY = 20;
  const int16_t headW = 28;
  const int16_t headH = 12;
  u8g2.drawBox(ax(headX), ay(headY), headW, headH);
  u8g2.setDrawColor(0);
  u8g2.drawBox(ax(headX + 2), ay(headY + 2), headW - 4, headH - 4);
  u8g2.setDrawColor(1);

  u8g2.drawBox(ax(headX + 8), ay(headY + 5), 3, 3);
  u8g2.drawBox(ax(headX + headW - 11), ay(headY + 5), 3, 3);

  const int16_t kbX = 8;
  const int16_t kbY = 32;
  const int16_t kbW = 48;
  const int16_t kbH = 12;
  u8g2.drawBox(ax(kbX), ay(kbY), kbW, kbH);
  u8g2.setDrawColor(0);
  u8g2.drawFrame(ax(kbX + 1), ay(kbY + 1), kbW - 2, kbH - 2);

  for (int r = 0; r < 2; r++) {
    for (int c = 0; c < 10; c++) {
      const int16_t px = kbX + 4 + c * 4;
      const int16_t py = kbY + 3 + r * 4;
      u8g2.drawBox(ax(px), ay(py), 2, 2);
    }
  }

  u8g2.setDrawColor(1);

  u8g2.drawBox(ax(16), ay(44), 4, 2);
  u8g2.drawBox(ax(44), ay(44), 4, 2);

  u8g2.sendBuffer();
}

void oled_show_hollow() {
  drawWaitingScene();
}