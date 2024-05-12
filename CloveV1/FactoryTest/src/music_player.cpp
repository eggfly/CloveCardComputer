#include <Audio.h>
#include <Arduino_GFX_Library.h>
#include <rom/rtc.h>
#include <vector>
#include <unordered_set>

#include "fft.h"
#include "res.h"
#include "utils.h"
#include <app.h>

void startNextSong(bool isNextOrPrev);
bool endsWithIgnoreCase(const char *base, const char *str);

void print_reset_reason(RESET_REASON reason)
{
  switch (reason)
  {
  case 1:
    Serial.println("POWERON_RESET");
    break; /**<1, Vbat power on reset*/
  case 3:
    Serial.println("SW_RESET");
    break; /**<3, Software reset digital core*/
  case 4:
    Serial.println("OWDT_RESET");
    break; /**<4, Legacy watch dog reset digital core*/
  case 5:
    Serial.println("DEEPSLEEP_RESET");
    break; /**<5, Deep Sleep reset digital core*/
  case 6:
    Serial.println("SDIO_RESET");
    break; /**<6, Reset by SLC module, reset digital core*/
  case 7:
    Serial.println("TG0WDT_SYS_RESET");
    break; /**<7, Timer Group0 Watch dog reset digital core*/
  case 8:
    Serial.println("TG1WDT_SYS_RESET");
    break; /**<8, Timer Group1 Watch dog reset digital core*/
  case 9:
    Serial.println("RTCWDT_SYS_RESET");
    break; /**<9, RTC Watch dog Reset digital core*/
  case 10:
    Serial.println("INTRUSION_RESET");
    break; /**<10, Instrusion tested to reset CPU*/
  case 11:
    Serial.println("TGWDT_CPU_RESET");
    break; /**<11, Time Group reset CPU*/
  case 12:
    Serial.println("SW_CPU_RESET");
    break; /**<12, Software reset CPU*/
  case 13:
    Serial.println("RTCWDT_CPU_RESET");
    break; /**<13, RTC Watch dog Reset CPU*/
  case 14:
    Serial.println("EXT_CPU_RESET");
    break; /**<14, for APP CPU, reseted by PRO CPU*/
  case 15:
    Serial.println("RTCWDT_BROWN_OUT_RESET");
    break; /**<15, Reset when the vdd voltage is not stable*/
  case 16:
    Serial.println("RTCWDT_RTC_RESET");
    break; /**<16, RTC Watch dog reset digital core and rtc module*/
  default:
    Serial.println("NO_MEAN");
  }
}

#define AUDIO_LOG

const bool APP_DEBUG = false;

#define ssid "wifi_ssid"
#define password "wifi_password"

#define SD_CS 42
#define SD_MOSI 41
#define SD_MISO 39
#define SD_SCLK 40

#define I2S_DOUT 46
#define I2S_BCLK 47
#define I2S_LRC 48

// #define SD_MODE 46

#define CANVAS_WIDTH 320
#define CANVAS_HEIGHT 240

#define ISR_SHORT_DITHERING_TIME_MS 50

#define KEY_MID 0
#define KEY_UP 14
#define KEY_DOWN 32
#define KEY_LEFT 36
#define KEY_RIGHT 39
#define KEY_A 35
#define KEY_B 34

// #define RGB565(r, g, b) ((r & 0b11111000) << 8) | ((g & 0b11111100) << 3) | (b >> 3)

// #define TFT_BACKLIGHT 45
// #define VBAT_PIN 33

uint8_t brightness_level = 6;
PROGMEM const uint8_t brightness_levels[] = {0, 1, 2, 4, 8, 16, 32, 64, 128, 255};
const uint8_t brightness_level_size = sizeof(brightness_levels) / sizeof(brightness_levels[0]);

unsigned long last_isr_time;

enum
{
  GAMEPAD_INPUT_UP = 0,
  GAMEPAD_INPUT_DOWN,
  GAMEPAD_INPUT_LEFT,
  GAMEPAD_INPUT_RIGHT,
  GAMEPAD_INPUT_A,
  GAMEPAD_INPUT_B,
  GAMEPAD_INPUT_MID,

  GAMEPAD_INPUT_MAX
};

bool gamepad_values[GAMEPAD_INPUT_MAX];
bool gamepad_changed = false;

// float p = 3.1415926;

void lcd_set_brightness(uint8_t level)
{
  return;
  if (level < brightness_level_size)
  {
    uint8_t brightness = brightness_levels[level];
    Serial.printf("lcd_set_brightness: level=%d, value=%d\n", level, brightness);
    ledcWrite(0, 255 - brightness);
  }
}

Audio audio(false, 3, I2S_NUM_1);

std::vector<String> m_songFiles{};

bool listDir(fs::FS &fs, const char *dirname, uint8_t levels)
{
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root)
  {
    Serial.println("Failed to open directory");
    return false;
  }
  if (!root.isDirectory())
  {
    Serial.println("Not a directory");
    return false;
  }

  File file = root.openNextFile();
  while (file)
  {
    if (file.isDirectory())
    {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels)
      {
        listDir(fs, file.path(), levels - 1);
      }
    }
    else
    {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
  Serial.println("listDir end");
  return true;
}

bool is_pure_ascii_str(const char *str, size_t max_len)
{
  auto len = min(strlen(str), max_len);
  for (size_t i = 0; i < len; i++)
  {
    char c = str[i];
    if (c > 127)
    {
      return false;
    }
  }
  return true;
}

void populateMusicFileList(String path, size_t depth)
{
  Serial.printf("search: %s, depth=%d\n", path.c_str(), depth);
  File musicDir = SD.open(path);
  bool nextFileFound;
  do
  {
    nextFileFound = false;
    File entry = musicDir.openNextFile();
    if (entry)
    {
      nextFileFound = true;
      if (entry.isDirectory())
      {
        if (depth)
        {
          populateMusicFileList(entry.path(), depth - 1);
        }
      }
      else
      {
        const bool entryIsFile = entry.size() > 4096;
        if (entryIsFile)
        {
          if (APP_DEBUG)
          {
            Serial.print(entry.path());
            Serial.print(" size=");
            Serial.println(entry.size());
          }
          if (endsWithIgnoreCase(entry.name(), ".mp3") || endsWithIgnoreCase(entry.name(), ".flac") || endsWithIgnoreCase(entry.name(), ".aac") || endsWithIgnoreCase(entry.name(), ".wav"))
          {
            m_songFiles.push_back(entry.path());
          }
        }
      }
      entry.close();
    }
  } while (nextFileFound);
}

void gamepad_isr()
{
  if (millis() - last_isr_time < ISR_SHORT_DITHERING_TIME_MS)
  {
    return;
  }
  last_isr_time = millis();
  gamepad_values[GAMEPAD_INPUT_UP] = !digitalRead(KEY_UP);
  gamepad_values[GAMEPAD_INPUT_DOWN] = !digitalRead(KEY_DOWN);
  gamepad_values[GAMEPAD_INPUT_LEFT] = !digitalRead(KEY_LEFT);
  gamepad_values[GAMEPAD_INPUT_RIGHT] = !digitalRead(KEY_RIGHT);
  gamepad_values[GAMEPAD_INPUT_A] = !digitalRead(KEY_A);
  gamepad_values[GAMEPAD_INPUT_B] = !digitalRead(KEY_B);
  gamepad_changed = true;
}

void gamepad_isr_new()
{
  // if (millis() - last_isr_time < ISR_SHORT_DITHERING_TIME_MS) {
  //   return;
  // }
  last_isr_time = millis();
  gamepad_values[GAMEPAD_INPUT_MID] = !digitalRead(KEY_MID);
  gamepad_changed = true;
}

uint8_t volume = 4;

typedef struct
{
  int16_t x;
  int16_t y;
  uint16_t w;
  uint16_t h;
} text_bounds;

void drawText(const char *text, uint16_t x, uint16_t y, uint16_t color)
{
  gfx->setCursor(x, y);
  gfx->setTextColor(color);
  gfx->println(text);
}

void drawTextWithShadow(const char *text, uint16_t x, uint16_t y, uint16_t color, uint16_t shadow_color)
{
  // shadow background
  drawText(text, x - 1, y, shadow_color);
  drawText(text, x, y - 1, shadow_color);
  drawText(text, x + 1, y, shadow_color);
  drawText(text, x, y + 1, shadow_color);
  // draw text
  drawText(text, x, y, color);
}

// void mediabuttons() {
//   // play
//   // tft.fillScreen(ST77XX_BLACK);
//   tft.fillRoundRect(25, 10, 78, 60, 8, ST77XX_WHITE);
//   tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_RED);
//   delay(500);
//   // pause
//   tft.fillRoundRect(25, 90, 78, 60, 8, ST77XX_WHITE);
//   tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_GREEN);
//   tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_GREEN);
//   delay(500);
//   // play color
//   tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_BLUE);
//   delay(50);
//   // pause color
//   tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_RED);
//   tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_RED);
//   // play color
//   tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_GREEN);
// }

int m_activeSongIdx{-1};

char music_title[256] = "";
char music_album[256] = "";
char music_artist[256] = "";

// void setChinesePrintEnabled(bool enable)
// {
//   if (enable)
//   {
//     gfx->setUTF8Print(true);
//     gfx->setFont(u8g2_font_unifont_t_chinese);
//   }
//   else
//   {
//     gfx->setUTF8Print(false);
//     gfx->setFont((const GFXfont *)NULL);
//   }
// }

const uint8_t CHINESE_FONT_HEIGHT = 16;

bool shuffle_mode = true;

#define PLAYER_TITLE "        Console32 Player"
// #define PLAYER_TITLE "       Console32 音乐播放器"

double vbat = 0;

void drawScreen()
{
  auto start = millis();
  gfx->setTextWrap(false);
  auto gray = RGB565(0x80, 0x80, 0x80);
  gfx->fillRect(0, 0, CANVAS_WIDTH, CANVAS_HEIGHT, gray);

  gfx->draw16bitRGBBitmap((CANVAS_WIDTH - 240) / 2, 0, (uint16_t *)apple_music_240x240_map, 240, 240);

  // gfx->println("Console32 - 音乐播放器\n（支持 mp3、flac、aac）");
  // gfx->println("1 My non-utf-8");
  // setChinesePrintEnabled(true);

  gfx->setTextColor(RGB565(0x80, 0x80, 0x80));
  gfx->setCursor(1, CHINESE_FONT_HEIGHT);
  gfx->println(PLAYER_TITLE);

  gfx->setTextColor(RGB565(0xf0, 0xf0, 0xf0));
  gfx->setCursor(0, CHINESE_FONT_HEIGHT - 1);
  gfx->println(PLAYER_TITLE);

  // setChinesePrintEnabled(false);
  {
    // setChinesePrintEnabled(true);
    char batteryText[64];
    // gfx->setTextSize(2);
    sprintf(batteryText, "%.2lfV 音量:%d", vbat, volume);
    drawTextWithShadow(batteryText, 200, 16, RED, RGB565(0x80, 0x0, 0x0));
    // setChinesePrintEnabled(false);
    // gfx->setTextSize(1);
  }
  // {
  //   setChinesePrintEnabled(true);
  //   char volumeText[64];
  //   sprintf(volumeText, "音量:%d", volume);
  //   drawTextWithShadow(volumeText, 243, 16, RED, RGB565(0x80, 0x0, 0x0));
  //   setChinesePrintEnabled(false);
  // }

  // gfx->println("Arduino 专案始于2003年，作为意大利伊夫雷亚地区伊夫雷亚互动设计研究所的学生专案，目的是为新手和专业人员提供一种低成本且简单的方法，以建立使用感测器与环境相互作用的装置执行器。适用于初学者爱好者的此类装置的常见范例包括感测器、简单机械人、恒温器和运动检测器。");
  // gfx->println("Arduino 是一個開源嵌入式硬體平台，用來供用戶製作可互動式的嵌入式專案。此外 Arduino 作為一個開源硬體和開源軟件的公司，同時兼有專案和用戶社群。該公司負責設計和製造Arduino電路板及相關附件。這些產品按照GNU寬通用公共許可證（LGPL）或GNU通用公共許可證（GPL）[1]許可的開源硬體和軟件分發的，Arduino 允許任何人製造 Arduino 板和軟件分發。 Arduino 板可以以預裝的形式商業銷售，也可以作為DIY套件購買。");
  if (m_activeSongIdx >= 0 && m_activeSongIdx < m_songFiles.size())
  {
    drawTextWithShadow(m_songFiles[m_activeSongIdx].c_str(), 0, 231, RGB565(0xf0, 0xf0, 0xf0), RGB565(0x80, 0x80, 0x80));
  }
  // ascii font height is 8
  const uint8_t margin = 8;
  const uint8_t left = 0;
  const uint8_t top = 130;

  // 1st line: music title
  if (strlen(music_title) > 0)
  {
    // gfx->draw16bitRGBBitmap(left, top + 36, (uint16_t*)epd_bitmap_music_album_32x32, epd_bitmap_music_album_32x32_alpha, 32, 32);
    if (is_pure_ascii_str(music_title, sizeof(music_title) / sizeof(music_title[0])))
    {
      // setChinesePrintEnabled(false);
      text_bounds b;
      gfx->getTextBounds(music_title, 0, 0, &b.x, &b.y, &b.w, &b.h);
      Serial.printf("%d, %d, %d, %d\n", b.x, b.y, b.w, b.h);
      int16_t calculated_left = 120 - b.w / 2;
      int16_t calculated_top = 176 - b.h / 2;
      drawTextWithShadow(music_title, calculated_left, calculated_top, RGB565(0xf0, 0xf0, 0xf0), RGB565(0x3f, 0x3f, 0x3f));
    }
    else
    {
      // setChinesePrintEnabled(true);
      text_bounds b;
      gfx->getTextBounds(music_title, left + 32 + margin, top + 36, &b.x, &b.y, &b.w, &b.h);
      Serial.printf("%d, %d, %d, %d\n", b.x, b.y, b.w, b.h);
      drawTextWithShadow(music_title, left + 32 + margin, top + 36, RGB565(0xf0, 0xf0, 0xf0), RGB565(0x3f, 0x3f, 0x3f));
    }
  }

  // 2nd line: artist and album
  if (strlen(music_artist) > 0 || strlen(music_album) > 0)
  {
    char artist_and_album[128];
    snprintf(artist_and_album, sizeof(artist_and_album) / sizeof(artist_and_album[0]), "%s <%s>", music_artist, music_album);
    if (is_pure_ascii_str(artist_and_album, sizeof(artist_and_album) / sizeof(artist_and_album[0])))
    {
      // setChinesePrintEnabled(false);
      text_bounds b;
      gfx->getTextBounds(artist_and_album, 0, 0, &b.x, &b.y, &b.w, &b.h);
      Serial.printf("%d, %d, %d, %d\n", b.x, b.y, b.w, b.h);
      int16_t calculated_left = 120 - b.w / 2;
      int16_t calculated_top = 189 - b.h / 2;
      drawTextWithShadow(artist_and_album, calculated_left, calculated_top, RGB565(0xf0, 0xf0, 0xf0), RGB565(0x3f, 0x3f, 0x3f));
    }
    else
    {
      // setChinesePrintEnabled(true);
      drawTextWithShadow(artist_and_album, left + 32 + margin, top, RGB565(0xf0, 0xf0, 0xf0), RGB565(0x3f, 0x3f, 0x3f));
    }
  }
  // gfx->draw16bitRGBBitmapWith8BitAlpha(0, 0, (uint16_t*)RGB_test_240x186_alpha_map, (uint8_t*)RGB_test_240x186_alpha_map_alpha, 240, 186);
  // gfx->draw16bitRGBBitmap(0, 0, (uint16_t*)RGB_test_240x186_alpha_map, 240, 186);
  flush_screen();
  // Serial.printf("60 percent rgb size: %d %d\n", sizeof(RGB_test_240x186_alpha_map), sizeof(RGB_test_240x186_alpha_map_alpha) );
  Serial.printf("drawScreen() cost %ld ms\n", millis() - start);
}

void setupButtonsNew()
{
  detachInterrupt(digitalPinToInterrupt(KEY_MID));
  pinMode(KEY_MID, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(KEY_MID), gamepad_isr_new, CHANGE);
  printf("setupButtonsNew() OK.\n");
}

void setupButtons()
{
  pinMode(KEY_UP, INPUT);
  pinMode(KEY_DOWN, INPUT);
  pinMode(KEY_LEFT, INPUT);
  pinMode(KEY_RIGHT, INPUT);
  pinMode(KEY_A, INPUT);
  pinMode(KEY_B, INPUT);

  attachInterrupt(digitalPinToInterrupt(KEY_UP), gamepad_isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(KEY_DOWN), gamepad_isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(KEY_LEFT), gamepad_isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(KEY_RIGHT), gamepad_isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(KEY_A), gamepad_isr, CHANGE);
  attachInterrupt(digitalPinToInterrupt(KEY_B), gamepad_isr, CHANGE);
}

SPIClass SD_SPI(HSPI);

// void pmu_init()
// {
//   bool result = PMU.begin(Wire, AXP2101_SLAVE_ADDRESS, i2c_sda, i2c_scl);

//   if (result == false)
//   {
//     Serial.println("PMU is not online...");
//     while (1)
//       delay(50);
//   }

//   Serial.printf("getID:0x%x\n", PMU.getChipID());

//   PMU.enableBattDetection();
//   PMU.enableVbusVoltageMeasure();
//   PMU.enableBattVoltageMeasure();
//   PMU.enableSystemVoltageMeasure();
//   printf("PMU initialized OK!\n");
// }

void setup_music_player()
{
  printf("setup_music_player()\n");
  // setupButtonsNew();

  // pinMode(SD_MODE, OUTPUT);
  // ENABLE SPEAKER
  // digitalWrite(SD_MODE, HIGH);

  // pinMode(SD_CS, OUTPUT);
  // digitalWrite(SD_CS, HIGH);
  SD_SPI.begin(SD_SCLK, SD_MISO, SD_MOSI);
  // SD_SPI.setFrequency(1000000);

  // WiFi.mode(WIFI_OFF);
  // gfx->begin();
  // gfx->fillScreen(BLACK);

  Serial.println("CPU0 reset reason: ");
  print_reset_reason(rtc_get_reset_reason(0));

  Serial.println("CPU1 reset reason: ");
  print_reset_reason(rtc_get_reset_reason(1));

  auto sd_ret = SD.begin(SD_CS, SD_SPI, 80000000);
  Serial.print("SD.begin()=");
  Serial.println(sd_ret);

  drawScreen();
  lcd_set_brightness(brightness_level);

  if (sd_ret != 1)
  {
    drawText("NO TF CARD?", 100, 100, RGB565(255, 127, 127));
    flush_screen();
    delay(3000);
    for (;;)
      ;
    // ESP.restart();
  }
  // setup_keypad();
  Serial.println(F("Initialized"));

  audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
  audio.setVolume(volume); // 0...21
  // delay(100);

  // large block of text
  // tft.fillScreen(ST77XX_BLACK);

  //    WiFi.mode(WIFI_STA);
  //    WiFi.begin(ssid, password);
  //    while (!WiFi.isConnected()) {
  //      delay(10);
  //    }

  // ESP_LOGI(TAG, "Connected");
  // ESP_LOGI(TAG, "Starting...\n");

  // listDir(SD, "/", 0);
  // listDir(SD, "/musics", 0);
  populateMusicFileList("/", 1); // depth = 1
  Serial.print("MusicFileList length: ");
  Serial.println(m_songFiles.size());

  // audio.connecttoFS(SD, "/StandByMe.flac");
  // audio.connecttoFS(SD, "yemao.mp3");
  // audio.connecttohost("http://air.ofr.fm:8008/jazz/mp3/128");
  // audio.connecttospeech("Миска вареників з картоплею та шкварками, змащених салом!", "uk-UA");

  // drawText("Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", ST77XX_WHITE);

  // mediabuttons();
}

void autoPlayNextSong()
{
  if (m_songFiles.size() == 0)
  {
    delay(100);
    return;
  }
  if (!audio.isRunning())
  {
    Serial.println("autoPlay: playNextSong()");
    startNextSong(true);
  }
}

std::unordered_set<int> m_played_songs{};

void clearSongInfo()
{
  music_title[0] = '\0';
  music_artist[0] = '\0';
  music_album[0] = '\0';
}

void startNextSong(bool isNextOrPrev)
{
  if (m_songFiles.size() == 0)
  {
    return;
  }
  m_played_songs.insert(m_activeSongIdx);
  if (m_played_songs.size() * 2 > m_songFiles.size())
  {
    Serial.println("re-shuffle.");
    m_played_songs.clear();
  }
  if (isNextOrPrev)
  {
    m_activeSongIdx++;
  }
  else
  {
    m_activeSongIdx--;
  }
  if (shuffle_mode)
  {
    do
    {
      m_activeSongIdx = random(m_songFiles.size());
    } while (m_played_songs.find(m_activeSongIdx) != std::end(m_played_songs));
  }
  //  if (m_activeSongIdx >= m_songFiles.size() || m_activeSongIdx < 0) {
  //    m_activeSongIdx = 0;
  //  }
  m_activeSongIdx %= m_songFiles.size();
  Serial.print("songIndex=");
  Serial.print(m_activeSongIdx);
  Serial.print(", total=");
  Serial.println(m_songFiles.size());

  if (audio.isRunning())
  {
    audio.stopSong();
  }
  clearSongInfo();
  // walkaround
  // setupButtonsNew();
  audio.connecttoSD(m_songFiles[m_activeSongIdx].c_str());

  drawScreen();
  Serial.println(m_songFiles[m_activeSongIdx].c_str());
}

void volumeUp()
{
  if (volume < 21)
  {
    volume++;
    audio.setVolume(volume); // 0...21
  }
}

void volumeDown()
{
  if (volume > 0)
  {
    volume--;
    audio.setVolume(volume); // 0...21
  }
  else
  {
    // volume == 0;
  }
}

unsigned long gamepad_last_handled_time = 0;

void handleGamepadChange()
{
  if (millis() - gamepad_last_handled_time < 500)
  {
    return;
  }
  bool handled = false;
  if (keypad_states[GAMEPAD_UP])
  {
    volumeUp();
    printf("set volume to %d\n", audio.getVolume());
    handled = true;
  }
  else if (keypad_states[GAMEPAD_DOWN])
  {
    volumeDown();
    printf("set volume to %d\n", audio.getVolume());
    handled = true;
  }
  else if (keypad_states[GAMEPAD_LEFT])
  {
    startNextSong(false);
    handled = true;
  }
  else if (keypad_states[GAMEPAD_RIGHT])
  {
    startNextSong(true);
    handled = true;
  }
  else if (keypad_states[GAMEPAD_PRESSED])
  {
    audio.pauseResume();
    handled = true;
  }
  if (handled)
  {
    gamepad_last_handled_time = millis();
  }
  // if ([])
  // {
  //   gamepad_changed = false;

  //   Serial.print("gamepad values: ");
  //   for (int i = 0; i < GAMEPAD_INPUT_MAX; i++)
  //   {
  //     Serial.print(gamepad_values[i]);
  //   }
  //   Serial.println();

  //   Serial.print("gamepad_changed, pressed keys: ");
  //   if (gamepad_values[GAMEPAD_INPUT_UP])
  //   {
  //     if (brightness_level < brightness_level_size - 1)
  //     {
  //       brightness_level++;
  //     }
  //     lcd_set_brightness(brightness_level);
  //     Serial.print("KEY_UP, ");
  //   }
  //   if (gamepad_values[GAMEPAD_INPUT_DOWN])
  //   {
  //     if (brightness_level > 0)
  //     {
  //       brightness_level--;
  //     }
  //     lcd_set_brightness(brightness_level);
  //     Serial.print("KEY_DOWN, ");
  //   }
  //   if (gamepad_values[GAMEPAD_INPUT_LEFT])
  //   {
  //     startNextSong(false);
  //     Serial.print("KEY_LEFT, ");
  //   }
  //   if (gamepad_values[GAMEPAD_INPUT_RIGHT])
  //   {
  //     startNextSong(true);
  //     Serial.print("KEY_RIGHT, ");
  //   }
  //   if (gamepad_values[GAMEPAD_INPUT_MID])
  //   {
  //     brightness_level++;
  //     brightness_level %= brightness_level_size;
  //     lcd_set_brightness(brightness_level);
  //     Serial.print("KEY_MID, ");
  //   }
  //   if (gamepad_values[GAMEPAD_INPUT_A])
  //   {
  //     volumeUp();
  //     Serial.print("KEY_A, ");
  //   }
  //   if (gamepad_values[GAMEPAD_INPUT_B])
  //   {
  //     volumeDown();
  //     Serial.println("KEY_B, ");
  //   }
  //   Serial.println();
  // }
}

long last_ui_update = 0;

void tryControlBySerialInput()
{
  if (Serial.available())
  { // put streamURL in serial monitor
    audio.stopSong();
    String r = Serial.readString();
    r.trim();
    if (r.length() > 5)
    {
      startNextSong(true);
      // audio.connecttohost(r.c_str());
    }
    log_i("free heap=%i", ESP.getFreeHeap());
  }
}

// void handle_keypad()
// {
//   while (customKeypad.available())
//   {
//     keypadEvent e = customKeypad.read();
//     Serial.print((char)e.bit.KEY);
//     if (e.bit.EVENT == KEY_JUST_PRESSED)
//     {
//       Serial.print(" pressed");
//       uint8_t row = e.bit.ROW;
//       uint8_t col = e.bit.COL;
//       Serial.print(" row: ");
//       Serial.print(row);
//       Serial.print(" col: ");
//       Serial.print(col);
//       Serial.println();
//       if (e.bit.KEY == 'W')
//       {
//         volumeUp();
//         drawScreen();
//       }
//       else if (e.bit.KEY == 'S')
//       {
//         volumeDown();
//         drawScreen();
//       }
//       else if (e.bit.KEY == 'A')
//       {
//         startNextSong(false);
//       }
//       else if (e.bit.KEY == 'D')
//       {
//         startNextSong(true);
//       }
//     }
//     else if (e.bit.EVENT == KEY_JUST_RELEASED)
//     {
//       Serial.println(" released");
//     }
//   }
// }

void loop_music_player()
{
  // loop_keypad();
  audio.loop();
  // handle_keypad();
  auto curr_time = millis();
  if (curr_time - last_ui_update > 1000)
  {
    // auto batteryValue = analogRead(VBAT_PIN);
    // vbat = batteryValue * 2 * 3.33 / 4095;
    // Serial.printf("adc=%ld, vbat=%.3lfV\n", batteryValue, vbat);
    double batteryValue = PMU.getBattVoltage();
    vbat = batteryValue / 1000.0; // mV -> V
    if (audio.isRunning())
    {
      auto audioDuration = audio.getAudioFileDuration();
      auto currPos = audio.getAudioCurrentTime();
      auto totalPlayingTime = audio.getTotalPlayingTime();
      // Serial.printf("AudioFileDuration=%d, AudioCurrentTime=%d, TotalPlayingTime=%d\n", audioDuration, currPos, totalPlayingTime);
      // drawText("", 10, 200, )
      printf("getBattVoltage:");
      printf("%d", PMU.getBattVoltage());
      printf("mV");
      printf("getVbusVoltage:");
      printf("%d", PMU.getVbusVoltage());
      printf("mV");
      printf("getSystemVoltage:");
      printf("%d", PMU.getSystemVoltage());
      printf("mV");

      last_ui_update = curr_time;
    }
  }

  autoPlayNextSong();
  tryControlBySerialInput();

  handleGamepadChange();
}

int strncmpci(const char *str1, const char *str2, size_t num)
{
  int ret_code = 0;
  size_t chars_compared = 0;

  if (!str1 || !str2)
  {
    ret_code = INT_MIN;
    return ret_code;
  }

  while ((chars_compared < num) && (*str1 || *str2))
  {
    ret_code = tolower((int)(*str1)) - tolower((int)(*str2));
    if (ret_code != 0)
    {
      break;
    }
    chars_compared++;
    str1++;
    str2++;
  }

  return ret_code;
}

bool startsWithIgnoreCase(const char *pre, const char *str)
{
  return strncmpci(pre, str, strlen(pre)) == 0;
}

bool endsWithIgnoreCase(const char *base, const char *str)
{
  int blen = strlen(base);
  int slen = strlen(str);
  return (blen >= slen) && (0 == strncmpci(base + blen - slen, str, strlen(str)));
}

// optional
void audio_info(const char *info)
{
  Serial.print("info        ");
  Serial.println(info);
}

void audio_id3data(const char *info)
{ // id3 metadata
  Serial.print("id3data     ");
  Serial.println(info);
  if (startsWithIgnoreCase("TITLE", info))
  {
    strncpy(music_title, info + sizeof("TITLE") + 1, sizeof(music_title) / sizeof(music_title[0]));
  }
  if (startsWithIgnoreCase("ALBUM", info))
  {
    strncpy(music_album, info + sizeof("ALBUM") + 1, sizeof(music_album) / sizeof(music_album[0]));
  }
  if (startsWithIgnoreCase("ARTIST", info))
  {
    strncpy(music_artist, info + sizeof("ARTIST") + 1, sizeof(music_artist) / sizeof(music_artist[0]));
  }
  drawScreen();
}

void audio_eof_mp3(const char *info)
{ // end of file
  Serial.print("eof_mp3     ");
  Serial.println(info);
}

void audio_showstation(const char *info)
{
  Serial.print("station     ");
  Serial.println(info);
}

void audio_showstreamtitle(const char *info)
{
  Serial.print("streamtitle ");
  Serial.println(info);
}

void audio_bitrate(const char *info)
{
  Serial.print("bitrate     ");
  Serial.println(info);
}

void audio_commercial(const char *info)
{ // duration in sec
  Serial.print("commercial  ");
  Serial.println(info);
}

void audio_icyurl(const char *info)
{ // homepage
  Serial.print("icyurl      ");
  Serial.println(info);
}

void audio_lasthost(const char *info)
{ // stream URL played
  Serial.print("lasthost    ");
  Serial.println(info);
}

void audio_eof_speech(const char *info)
{
  Serial.print("eof_speech  ");
  Serial.println(info);
}

// By experience
#define MAX_LOG_STRENGTH 5.2

#define NEW_SPECTRUM_DISPLAY_HEIGHT 100
#define NEW_SPECTRUM_DISPLAY_HISTORY 240

uint16_t new_fft_display_column_buff[NEW_SPECTRUM_DISPLAY_HEIGHT];
uint16_t new_fft_pos = NEW_SPECTRUM_DISPLAY_HISTORY;

void audio_process_extern(int16_t *buff, uint16_t len, bool *continueI2S)
{
  auto start = millis();
  *continueI2S = true;
  double max_value = 0;
  uint8_t channels = audio.getChannels();
  uint32_t rate = audio.getSampleRate();
  uint8_t bitsPerSample = audio.getBitsPerSample();
  if (len >= 1024)
  {
    // init cost 1249 us
    fft_config_t *real_fft_plan = fft_init(512, FFT_REAL, FFT_FORWARD, NULL, NULL);
    int16_t *buffptr = (int16_t *)buff;
    uint16_t count_n = 0;
    // size=512
    for (count_n = 0; count_n < real_fft_plan->size; count_n++)
    {
      float adc_data =
          (float)map(buffptr[count_n << 1], INT16_MIN, INT16_MAX, -1000, 1000);
      real_fft_plan->input[count_n] = adc_data;
    }
    fft_execute(real_fft_plan);
    // size / 4 = 128
    for (count_n = 1; count_n < real_fft_plan->size / 4; count_n++)
    {
      double dat = sqrt(real_fft_plan->output[2 * count_n] *
                            real_fft_plan->output[2 * count_n] +
                        real_fft_plan->output[2 * count_n + 1] *
                            real_fft_plan->output[2 * count_n + 1]);
      if ((count_n - 1) < NEW_SPECTRUM_DISPLAY_HEIGHT)
      {
        dat = log10(dat);
        max_value = max(max_value, dat);
        //        double ydata = mapToDouble(constrain(dat, 0, MAX_LOG_STRENGTH), 0, MAX_LOG_STRENGTH, 240, 420);
        uint16_t colorPos = mapToDouble(constrain(dat, 0, MAX_LOG_STRENGTH),
                                        0, MAX_LOG_STRENGTH,
                                        0, sizeof(spectrumColor) / sizeof(spectrumColor[0]) / 3 - 1);
        uint16_t color = RGB565(spectrumColor[colorPos * 3 + 0],
                                spectrumColor[colorPos * 3 + 1],
                                spectrumColor[colorPos * 3 + 2]);
        new_fft_display_column_buff[NEW_SPECTRUM_DISPLAY_HEIGHT - count_n] = color;
      }
    }
    new_fft_pos++;
    if (new_fft_pos >= NEW_SPECTRUM_DISPLAY_HISTORY)
    {
      new_fft_pos = 0;
    }

    // destroy cost 18 us
    fft_destroy(real_fft_plan);
    gfx->draw16bitRGBBitmap(new_fft_pos + (CANVAS_WIDTH - 240) / 2, 22, (uint16_t *)new_fft_display_column_buff, 1, NEW_SPECTRUM_DISPLAY_HEIGHT);
  }
  // auto btn = digitalRead(KEY_MID);
  // printf("ch=%d, rate=%d, bits=%d, len=%d, cost %ld ms, btn=%d\n", channels, rate, bitsPerSample, len, millis() - start, btn);
}
