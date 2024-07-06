#include <app.h>
#include "FM_536x240.h"

const auto LINE_WIDTH = 3;
const auto LINE_TOP = 70;
const auto LINE_BOTTOM = 240 - 40;
const auto FREQ_TEXT_LEFT = 245;
const auto FREQ_TEXT_TOP = 19;
const auto MIN_FREQ_LEFT_X = 205;
const auto MAX_FREQ_RIGHT_X = 536 - 54;
const auto MIN_FREQ = 8700;
const auto MAX_FREQ = 10800;
const auto SAVED_FREQ_DOT_Y = 82;
const auto SAVED_FREQ_DOT_R = 2;
const auto SAVED_FREQ_DOT_COLOR = RGB565(0xdc, 0x28, 0x18);
const auto INFO_SMALL_TEXT_TOP = 13;
const auto INFO_MARGIN_RIGHT = 20;
const auto INFO_LINE_GAP = 1;

const uint32_t beijing_fm_stations[] = {
    8870,  // Hit FM
    9050,  // 环球资讯广播
    9450,  // 北京新闻广播
    9740,  // 北京音乐广播
    9960,  // 北京交通广播
    10060, // 京津冀之声
    10390, // 北京交通广播
    10520, // 河北人民广播电台
    10610, // 带 RDS 央广中国之声
};

void loop_radio_ui()
{
    uint16_t freq;
    if (tp_fingers_count > 0)
    {
        freq = map(tp_x1, MIN_FREQ_LEFT_X, MAX_FREQ_RIGHT_X, MIN_FREQ, MAX_FREQ);
        freq = constrain(freq, MIN_FREQ, MAX_FREQ);
        // Round to 10
        freq = (freq + 5) / 10 * 10;
        radio.setFrequency(freq);
    }
    else
    {
        freq = radio.getFrequency();
    }
    gfx->draw16bitRGBBitmap(0, 0, (uint16_t *)FM_536x240_map, 536, 240);
    gfx->setTextColor(RGB565(0xad, 0x92, 0x5a));
    gfx->setCursor(FREQ_TEXT_LEFT, FREQ_TEXT_TOP);
    gfx->setTextSize(3);
    gfx->printf("%.1f MHz\n", freq / 100.0);
    int32_t tuner_line_x = map(freq, MIN_FREQ, MAX_FREQ, MIN_FREQ_LEFT_X, MAX_FREQ_RIGHT_X);
    gfx->fillRect(tuner_line_x - LINE_WIDTH / 2, LINE_TOP, LINE_WIDTH, LINE_BOTTOM - LINE_TOP, RGB565(0x29, 0x29, 0x29));
    gfx->drawRect(tuner_line_x - LINE_WIDTH / 2 - 1, LINE_TOP - 1, LINE_WIDTH + 2, LINE_BOTTOM - LINE_TOP + 2, RGB565(0x73, 0x6d, 0x63));
    gfx->drawRect(tuner_line_x - LINE_WIDTH / 2 - 2, LINE_TOP - 2, LINE_WIDTH + 4, LINE_BOTTOM - LINE_TOP + 4, RGB565(0x08, 0x08, 0x08));
    for (auto station : beijing_fm_stations)
    {
        auto x = map(station, MIN_FREQ, MAX_FREQ, MIN_FREQ_LEFT_X, MAX_FREQ_RIGHT_X);
        gfx->fillCircle(x, SAVED_FREQ_DOT_Y, SAVED_FREQ_DOT_R, SAVED_FREQ_DOT_COLOR);
    }
    RADIO_INFO info;
    radio.getRadioInfo(&info);
    char info_str[32];
    sprintf(info_str, "RSSI: %d", info.rssi);
    typedef struct
    {
        int16_t x;
        int16_t y;
        uint16_t w;
        uint16_t h;
    } TextBounds;
    TextBounds bounds;
    gfx->setTextSize(2);
    gfx->getTextBounds(info_str, 0, 0, &bounds.x, &bounds.y, &bounds.w, &bounds.h);
    gfx->setCursor(536 - INFO_MARGIN_RIGHT - bounds.w, INFO_SMALL_TEXT_TOP);
    gfx->println(info_str);
    sprintf(info_str, "Tuned: %c", info.tuned ? 'Y' : 'N');
    gfx->getTextBounds(info_str, 0, 0, &bounds.x, &bounds.y, &bounds.w, &bounds.h);
    gfx->setCursor(536 - INFO_MARGIN_RIGHT - bounds.w, INFO_SMALL_TEXT_TOP + bounds.h + INFO_LINE_GAP);
    gfx->println(info_str);
    flush_screen();
}
