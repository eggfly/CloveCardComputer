//**********************************************************************************************************
//*    audioI2S-- I2S audiodecoder for ESP32,                                                              *
//**********************************************************************************************************
//
// first release on 11/2018
// Version 3  , Jul.02/2020
//
//
// THE SOFTWARE IS PROVIDED "AS IS" FOR PRIVATE USE ONLY, IT IS NOT FOR COMMERCIAL USE IN WHOLE OR PART OR CONCEPT.
// FOR PERSONAL USE IT IS SUPPLIED WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHOR
// OR COPYRIGHT HOLDER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE
//

#include "Arduino.h"
#include "WiFiMulti.h"
#include "Audio.h"
#include "SPI.h"
#include "SD.h"
#include "FS.h"

// Digital I/O used
#define SD_CS    42
#define SPI_MOSI 41
#define SPI_MISO 39
#define SPI_SCK  40

#define I2S_DOUT  9
#define I2S_BCLK 47
#define I2S_LRC  48

Audio audio;
WiFiMulti wifiMulti;
String ssid = "MIWIFI8";
String password = "12345678";

void setup_i2s()
{
    // GPIO46 -> SD_MODE, ESP32-S3 的内部上拉电阻范围为几万欧姆(几十K)
    // pinMode(46, OUTPUT);
    // digitalWrite(46, HIGH);

    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    SPI.setFrequency(1000000);
    // Serial.begin(115200);
    SD.begin(SD_CS);
    WiFi.mode(WIFI_STA);
    wifiMulti.addAP(ssid.c_str(), password.c_str());
    wifiMulti.run();
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.disconnect(true);
        wifiMulti.run();
    }
    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(8); // 0...21
    // const char * url = "http://42.193.120.65:8002/ygzjhyl.mp3";
    const char * url = "http://42.193.120.65:8002/520AM.mp3";
    audio.connecttohost(url);
}

void loop_i2s()
{
    audio.loop();
    if (Serial.available())
    { // put streamURL in serial monitor
        audio.stopSong();
        String r = Serial.readString();
        r.trim();
        if (r.length() > 5)
            audio.connecttohost(r.c_str());
        log_i("free heap=%i", ESP.getFreeHeap());
    }
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
