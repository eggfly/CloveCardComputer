
#include "rm67162.h"
#include <TFT_eSPI.h> //https://github.com/Bodmer/TFT_eSPI
#include "true_color.h"

#include "esp_err.h"

#include "esp_partition.h"
#include "esp_log.h"

#include <Arduino.h>
#include <app.h>

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
  delay(1000);
  delay(100);
  printf("Serial.begin() called.\n");
  mmap_font_partition();
  setup_pmu();
  // setup_aw9523();
  // setup_keypad();
  // setup_spm1423();
  setup_amoled();
  setup_music_player();
}

void loop(void)
{
  // loop_pmu();
  // loop_aw9523();
  loop_keypad();
  // Serial.println("loop...");
  // loop_amoled();
  // loop_spm1423();
  loop_music_player();
}
