#include <app.h>
void loop_radio_ui()
{
    gfx->fillScreen(TFT_BLACK);
    gfx->setCursor(0, 0);
    gfx->setTextSize(5);
    gfx->println("Radio UI");
    auto fCurr = radio.getFrequency();
    gfx->printf("FM %.1f MHz\n", fCurr / 100.0);
    flush_screen();
}
