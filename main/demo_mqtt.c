#include <stdio.h>
#include "esp_log.h"
#include "wifi_app.h"
#include "mqtt_app.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "[APP] Khởi tạo hệ thống...");

    // 1. Khởi tạo cấu hình MQTT (chuẩn bị sẵn)
    mqtt_app_init();

    // 2. Chạy Wi-Fi (Khi có IP, Wi-Fi sẽ tự gọi mqtt_app_start)
    wifi_app_start();
}