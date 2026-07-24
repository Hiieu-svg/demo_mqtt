#include <stdio.h>
#include "esp_log.h"
#include "wifi_app.h"
#include "mqtt_app.h"

static const char *TAG = "MAIN_FUNCTION";

void app_main(void)
{
    ESP_LOGI(TAG, " Khởi tạo hệ thống...");

    // 1. Khởi tạo cấu hình MQTT
    mqtt_app_init();

    // 2. Chạy Wi-Fi 
    wifi_app_start();
}