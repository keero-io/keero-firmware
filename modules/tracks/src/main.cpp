#include <Arduino.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "light_control.h"
#include "motion_control.h"
#include "power_management.h"

static const char *TAG = "KEERO_TANK";

#define UART_ECHO_TXD (10)
#define UART_ECHO_RXD (18)

#define ECHO_UART_BAUD_RATE     (115200)
#define ECHO_TASK_STACK_SIZE    (4096)
#define BUF_SIZE                (1024)

static void echo_task(void *arg)
{
    (void)arg;

    Serial1.begin(ECHO_UART_BAUD_RATE, SERIAL_8N1, UART_ECHO_RXD, UART_ECHO_TXD);

    uint8_t *data = (uint8_t *)malloc(BUF_SIZE);
    if (data == NULL) {
        Serial.printf("[%s] Failed to allocate UART buffer\n", TAG);
        vTaskDelete(NULL);
        return;
    }

    int uart_read_count = 0;

    while (1) {
        int len = 0;
        int avail = Serial1.available();
        if (avail > 0) {
            len = avail;
            if (len > (BUF_SIZE - 1)) {
                len = BUF_SIZE - 1;
            }
            len = Serial1.readBytes(data, len);
            Serial1.write(data, len);
        }

        if (len > 0) {
            data[len] = '\0';

            char command;
            float value = 0.0f;
            float x_value = 0.0f;
            float y_value = 0.0f;

            if ((sscanf((const char *)data, "x%f y%f", &x_value, &y_value) == 2) ||
                (sscanf((const char *)data, "x%fy%f", &x_value, &y_value) == 2)) {
                uart_read_count = 0;
                Serial.printf("[%s] Parsed x: %.2f, y: %.2f\n", TAG, x_value, y_value);
                keero_motion_control(x_value, y_value);
            } else if (sscanf((const char *)data, "%c%f", &command, &value) == 2) {
                Serial.printf("[%s] Command: %c, Value: %.2f\n", TAG, command, value);
                if (command == 'w') {
                    rgb_ws2812_mode_set((light_mode_t)value);
                    set_power_current_light_mode((light_mode_t)value);
                } else if (command == 'c') {
                    set_motor_speed_coefficients(value);
                } else if (command == 'd') {
                    keero_dance();
                }
            } else {
                Serial.printf("[%s] Unknown command: %s\n", TAG, (char *)data);
            }
        }

        uart_read_count++;
        if (uart_read_count > 25) {
            uart_read_count = 0;
            keero_motion_control(0, 0);
        }

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}

static void app_main_logic(void)
{
    Serial.printf("[%s] Let's go!\n", TAG);
    motor_ledc_init();
    rgb_ws2812_init();
    battery_voltage_monitor_start();
    xTaskCreate(echo_task, "uart_echo_task", ECHO_TASK_STACK_SIZE, NULL, 10, NULL);
}

void setup()
{
    Serial.begin(115200);
    app_main_logic();
}

void loop()
{
    vTaskDelay(pdMS_TO_TICKS(1000));
}
