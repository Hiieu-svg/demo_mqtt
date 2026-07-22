#include <stdio.h>
#include "esp_log.h"
#include "mqtt_client.h"
#include "mqtt_app.h"

#define BROKER_URI "mqtt://broker.emqx.io:1883"

static const char *TAG = "MQTT_APP";
static esp_mqtt_client_handle_t client = NULL;

// 1. Hàm Handler hứng các sự kiện từ MQTT Broker
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "🟢 MQTT_EVENT_CONNECTED: Đã bắt tay thành công với EMQX Broker!");

        // 🔴 GỬI DỮ LIỆU TEST LÊN WEB MQTTX
    const char *topic = "demo_topic"; // Phải trùng với Topic trên Web MQTTX
    const char *payload = "{\"status\": \"OK\", \"msg\": \"ESP32 da gui tin nhan thanh cong!\"}";
    
    int msg_id = esp_mqtt_client_publish(client, topic, payload, 0, 1, 0);
    ESP_LOGI(TAG, "Đã gửi tin nhắn test thành công, msg_id=%d", msg_id);

        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "🔴 MQTT_EVENT_DISCONNECTED: Mất kết nối tới Broker! (SDK đang tự kết nối lại...)");
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "❌ MQTT_EVENT_ERROR: Gặp lỗi giao thức MQTT!");
        break;

    default:
        break;
    }
}

// 2. Khởi tạo cấu hình MQTT Client
void mqtt_app_init(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = BROKER_URI,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    // Đăng ký Event Handler xử lý sự kiện MQTT
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
}

// 3. Cho phép MQTT Client bắt đầu chạy
void mqtt_app_start(void)
{
    if (client != NULL) {
        ESP_LOGI(TAG, "Đang kết nối tới Broker: %s ...", BROKER_URI);
        esp_mqtt_client_start(client);
    }
}