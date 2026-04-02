#include <Arduino.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "light_control.h"
#include "power_management.h"

#define VBAT_ADC_PIN             (3)
#define VBAT_MONITOR_GPIO        (2)
#define VBAT_CHARGING_LEVEL      (0)
#define BATTERY_VOLTAGE_PERCENT  (20)
#define ADC_SMOOTH_WINDOW_SIZE   (10)

static light_mode_t power_current_light_mode = LIGHT_MODE_SLEEP;

static int battery_voltage_smoothing(int new_sample)
{
    static int battery_voltage_samples[ADC_SMOOTH_WINDOW_SIZE] = {0};
    static uint8_t battery_voltage_index = 0;
    static int battery_voltage_sum = 0;

    battery_voltage_sum -= battery_voltage_samples[battery_voltage_index];
    battery_voltage_samples[battery_voltage_index] = new_sample;
    battery_voltage_sum += new_sample;
    battery_voltage_index = (battery_voltage_index + 1) % ADC_SMOOTH_WINDOW_SIZE;

    return battery_voltage_sum / ADC_SMOOTH_WINDOW_SIZE;
}

static void vbat_monitor_task(void *arg)
{
    (void)arg;

    pinMode(VBAT_MONITOR_GPIO, INPUT_PULLUP);
    analogReadResolution(12);
    analogSetPinAttenuation(VBAT_ADC_PIN, ADC_11db);

    int last_vbat_charging_state = !VBAT_CHARGING_LEVEL;
    light_mode_t charging_light_mode_last = LIGHT_MODE_MAX;

    while (1) {
        int battery_voltage = analogReadMilliVolts(VBAT_ADC_PIN) * 2;
        battery_voltage = battery_voltage_smoothing(battery_voltage);

        float voltage_percent = ((battery_voltage - 3300) / (4200.0f - 3300.0f)) * 100.0f;
        int battery_voltage_percent = constrain((int)voltage_percent, 0, 100);

        int vbat_charging_state = digitalRead(VBAT_MONITOR_GPIO);

        if (vbat_charging_state != last_vbat_charging_state) {
            last_vbat_charging_state = vbat_charging_state;
            if (vbat_charging_state == VBAT_CHARGING_LEVEL) {
                Serial.println("[power] Battery charging");
            } else {
                Serial.println("[power] Battery not charging");
            }
        }

        light_mode_t charging_light_mode = LIGHT_MODE_WHITE_BREATH_SLOW;
        if (vbat_charging_state == VBAT_CHARGING_LEVEL) {
            charging_light_mode = LIGHT_MODE_CHARGING_BREATH;
        } else if (battery_voltage_percent <= BATTERY_VOLTAGE_PERCENT) {
            charging_light_mode = LIGHT_MODE_POWER_LOW;
        } else {
            charging_light_mode = power_current_light_mode;
        }

        if (charging_light_mode != charging_light_mode_last) {
            rgb_ws2812_mode_set(charging_light_mode);
            charging_light_mode_last = charging_light_mode;
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

void set_power_current_light_mode(light_mode_t mode)
{
    power_current_light_mode = mode;
}

void battery_voltage_monitor_start(void)
{
    xTaskCreate(vbat_monitor_task, "vbat_monitor_task", 3072, NULL, 5, NULL);
}
