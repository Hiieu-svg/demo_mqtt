#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "mqtt_client.h"
#include "mqtt_app.h"

// 🔑 1. ĐIỀN THÔNG TIN HIVEMQ CLOUD CỦA BẠN VÀO ĐÂY
#define HIVEMQ_HOST "4a1375dbae154624a6759636e2559329.s1.eu.hivemq.cloud" // Thay bằng Host của bạn (không chứa mqtts://)
#define HIVEMQ_USER "hivemq.webclient.1784773891069"                   // Username tạo trên Web HiveMQ
#define HIVEMQ_PASS "l8PwDW6lbyH4JCi5RIXTBi4ruH*noZrU"                  // Password tạo trên Web HiveMQ

// 🔐 2. Nhúng chứng chỉ SSL từ file hivemq_ca.pem vào code C
extern const uint8_t hivemq_ca_pem_start[] asm("_binary_hivemq_ca_pem_start");
extern const uint8_t hivemq_ca_pem_end[]   asm("_binary_hivemq_ca_pem_end");

static const char *TAG = "MQTT_HIVEMQ";
static esp_mqtt_client_handle_t client = NULL;

// 3. Hàm xử lý các sự kiện từ HiveMQ Broker
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "🟢 Đã kết nối THÀNH CÔNG tới HiveMQ Cloud (Port 8883 - SSL)!");

        // --- A. SUBSCRIBE (Đăng ký nhận lệnh từ Web Client) ---
        // Đăng ký channel "esp32/control" để chờ web gửi lệnh ON/OFF
        int sub_id = esp_mqtt_client_subscribe(client, "esp32/control", 1);
        ESP_LOGI(TAG, "Đã Subscribe topic 'esp32/control', sub_id=%d", sub_id);

        // --- B. PUBLISH (Gửi 1 tin nhắn chào mừng lên HiveMQ) ---
        const char *topic = "esp32/data";
        const char *payload = "{\"device\": \"ESP32\", \"status\": \"CONNECTED TO HIVEMQ!\"}";
        int msg_id = esp_mqtt_client_publish(client, topic, payload, 0, 1, 0);
        ESP_LOGI(TAG, "Đã gửi dữ liệu lên topic 'esp32/data', msg_id=%d", msg_id);

        break;

    case MQTT_EVENT_DATA:
        // --- C. HỨNG DỮ LIỆU NHẬN ĐƯỢC TỪ WEB CLIENT ---
        ESP_LOGI(TAG, "📩 NHẬN ĐƯỢC TIN NHẮN MỚI TỪ HIVEMQ!");
        ESP_LOGI(TAG, "TOPIC: %.*s", event->topic_len, event->topic);
        ESP_LOGI(TAG, "DATA : %.*s", event->data_len, event->data);

        // Kiểm tra nếu web gửi lệnh "ON" hoặc "OFF"
        if (strncmp(event->data, "ON", event->data_len) == 0) {
            ESP_LOGW(TAG, "⚡ LỆNH NHẬN ĐƯỢC: BẬT ĐÈN!");
        } else if (strncmp(event->data, "OFF", event->data_len) == 0) {
            ESP_LOGW(TAG, "⚡ LỆNH NHẬN ĐƯỢC: TẮT ĐÈN!");
        }
        break;

    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGW(TAG, "🔴 Mất kết nối tới HiveMQ! (Đang tự kết nối lại...)");
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGE(TAG, "❌ Lỗi giao thức/Xác thực SSL với HiveMQ!");
        break;

    default:
        break;
    }
}

// 4. Khởi tạo cấu hình kết nối HiveMQ
void mqtt_app_init(void)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        // Cú pháp đường dẫn: mqtts://<HOST>:8883
        .broker.address.uri = "mqtts://" HIVEMQ_HOST ":8883",
        
        // Cấu hình Username / Password
        .credentials.username = HIVEMQ_USER,
        .credentials.authentication.password = HIVEMQ_PASS,
        
        // Cấu hình Chứng chỉ SSL/TLS
        .broker.verification.certificate = (const char *)hivemq_ca_pem_start,
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
}

// 5. Kích hoạt kết nối
void mqtt_app_start(void)
{
    if (client != NULL) {
        ESP_LOGI(TAG, "Đang kết nối tới HiveMQ Cloud: %s ...", HIVEMQ_HOST);
        esp_mqtt_client_start(client);
    }
}