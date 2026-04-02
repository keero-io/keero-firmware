#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#if !defined(CONFIG_IDF_TARGET_ESP32C2)
#include "Adafruit_NeoPixel.h"
#endif
#include "light_control.h"

#if !defined(CONFIG_IDF_TARGET_ESP32C2)
static Adafruit_NeoPixel s_strip(WS2812_STRIPS_NUM, WS2812_GPIO_NUM, NEO_GRB + NEO_KHZ800);
static volatile light_mode_t s_current_mode = LIGHT_MODE_SLEEP;

static uint32_t white_color(uint8_t b)
{
    return s_strip.Color(b, b, b);
}

static void fill_all(uint32_t color)
{
    for (int i = 0; i < WS2812_STRIPS_NUM; ++i) {
        s_strip.setPixelColor(i, color);
    }
    s_strip.show();
}

static void led_anim_task(void *arg)
{
    (void)arg;
    bool blink_state = false;
    uint8_t breathe = 0;
    int breathe_step = 4;
    uint16_t hue = 0;
    uint8_t chase_index = 0;

    while (1) {
        switch (s_current_mode) {
        case LIGHT_MODE_CHARGING_BREATH:
        case LIGHT_MODE_WHITE_BREATH_SLOW:
            breathe = (uint8_t) (breathe + breathe_step);
            if (breathe == 0 || breathe == 255) {
                breathe_step = -breathe_step;
            }
            fill_all(white_color(breathe));
            vTaskDelay(pdMS_TO_TICKS(20));
            break;
        case LIGHT_MODE_WHITE_BREATH_FAST:
            breathe = (uint8_t) (breathe + breathe_step);
            if (breathe == 0 || breathe == 255) {
                breathe_step = -breathe_step;
            }
            fill_all(white_color(breathe));
            vTaskDelay(pdMS_TO_TICKS(8));
            break;
        case LIGHT_MODE_ALWAYS_ON:
            fill_all(white_color(127));
            vTaskDelay(pdMS_TO_TICKS(120));
            break;
        case LIGHT_MODE_BLINK:
            blink_state = !blink_state;
            fill_all(blink_state ? white_color(127) : 0);
            vTaskDelay(pdMS_TO_TICKS(200));
            break;
        case LIGHT_MODE_FLOWING:
            for (int i = 0; i < WS2812_STRIPS_NUM; ++i) {
                uint16_t h = hue + (65535 / WS2812_STRIPS_NUM) * i;
                s_strip.setPixelColor(i, s_strip.gamma32(s_strip.ColorHSV(h, 255, 180)));
            }
            s_strip.show();
            hue += 512;
            vTaskDelay(pdMS_TO_TICKS(40));
            break;
        case LIGHT_MODE_SHOW:
            for (int i = 0; i < WS2812_STRIPS_NUM; ++i) {
                s_strip.setPixelColor(i, 0);
            }
            s_strip.setPixelColor(chase_index % WS2812_STRIPS_NUM, white_color(200));
            s_strip.setPixelColor((chase_index + 3) % WS2812_STRIPS_NUM, white_color(200));
            s_strip.show();
            chase_index = (uint8_t)((chase_index + 1) % WS2812_STRIPS_NUM);
            vTaskDelay(pdMS_TO_TICKS(80));
            break;
        case LIGHT_MODE_POWER_LOW:
            blink_state = !blink_state;
            fill_all(blink_state ? s_strip.Color(255, 0, 0) : 0);
            vTaskDelay(pdMS_TO_TICKS(120));
            break;
        case LIGHT_MODE_SLEEP:
        default:
            blink_state = !blink_state;
            fill_all(blink_state ? white_color(50) : 0);
            vTaskDelay(pdMS_TO_TICKS(5000));
            break;
        }
    }
}

void rgb_ws2812_init(void)
{
    s_strip.begin();
    s_strip.clear();
    s_strip.show();
    xTaskCreate(led_anim_task, "led_anim_task", 3072, NULL, 4, NULL);
}

void rgb_ws2812_mode_set(light_mode_t mode)
{
    if (mode < LIGHT_MODE_MAX) {
        s_current_mode = mode;
    }
}
#else
static volatile light_mode_t s_current_mode = LIGHT_MODE_SLEEP;

void rgb_ws2812_init(void)
{
    // ESP32-C2 build fallback: NeoPixel backend is disabled in this environment.
    s_current_mode = LIGHT_MODE_SLEEP;
}

void rgb_ws2812_mode_set(light_mode_t mode)
{
    if (mode < LIGHT_MODE_MAX) {
        s_current_mode = mode;
    }
}
#endif
