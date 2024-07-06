#include <app.h>
#include "esp_err.h"

#include "esp_partition.h"
#include "esp_log.h"

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
