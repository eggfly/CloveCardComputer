
#include "rm67162.h"
#include <TFT_eSPI.h> //https://github.com/Bodmer/TFT_eSPI
#include "true_color.h"

#include "esp_err.h"

#include "esp_partition.h"
#include "esp_log.h"

#include <Arduino.h>
#include <app.h>
#include <clove_pin_config.h>

static const char *TAG = "main";

void mmap_font_partition()
{
  const esp_partition_t *partition = find_partition(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "font2");
  if (partition)
  {
    printf("Partition subtype=%x, size=%d\n", partition->subtype, partition->size);
    spi_flash_mmap_handle_t map_handle;
    // Map the partition to data memory
    ESP_ERROR_CHECK(esp_partition_mmap(partition, 0, partition->size, SPI_FLASH_MMAP_DATA, &font_map_ptr, &map_handle));
    ESP_LOGI(TAG, "Mapped partition to data memory address %p", font_map_ptr);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.setTxTimeoutMs(0);
  delay(10);
  printf("Serial.begin() called.\n");
  mmap_font_partition();
  setup_pmu();
  setup_encoder();

  // setup_spm1423();
  setup_pcf8574();
  pcf.digitalWrite(PCF8574_TP_RST, HIGH);
  pcf.digitalWrite(PCF8574_TP_RST, LOW);
  pcf.digitalWrite(PCF8574_TP_RST, HIGH);

  pcf.digitalWrite(PCF8574_LCD_RST, HIGH);
  pcf.digitalWrite(PCF8574_LCD_RST, LOW);
  pcf.digitalWrite(PCF8574_LCD_RST, HIGH);

  setup_ft3168();
  setup_amoled();
  setup_music_player();
  pinMode(0, INPUT_PULLUP);
  // test_amoled();
}

bool isFM = false;
bool fmInitialized = false;

#define VDD_FM_VOLTAGE 3300

void loop_radio_tuner() {
  static int32_t prev_encoder_count = 0;
  int32_t count = static_cast<int32_t>(encoder.getCount() / 2);
  if (count != prev_encoder_count)
  {
    int32_t diff = count - prev_encoder_count;
    // Serial.printf("count=%d, diff=%d\n", count, diff);
    prev_encoder_count = count;
    auto fMin = radio.getMinFrequency();
    auto fMax = radio.getMaxFrequency();
    auto step = radio.getFrequencyStep();
    auto fCurr = radio.getFrequency();
    auto fNew = fCurr + diff * step;
    fNew = constrain(fNew, fMin, fMax);
    if (fNew != fCurr)
    {
      radio.setFrequency(fNew);
    }
    Serial.printf("count=%d, freq=%.1fMHz, new=%.1fMHz\n", count, fCurr / 100.0, fNew / 100.0);
  }
}

void loop(void)
{
  // loop_pmu();
  loop_pcf8574();
  // Serial.println("loop...");
  loop_ft3168();
  // loop_touch_amoled();
  // loop_spm1423();
  auto bootPinState = digitalRead(0);
  if (bootPinState == LOW)
  {
    isFM = !isFM;
    Serial.println(isFM ? "Switch to FM" : "Switch to Music Player");
  }
  if (isFM)
  {
    if (!fmInitialized)
    {
      fmInitialized = true;
      
      PMU.setALDO4Voltage(VDD_FM_VOLTAGE);
      PMU.enableALDO4();

      // Because BLDO2 Input is DC3, Set The Voltage to 3.3V
      PMU.setDC3Voltage(3300);
      // AUDIO_SEL as FM Input
      PMU.enableBLDO2();

      setup_rda5807m();
    }
    loop_rda5807m();
    loop_radio_tuner();
    loop_radio_ui();
  }
  else
  {
    loop_music_player();
  }
}
