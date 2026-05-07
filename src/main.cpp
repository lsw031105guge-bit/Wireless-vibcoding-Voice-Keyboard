#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <bluefruit.h>

#include "oled_display.h"

static constexpr uint32_t kBaud = 115200;

// EN: MacroPad Key Map (4 keys)
// EN: Key1 "Submit"  : Enter
// EN: Key2 "Accept"  : Enter, then Cmd+Enter
// EN: Key3 "Dictation": Hold-to-talk (Ctrl+Shift+Space configured in macOS)
// EN: Key4 "Abort"   : Esc, then Cmd+., then Esc
//
// 中文：四键宏键盘映射
// 按键1「提交」：回车 Enter
// 按键2「接受」：回车 Enter，然后 Command+Enter
// 按键3「听写」：按住说话（需在 macOS 把“听写”快捷键设置为 Ctrl+Shift+Space）
// 按键4「中止」：Esc，然后 Command+.，然后 Esc
//
// EN: Wiring (S1=Button, S2=NeoPixel)
// EN: Key1 S1->D0, S2->D1
// EN: Key2 S1->D8, S2->D9
// EN: Key3 S1->D2, S2->D3
// EN: Key4 S1->D7, S2->D6
//
// 中文：接线（S1=按键输出，S2=NeoPixel 灯控）
// 按键1 S1->D0，S2->D1
// 按键2 S1->D8，S2->D9
// 按键3 S1->D2，S2->D3
// 按键4 S1->D7，S2->D6

#define MACROPAD_KEY1_BTN_PIN D0
#define MACROPAD_KEY1_LED_PIN D1
#define MACROPAD_KEY2_BTN_PIN D8
#define MACROPAD_KEY2_LED_PIN D9
#define MACROPAD_KEY3_BTN_PIN D2
#define MACROPAD_KEY3_LED_PIN D3
#define MACROPAD_KEY4_BTN_PIN D7
#define MACROPAD_KEY4_LED_PIN D6

#define MACROPAD_DICTATION_MODIFIER (KEYBOARD_MODIFIER_LEFTCTRL | KEYBOARD_MODIFIER_LEFTSHIFT)
#define MACROPAD_DICTATION_KEYCODE 0x2C

#define MACROPAD_ACCEPT_STEP1_MOD 0
#define MACROPAD_ACCEPT_STEP1_KEY 0x28
#define MACROPAD_ACCEPT_STEP2_MOD KEYBOARD_MODIFIER_LEFTGUI
#define MACROPAD_ACCEPT_STEP2_KEY 0x28

#define MACROPAD_ABORT_STEP1_MOD 0
#define MACROPAD_ABORT_STEP1_KEY 0x29
#define MACROPAD_ABORT_STEP2_MOD KEYBOARD_MODIFIER_LEFTGUI
#define MACROPAD_ABORT_STEP2_KEY 0x37
#define MACROPAD_ABORT_STEP3_MOD 0
#define MACROPAD_ABORT_STEP3_KEY 0x29

static constexpr uint8_t kButtonPin1 = MACROPAD_KEY1_BTN_PIN;
// EN: If your button module outputs LOW when pressed, set this to true.
// 中文：如果你的按键模块“按下输出低电平(LOW)”，这里设为 true。
static constexpr bool kButtonActiveLow = false;
static constexpr uint32_t kDebounceMs = 25;

// EN: Key #1 NeoPixel
// 中文：按键 #1 的 NeoPixel
static constexpr uint8_t kNeoPixelPin1 = MACROPAD_KEY1_LED_PIN;
static constexpr uint16_t kNeoPixelCount1 = 1;

// EN: Key #2 (new Grove keycap): S1->D8 (button), S2->D9 (NeoPixel)
// 中文：按键 #2（新加 Grove 键帽）：S1->D8（按键输出），S2->D9（灯控信号）
static constexpr uint8_t kButtonPin2 = MACROPAD_KEY2_BTN_PIN;
static constexpr uint8_t kNeoPixelPin2 = MACROPAD_KEY2_LED_PIN;
static constexpr uint16_t kNeoPixelCount2 = 1;

// EN: Key #3 (voice/dictation toggle): S1->D5 (button), S2->D4 (NeoPixel)
// 中文：按键 #3（语音/听写开关）：S1->D5（按键输出），S2->D4（灯控信号）
static constexpr uint8_t kButtonPin3 = MACROPAD_KEY3_BTN_PIN;
static constexpr uint8_t kNeoPixelPin3 = MACROPAD_KEY3_LED_PIN;
static constexpr uint16_t kNeoPixelCount3 = 1;

static constexpr uint8_t kButtonPin4 = MACROPAD_KEY4_BTN_PIN;
static constexpr uint8_t kNeoPixelPin4 = MACROPAD_KEY4_LED_PIN;
static constexpr uint16_t kNeoPixelCount4 = 1;

static constexpr uint32_t kVoiceHoldMaxMs = 30000;
static constexpr uint8_t kVoiceEndBlinkTimes = 3;
static constexpr uint16_t kVoiceEndBlinkOnMs = 80;
static constexpr uint16_t kVoiceEndBlinkOffMs = 80;

// EN: macOS shortcuts (Command + key)
// 中文：macOS 快捷键（Command + 按键）
// EN: HID keycodes: A=0x04, B=0x05, C=0x06 ... V=0x19
// 中文：HID 键码：A=0x04, B=0x05, C=0x06 ... V=0x19
static constexpr uint8_t kHidKeycodeC = 0x06; // EN: 'c' | 中文：字母 c
static constexpr uint8_t kHidKeycodeV = 0x19; // EN: 'v' | 中文：字母 v
static constexpr uint8_t kHidKeycodeEnter = 0x28; // EN: Enter | 中文：回车
static constexpr uint8_t kHidKeycodeEscape = 0x29; // EN: Escape | 中文：Esc
static constexpr uint8_t kHidKeycodeTab = 0x2B; // EN: Tab | 中文：Tab
static constexpr uint8_t kHidKeycodeSpace = 0x2C; // EN: Space | 中文：空格
static constexpr uint8_t kHidKeycodePeriod = 0x37; // EN: . and > | 中文：句号

// EN: Configure your macOS Dictation shortcut to "Ctrl + Shift + Space", then this works.
// 中文：把 macOS 的“听写”快捷键设置为“Ctrl + Shift + Space”，即可按住说话/松开结束。
static constexpr uint8_t kDictationModifier = MACROPAD_DICTATION_MODIFIER;
static constexpr uint8_t kDictationKeycode = MACROPAD_DICTATION_KEYCODE;

// EN: BLE HID keyboard (macro pad)
// 中文：BLE HID 键盘（宏键盘）
BLEDis bledis;
BLEHidAdafruit blehid;

Adafruit_NeoPixel pixels1(kNeoPixelCount1, kNeoPixelPin1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels2(kNeoPixelCount2, kNeoPixelPin2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels3(kNeoPixelCount3, kNeoPixelPin3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels4(kNeoPixelCount4, kNeoPixelPin4, NEO_GRB + NEO_KHZ800);

static void setPixels(uint32_t c1, uint32_t c2, uint32_t c3, uint32_t c4) {
  pixels1.setPixelColor(0, c1);
  pixels1.show();
  pixels2.setPixelColor(0, c2);
  pixels2.show();
  pixels3.setPixelColor(0, c3);
  pixels3.show();
  pixels4.setPixelColor(0, c4);
  pixels4.show();
}

static void setAllPixels(uint32_t c) {
  setPixels(c, c, c, c);
}

static void blinkPixel3(uint32_t color) {
  for (uint8_t i = 0; i < kVoiceEndBlinkTimes; i++) {
    pixels3.setPixelColor(0, color);
    pixels3.show();
    delay(kVoiceEndBlinkOnMs);
    pixels3.setPixelColor(0, 0);
    pixels3.show();
    delay(kVoiceEndBlinkOffMs);
  }
}

static void sendShortcut(uint8_t modifier, uint8_t keycode) {
  uint8_t keys[6] = {keycode, 0, 0, 0, 0, 0};
  blehid.keyboardReport(modifier, keys);
  delay(8);
  blehid.keyRelease();
  delay(8);
}

static void sendMacCommandShortcut(uint8_t keycode) {
  // EN: Send a macOS shortcut: Command + <key>
  // 中文：发送 macOS 快捷键：Command + <键>
  uint8_t keys[6] = {keycode, 0, 0, 0, 0, 0};

  // EN: Left GUI maps to Command on macOS
  // 中文：Left GUI 在 macOS 上对应 Command
  blehid.keyboardReport(KEYBOARD_MODIFIER_LEFTGUI, keys);
  delay(8);

  // EN: Release all keys, otherwise host may think keys are held
  // 中文：发送松开报告，否则主机可能认为按键一直按住
  blehid.keyRelease();
  delay(8);
}

static void startAdv() {
  // EN: Advertising as a keyboard
  // 中文：以“蓝牙键盘”身份广播
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);
  Bluefruit.Advertising.addService(blehid);
  Bluefruit.Advertising.addName();

  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);
  Bluefruit.Advertising.setFastTimeout(30);
  Bluefruit.Advertising.start(0);
}

static void setConnPixels(bool connected, bool voiceActive) {
  const uint32_t c = connected ? pixels1.Color(30, 255, 60) : pixels1.Color(255, 40, 40);
  const uint32_t c3 = voiceActive ? pixels1.Color(190, 60, 255) : c;
  setPixels(c, c, c3, c);
}

static void onConnect(uint16_t conn_handle) {
  (void)conn_handle;
  setConnPixels(true, false);
  Serial.println("ble connected");
}

static void onDisconnect(uint16_t conn_handle, uint8_t reason) {
  (void)conn_handle;
  (void)reason;
  setConnPixels(false, false);
  Serial.println("ble disconnected (advertising)");
}

void setup() {
  Serial.begin(kBaud);
  uint32_t startMs = millis();
  while (!Serial && (millis() - startMs) < 1500) {
    delay(10);
  }

  // EN: Button inputs
  // 中文：按键输入
  pinMode(kButtonPin1, kButtonActiveLow ? INPUT_PULLUP : INPUT);
  pinMode(kButtonPin2, kButtonActiveLow ? INPUT_PULLUP : INPUT);
  pinMode(kButtonPin3, kButtonActiveLow ? INPUT_PULLUP : INPUT);
  pinMode(kButtonPin4, kButtonActiveLow ? INPUT_PULLUP : INPUT);

  // EN: NeoPixel status LEDs
  // 中文：NeoPixel 状态灯
  pixels1.begin();
  pixels1.setBrightness(80);
  pixels1.clear();
  pixels1.show();

  pixels2.begin();
  pixels2.setBrightness(80);
  pixels2.clear();
  pixels2.show();

  pixels3.begin();
  pixels3.setBrightness(80);
  pixels3.clear();
  pixels3.show();

  pixels4.begin();
  pixels4.setBrightness(80);
  pixels4.clear();
  pixels4.show();

  // EN: Init BLE stack
  // 中文：初始化蓝牙协议栈
  Bluefruit.begin();
  Bluefruit.setTxPower(4);
  Bluefruit.setName("XIAO MacroPad");
  Bluefruit.Periph.setConnectCallback(onConnect);
  Bluefruit.Periph.setDisconnectCallback(onDisconnect);

  // EN: Device info (shown in some hosts)
  // 中文：设备信息（部分系统会显示）
  bledis.setManufacturer("Seeed Studio");
  bledis.setModel("XIAO nRF52840");
  bledis.begin();

  // EN: Start HID service
  // 中文：启动 HID 服务
  blehid.begin();

  onDisconnect(0, 0);
  startAdv();

  oled_init();
  oled_show_hollow();

  Serial.println("ready: pair via Bluetooth settings, then press the key");
}

void loop() {
  static bool stablePressed1 = false;
  static bool lastReadingPressed1 = false;
  static uint32_t lastChangeMs1 = 0;
  static uint32_t pressEdgeCount1 = 0;

  static bool stablePressed2 = false;
  static bool lastReadingPressed2 = false;
  static uint32_t lastChangeMs2 = 0;
  static uint32_t pressEdgeCount2 = 0;

  static bool stablePressed3 = false;
  static bool lastReadingPressed3 = false;
  static uint32_t lastChangeMs3 = 0;
  static uint32_t pressEdgeCount3 = 0;
  static uint32_t releaseEdgeCount3 = 0;

  static bool stablePressed4 = false;
  static bool lastReadingPressed4 = false;
  static uint32_t lastChangeMs4 = 0;
  static uint32_t pressEdgeCount4 = 0;

  static uint32_t lastStatusPrintMs = 0;

  static bool voiceActive = false;
  static bool voiceWaitRelease = false;
  static uint32_t voiceStartMs = 0;

  const uint32_t nowMs = millis();

  const int level1 = digitalRead(kButtonPin1);
  const int level2 = digitalRead(kButtonPin2);
  const int level3 = digitalRead(kButtonPin3);
  const int level4 = digitalRead(kButtonPin4);
  const bool readingPressed1 = kButtonActiveLow ? (level1 == LOW) : (level1 == HIGH);
  const bool readingPressed2 = kButtonActiveLow ? (level2 == LOW) : (level2 == HIGH);
  const bool readingPressed3 = kButtonActiveLow ? (level3 == LOW) : (level3 == HIGH);
  const bool readingPressed4 = kButtonActiveLow ? (level4 == LOW) : (level4 == HIGH);

  if (nowMs - lastStatusPrintMs >= 500) {
    lastStatusPrintMs = nowMs;
    Serial.print("k1 level=");
    Serial.print(level1);
    Serial.print(" pressed=");
    Serial.print(readingPressed1 ? 1 : 0);
    Serial.print(" stable=");
    Serial.print(stablePressed1 ? 1 : 0);
    Serial.print(" | k2 level=");
    Serial.print(level2);
    Serial.print(" pressed=");
    Serial.print(readingPressed2 ? 1 : 0);
    Serial.print(" stable=");
    Serial.print(stablePressed2 ? 1 : 0);
    Serial.print(" | k3 level=");
    Serial.print(level3);
    Serial.print(" pressed=");
    Serial.print(readingPressed3 ? 1 : 0);
    Serial.print(" stable=");
    Serial.print(stablePressed3 ? 1 : 0);
    Serial.print(" voice=");
    Serial.print(voiceActive ? 1 : 0);
    Serial.print(" | connected=");
    Serial.println(Bluefruit.Periph.connected() ? 1 : 0);
  }

  if (readingPressed1 != lastReadingPressed1) {
    lastReadingPressed1 = readingPressed1;
    lastChangeMs1 = nowMs;
  }

  bool pressedEdge1 = false;
  if ((nowMs - lastChangeMs1) >= kDebounceMs && stablePressed1 != readingPressed1) {
    stablePressed1 = readingPressed1;
    if (stablePressed1) {
      pressedEdge1 = true;
    }
  }

  if (readingPressed2 != lastReadingPressed2) {
    lastReadingPressed2 = readingPressed2;
    lastChangeMs2 = nowMs;
  }

  bool pressedEdge2 = false;
  if ((nowMs - lastChangeMs2) >= kDebounceMs && stablePressed2 != readingPressed2) {
    stablePressed2 = readingPressed2;
    if (stablePressed2) {
      pressedEdge2 = true;
    }
  }

  if (readingPressed3 != lastReadingPressed3) {
    lastReadingPressed3 = readingPressed3;
    lastChangeMs3 = nowMs;
  }

  bool pressedEdge3 = false;
  bool releasedEdge3 = false;
  if ((nowMs - lastChangeMs3) >= kDebounceMs && stablePressed3 != readingPressed3) {
    stablePressed3 = readingPressed3;
    if (stablePressed3) {
      pressedEdge3 = true;
    } else {
      releasedEdge3 = true;
    }
  }

  if (readingPressed4 != lastReadingPressed4) {
    lastReadingPressed4 = readingPressed4;
    lastChangeMs4 = nowMs;
  }

  bool pressedEdge4 = false;
  if ((nowMs - lastChangeMs4) >= kDebounceMs && stablePressed4 != readingPressed4) {
    stablePressed4 = readingPressed4;
    if (stablePressed4) {
      pressedEdge4 = true;
    }
  }

  if (voiceActive && (nowMs - voiceStartMs) >= kVoiceHoldMaxMs) {
    voiceActive = false;
    voiceWaitRelease = true;

    if (Bluefruit.Periph.connected()) {
      sendShortcut(kDictationModifier, kDictationKeycode);
      Serial.println("key3 dictation stop: timeout");
    } else {
      Serial.println("key3 dictation stop skipped: not connected");
    }

    blinkPixel3(pixels1.Color(255, 60, 60));
    setConnPixels(Bluefruit.Periph.connected(), false);
  }

  if (!pressedEdge1 && !pressedEdge2 && !pressedEdge3 && !releasedEdge3 && !pressedEdge4) {
    delay(5);
    return;
  }

  if (pressedEdge1) {
    pressEdgeCount1++;
    Serial.print("key1 pressedEdge #");
    Serial.print(pressEdgeCount1);
    Serial.print(" connected=");
    Serial.println(Bluefruit.Periph.connected() ? 1 : 0);
  }

  if (pressedEdge2) {
    pressEdgeCount2++;
    Serial.print("key2 pressedEdge #");
    Serial.print(pressEdgeCount2);
    Serial.print(" connected=");
    Serial.println(Bluefruit.Periph.connected() ? 1 : 0);
  }

  if (pressedEdge3) {
    pressEdgeCount3++;
    Serial.print("key3 pressedEdge #");
    Serial.print(pressEdgeCount3);
    Serial.print(" connected=");
    Serial.println(Bluefruit.Periph.connected() ? 1 : 0);

    if (!voiceWaitRelease && !voiceActive) {
      voiceActive = true;
      voiceStartMs = nowMs;

      if (!Bluefruit.Periph.connected()) {
        setAllPixels(pixels1.Color(255, 120, 20));
        delay(40);
        setConnPixels(false, false);
        Serial.println("key3 pressed (not connected)");
        voiceActive = false;
      } else {
        sendShortcut(kDictationModifier, kDictationKeycode);
        Serial.println("key3 dictation start");
        setConnPixels(true, true);
      }
    } else {
      Serial.println("key3 start ignored");
    }
  }

  if (pressedEdge4) {
    pressEdgeCount4++;
    Serial.print("key4 pressedEdge #");
    Serial.print(pressEdgeCount4);
    Serial.print(" connected=");
    Serial.println(Bluefruit.Periph.connected() ? 1 : 0);
  }

  if (releasedEdge3) {
    releaseEdgeCount3++;
    Serial.print("key3 releasedEdge #");
    Serial.println(releaseEdgeCount3);

    voiceWaitRelease = false;
    if (voiceActive) {
      voiceActive = false;
      if (Bluefruit.Periph.connected()) {
        sendShortcut(kDictationModifier, kDictationKeycode);
        Serial.println("key3 dictation stop: release");
      } else {
        Serial.println("key3 dictation stop skipped: not connected");
      }
      blinkPixel3(pixels1.Color(255, 200, 40));
      setConnPixels(Bluefruit.Periph.connected(), false);
    }
  }

  if (!Bluefruit.Periph.connected()) {
    setAllPixels(pixels1.Color(255, 40, 40));
    delay(5);
    return;
  }

  setAllPixels(pixels1.Color(60, 120, 255));

  if (pressedEdge1) {
    sendShortcut(0, kHidKeycodeEnter);
    Serial.println("key1 enter sent");
  }

  if (pressedEdge2) {
    sendShortcut(MACROPAD_ACCEPT_STEP1_MOD, MACROPAD_ACCEPT_STEP1_KEY);
    delay(15);
    sendShortcut(MACROPAD_ACCEPT_STEP2_MOD, MACROPAD_ACCEPT_STEP2_KEY);
    Serial.println("key2 accept sent");
  }

  if (pressedEdge4) {
    sendShortcut(MACROPAD_ABORT_STEP1_MOD, MACROPAD_ABORT_STEP1_KEY);
    delay(15);
    sendShortcut(MACROPAD_ABORT_STEP2_MOD, MACROPAD_ABORT_STEP2_KEY);
    delay(15);
    sendShortcut(MACROPAD_ABORT_STEP3_MOD, MACROPAD_ABORT_STEP3_KEY);
    Serial.println("key4 abort sent");
  }

  delay(10);
  setConnPixels(true, voiceActive);
  delay(5);
}
