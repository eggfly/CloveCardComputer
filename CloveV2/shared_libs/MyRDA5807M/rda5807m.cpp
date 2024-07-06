///
/// \file ScanRadio.ino
/// \brief This sketch implements a FM scanner that lists all availabe radio stations including some information.
///
/// \author Matthias Hertel, http://www.mathertel.de
/// \copyright Copyright (c) by Matthias Hertel.\n
/// This work is licensed under a BSD 3-Clause license.\n
/// See http://www.mathertel.de/License.aspx
///
/// \details
/// This is a Arduino sketch radio implementation that can be controlled using commands on the Serial input.
/// There are some experimental scan algorythms implemented that uses a state machine to scan through all radio stations
/// the radio chip can detect and outputs them on the Serial interface.
/// Open the Serial console with 115200 baud to see current radio information and change various settings.
///
/// Wiring
/// ------
/// The necessary wiring of the various chips are described in the Testxxx example sketches.
/// No additional components are required because all is done through the serial interface.
///
/// More documentation is available at http://www.mathertel.de/Arduino
/// Source Code is available on https://github.com/mathertel/Radio
///
/// History:
/// --------
/// * 17.05.2015 created.
/// * 27.05.2015 first version is working (beta with SI4705).
/// * 04.07.2015 2 scan algorithms working with good results with SI4705.
/// * 18.09.2020 more RDS output, better command handling.

#include <Arduino.h>
#include <Wire.h>
#include <radio.h>

#include <RDA5807M.h>
#include <RDSParser.h>

// 87.00
// 87.5 音乐
// 88.30 ?
// 88.70 HitFM
// 90.5 环球资讯
// 北京新闻广播 FM94.5 -> 信号最强
// FM97.4 北京音乐广播
// 9960
// 102.5 北京体育广播
// 10390 北京交通广播
// 104.8
// 105.20 河北人民广播电台
// 106.10 带 RDS 央广中国之声
// 107.30 北京城市广播副中心之声
// 100.60 京津冀之声 MONO

/*
   北京电台,
    北京新闻广播    FM100.6     北京地区权威新闻资讯台
    北京交通广播    FM103.9          北京地区最受欢迎电台
    北京文艺广播    FM87.6           北京地区京腔京韵最浓电台
    北京体育广播    FM102.5          专业体育广播
    北京音乐广播    FM97.4           全国最专业音乐电台,别于音乐之声的
    北京城市管理广播 FM107.3   北京市长王歧山一手创办,原北京经济广播

  中央电台,
    中国之声       FM106.1           全国范围的权威新闻资讯台
    音乐之声       FM90.0            全国范围的流行音乐电台
    都市之声       FM101.8           中央电台下属唯一专门面对北京市场电台
    经济之声       FM96.6            全国范围专业财经资讯电台
    文艺之声       FM106.6          以“语言文艺”节目服务于社会、“讲故事”为特色的类型化专业电台
    中华之声、神州之声、华夏之声、民族之声，北京地区不能收听，在此不介绍了。

  国际电台下属三个电台,
    轻松调频(EASY FM) FM91.5      音乐+英语新闻,早几年很流行这个台的JOYFM
    金曲调频(HIT  FM) FM88.7      个人最喜欢的流行音乐电台,主要是欧美音乐
    环球资讯       FM90.5             新创办的中文国际新闻资讯台


    在北京还能收到河北的电台，比如河北省台和廊坊电台,太非主流。一般在郊区能收到
  　河北地区调频广播（FM-DX）
  　　河北经济广播电台　　　90.80　　立体声
  　　河北交通台　　　　　　91.90
  　　河北承德人民广播电台　93.80　　立体声
  　　河北文艺广播电台　　　94.80　　立体声
  　　河北廊坊人民广播电台　95.10　　立体声　 （旅游时尚广播）
  这两个比较容易收到
  　　河北廊坊人民广播电台交通长书频道　　100.30　　立体声
  　　河北人民广播电台　　　105.20　　单声道
  ————————————————

                            版权声明：本文为博主原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接和本声明。

  原文链接：https://blog.csdn.net/zhiyuan411/article/details/4871888
*/
/// The radio object has to be defined by using the class corresponding to the used chip.
/// by uncommenting the right radio object definition.

/// Create the radio instance that fits the current chip:
RDA5807M radio;  ///< Create an instance of a RDA5807 chip radio

/// get a RDS parser
RDSParser rds;

const auto INITIAL_FREQUENCY = 9450;
const auto INITIAL_VOLUME = 0; // 0 -> Still have a little sound.

/// State of Keyboard input for this radio implementation.
enum RADIO_STATE {
  STATE_PARSECOMMAND,  ///< waiting for a new command character.
  STATE_PARSEINT,      ///< waiting for digits for the parameter.
  STATE_EXEC           ///< executing the command.
};

RADIO_STATE kbState;  ///< The state of parsing input characters.
char kbCommand;
int16_t kbValue;

uint16_t g_block1;
bool radioDebug = false;
bool lowLevelDebug = false;
String lastServiceName;

// - - - - - - - - - - - - - - - - - - - - - - - - - -

// use a function in between the radio chip and the RDS parser
// to catch the block1 value (used for sender identification)
void RDS_process(uint16_t block1, uint16_t block2, uint16_t block3, uint16_t block4) {
  Serial.printf("RDS: 0x%04x 0x%04x 0x%04x 0x%04x\n", block1, block2, block3, block4);
  g_block1 = block1;
  rds.processData(block1, block2, block3, block4);
}

/// Update the Time
void DisplayTime(uint8_t hour, uint8_t minute) {
  Serial.print("Time: ");
  if (hour < 10)
    Serial.print('0');
  Serial.print(hour);
  Serial.print(':');
  if (minute < 10)
    Serial.print('0');
  Serial.println(minute);
}  // DisplayTime()


/// Update the ServiceName text on the LCD display.
void DisplayServiceName(const char *name) {
  bool found = false;

  for (uint8_t n = 0; n < 8; n++)
    if (name[n] != ' ')
      found = true;

  if (found) {
    lastServiceName = name;
    Serial.print("Sender:<");
    Serial.print(name);
    Serial.println('>');
  }
}  // DisplayServiceName()


/// Update the ServiceName text on the LCD display.
void DisplayText(const char *txt) {
  Serial.print("Text: <");
  Serial.print(txt);
  Serial.println('>');
}  // DisplayText()


void PrintScanInfo(RADIO_INFO *ri) {
  char sFreq[12];
  radio.formatFrequency(sFreq, sizeof(sFreq));
  Serial.print(sFreq);
  Serial.print(' ');

  Serial.print(ri->rssi);
  Serial.print(' ');
  // Serial.print(ri->snr);
  // Serial.print(' ');
  Serial.print(ri->tuned ? 'T' : '-');
  Serial.print(ri->stereo ? 'S' : '-');
  Serial.print(ri->rds ? 'R' : '-');
  Serial.println();
}

/// Execute a command identified by a character and an optional number.
/// See the "?" command for available commands.
/// \param cmd The command character.
/// \param value An optional parameter for the command.
void runSerialCommand(char cmd, int16_t value) {
  unsigned long startSeek;  // after 300 msec must be tuned. after 500 msec must have RDS.
  RADIO_FREQ fSave, fLast = 0;
  RADIO_FREQ f = radio.getMinFrequency();
  RADIO_FREQ fMax = radio.getMaxFrequency();
  RADIO_INFO ri;

  if ((cmd == '\n') || (cmd == '\r')) {
    return;
  }

  Serial.print("do:");
  Serial.println(cmd);

  if (cmd == '?') {
    Serial.println();
    Serial.println("? Help");
    Serial.println("+ increase volume");
    Serial.println("- decrease volume");
    Serial.println("1 scan Frequency + Data");
    Serial.println("2 scan version 2");
    Serial.println("3 scan RDS stations");
    Serial.println(". scan up   : scan up to next sender");
    Serial.println(", scan down ; scan down to next sender");
    Serial.println("fnnnnn: direct frequency input");
    Serial.println("i station status");
    Serial.println("s mono/stereo mode");
    Serial.println("b bass boost");
    Serial.println("m mute/unmute");
    Serial.println("u soft mute/unmute");
    Serial.println("x debug...");
    Serial.println("y toggle Debug Messages...");
    Serial.println("* toggle i2c debug output");

    // ----- control the volume and audio output -----

  } else if (cmd == '+') {
    // increase volume
    int v = radio.getVolume();
    if (v < 15)
      radio.setVolume(++v);
  } else if (cmd == '-') {
    // decrease volume
    int v = radio.getVolume();
    if (v > 0)
      radio.setVolume(--v);

  } else if (cmd == 'm') {
    // toggle mute mode
    radio.setMute(!radio.getMute());

  } else if (cmd == 'u') {
    // toggle soft mute mode
    radio.setSoftMute(!radio.getSoftMute());

  } else if (cmd == 's') {
    // toggle stereo mode
    radio.setMono(!radio.getMono());

  } else if (cmd == 'b') {
    // toggle bass boost
    radio.setBassBoost(!radio.getBassBoost());


  } else if (cmd == '1') {
    // ----- control the frequency -----
    Serial.println("Scanning all available frequencies... (1)");
    fSave = radio.getFrequency();

    // start Simple Scan: all channels
    while (f <= fMax) {
      radio.setFrequency(f);
      delay(100);

      radio.getRadioInfo(&ri);

      // you may adjust the following condition to adjust to the chip you use.
      // some do not report snr or tuned and good rssi differs.

      if (true) {  // print all frequencies
        // if (ri.stereo) {  // print all stereo frequencies
        // if (ri.rssi >= 32) {  // si4703 usable threshold value

        // PrintScanInfo(&ri);

        RADIO_FREQ f = radio.getFrequency();
        Serial.print(f);
        Serial.print(',');
        Serial.print(ri.stereo);
        Serial.print(',');
        Serial.print(ri.rds);
        Serial.print(',');
        Serial.println(ri.rssi);
      }

      // tune up by 1 step
      f += radio.getFrequencyStep();
    }  // while
    Serial.println("done.");
    radio.setFrequency(fSave);

  } else if (cmd == '2') {
    Serial.println("Seeking all frequencies... (2)");
    fSave = radio.getFrequency();

    // start Scan
    radio.setFrequency(f);

    while (f <= fMax) {
      radio.seekUp(true);
      delay(100);  //
      startSeek = millis();

      // wait for seek complete
      do {
        radio.getRadioInfo(&ri);
      } while ((!ri.tuned) && (startSeek + 600 > millis()));

      // check frequency
      f = radio.getFrequency();
      if (f < fLast) {
        break;
      }
      fLast = f;

      // you may adjust the following condition to adjust to the chip you use.
      // some do not report snr or tuned and good rssi differs.

      if (true) {  // every frequency detected by builtin scan
        // if ((ri.tuned) && (ri.rssi > 34) && (ri.snr > 12)) {
        // if (ri.rssi >= 32) {  // si4703 threshold value

        radio.getRadioInfo(&ri);
        PrintScanInfo(&ri);
      }  // if
    }    // while
    Serial.println("done.");
    radio.setFrequency(fSave);


  } else if (cmd == '3') {
    Serial.println("Seeking all RDS senders...");
    fSave = radio.getFrequency();

    // start Scan
    radio.setFrequency(f);

    // start Simple Scan: all channels
    while (f <= fMax) {
      radio.setFrequency(f);
      lastServiceName.clear();
      delay(100);

      radio.getRadioInfo(&ri);

      // you may adjust the following condition to adjust to the chip you use.
      // some do not report snr or tuned and good rssi differs.

      // if (true) {  // print all frequencies
      // if (ri.stereo) {  // print all stereo frequencies

      if (ri.rssi >= 32) {  // si4703 usable threshold value
        if (ri.rds) {
          radio.checkRDS();
          PrintScanInfo(&ri);
          startSeek = millis();

          // wait 3 secs for sender name
          do {
            radio.checkRDS();
            delay(30);
          } while (lastServiceName.isEmpty() && (startSeek + 6000 > millis()));
        }  // if
      }

      // tune up by 1 step
      f += radio.getFrequencyStep();
    }  // while
    Serial.println("done.");
    radio.setFrequency(fSave);
  } else if (cmd == 'f') {
    radio.setFrequency(value);
  }

  else if (cmd == '.') {
    radio.seekUp(false);
  } else if (cmd == ':') {
    radio.seekUp(true);
  } else if (cmd == ',') {
    radio.seekDown(false);
  } else if (cmd == ';') {
    radio.seekDown(true);

  } else if (cmd == '!') {
    // not in help
    RADIO_FREQ f = radio.getFrequency();
    if (value == 0) {
      radio.term();
    } else if (value == 1) {
      radio.init();
      radio.setBandFrequency(RADIO_BAND_FM, f);
    }
  } else if (cmd == 'i') {
    // info
    char s[12];
    radio.formatFrequency(s, sizeof(s));
    Serial.print("Station:");
    Serial.println(s);
    Serial.print("Radio:");
    radio.debugRadioInfo();
    Serial.print("Audio:");
    radio.debugAudioInfo();
  } else if (cmd == 'x') {
    radio.debugStatus();  // print chip specific data.
  } else if (cmd == 'y') {
    radioDebug = !radioDebug;
    radio.debugEnable(radioDebug);
  } else if (cmd == '*') {
    lowLevelDebug = !lowLevelDebug;
    radio._wireDebug(lowLevelDebug);
  }
}  // runSerialCommand()


/// Setup a FM only radio configuration with I/O for commands and debugging on the Serial port.
void setup_rda5807m() {
  // open the Serial port
  Serial.println("ScanRadio...");
  delay(200);

  // Enable information to the Serial port
  radio.debugEnable(radioDebug);
  radio._wireDebug(lowLevelDebug);

  // Set FM Options for Europe
  radio.setup(RADIO_FMSPACING, RADIO_FMSPACING_100);   // for EUROPE
  radio.setup(RADIO_DEEMPHASIS, RADIO_DEEMPHASIS_50);  // for EUROPE
  // radio.setBand(RADIO_BAND_FMWORLD);

  // Initialize the Radio
  if (!radio.initWire(Wire)) {
    Serial.println("no radio chip found.");
    delay(4000);
    while (1) {};
  };

  radio.setBandFrequency(RADIO_BAND_FM, INITIAL_FREQUENCY);

  radio.setMono(false);
  radio.setMute(false);
  radio.setVolume(INITIAL_VOLUME);

  // setup the information chain for RDS data.
  radio.attachReceiveRDS(RDS_process);
  rds.attachServiceNameCallback(DisplayServiceName);
  rds.attachTextCallback(DisplayText);
  rds.attachTimeCallback(DisplayTime);

  runSerialCommand('?', 0);
  kbState = STATE_PARSECOMMAND;
}  // Setup


/// Constantly check for serial input commands and trigger command execution.
void loop_rda5807m() {
  if (Serial.available() > 0) {
    // read the next char from input.
    char c = Serial.peek();

    if ((kbState == STATE_PARSECOMMAND) && (c < 0x20)) {
      // ignore unprintable chars
      Serial.read();

    } else if (kbState == STATE_PARSECOMMAND) {
      // read a command.
      kbCommand = Serial.read();
      kbState = STATE_PARSEINT;

    } else if (kbState == STATE_PARSEINT) {
      if ((c >= '0') && (c <= '9')) {
        // build up the value.
        c = Serial.read();
        kbValue = (kbValue * 10) + (c - '0');
      } else {
        // not a value -> execute
        runSerialCommand(kbCommand, kbValue);
        kbCommand = ' ';
        kbState = STATE_PARSECOMMAND;
        kbValue = 0;
      }  // if
    }    // if
  }      // if

  // check for RDS data
  radio.checkRDS();


}  // loop

// End.
