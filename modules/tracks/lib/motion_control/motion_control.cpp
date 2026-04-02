/*
 * SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Arduino.h>
#include <math.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "motion_control.h"

#define LEDC_DUTY_RES_BITS          (13)
#define LEDC_FREQUENCY              (4000)

#define LEDC_M1_CHANNEL_A_IO        (4)
#define LEDC_M1_CHANNEL_B_IO        (5)
#define LEDC_M2_CHANNEL_A_IO        (6)
#define LEDC_M2_CHANNEL_B_IO        (7)

#define LEDC_M1_CHANNEL_A           (0)
#define LEDC_M1_CHANNEL_B           (1)
#define LEDC_M2_CHANNEL_A           (2)
#define LEDC_M2_CHANNEL_B           (3)

#define PWM_MAX_DUTY                ((1 << LEDC_DUTY_RES_BITS) - 1)

float m1_coefficient = 1.0;
float m2_coefficient = 1.0;

void motor_ledc_init(void)
{
    ledcAttachChannel(LEDC_M1_CHANNEL_A_IO, LEDC_FREQUENCY, LEDC_DUTY_RES_BITS, LEDC_M1_CHANNEL_A);
    ledcAttachChannel(LEDC_M1_CHANNEL_B_IO, LEDC_FREQUENCY, LEDC_DUTY_RES_BITS, LEDC_M1_CHANNEL_B);
    ledcAttachChannel(LEDC_M2_CHANNEL_A_IO, LEDC_FREQUENCY, LEDC_DUTY_RES_BITS, LEDC_M2_CHANNEL_A);
    ledcAttachChannel(LEDC_M2_CHANNEL_B_IO, LEDC_FREQUENCY, LEDC_DUTY_RES_BITS, LEDC_M2_CHANNEL_B);
}

static void set_left_motor_speed(int speed)
{
    speed = constrain(speed, -MOTOR_SPEED_MAX, MOTOR_SPEED_MAX);
    if (speed >= 0) {
        uint32_t m1a_duty = (uint32_t)((speed * m1_coefficient * PWM_MAX_DUTY) / 100);
        ledcWriteChannel(LEDC_M1_CHANNEL_A, m1a_duty);
        ledcWriteChannel(LEDC_M1_CHANNEL_B, 0);
    } else {
        uint32_t m1b_duty = (uint32_t)((-speed * m1_coefficient * PWM_MAX_DUTY) / 100);
        ledcWriteChannel(LEDC_M1_CHANNEL_A, 0);
        ledcWriteChannel(LEDC_M1_CHANNEL_B, m1b_duty);
    }
}

static void set_right_motor_speed(int speed)
{
    speed = constrain(speed, -MOTOR_SPEED_MAX, MOTOR_SPEED_MAX);
    if (speed >= 0) {
        uint32_t m2a_duty = (uint32_t)((speed * m2_coefficient * PWM_MAX_DUTY) / 100);
        ledcWriteChannel(LEDC_M2_CHANNEL_A, m2a_duty);
        ledcWriteChannel(LEDC_M2_CHANNEL_B, 0);
    } else {
        uint32_t m2b_duty = (uint32_t)((-speed * m2_coefficient * PWM_MAX_DUTY) / 100);
        ledcWriteChannel(LEDC_M2_CHANNEL_A, 0);
        ledcWriteChannel(LEDC_M2_CHANNEL_B, m2b_duty);
    }
}

void keero_motion_control(float x, float y)
{
    float base_speed = y * MOTOR_SPEED_MAX;
    float turn_adjust = x * MOTOR_SPEED_MAX;

    int left_speed = (int)(base_speed + turn_adjust);
    int right_speed = (int)(base_speed - turn_adjust);

    if (left_speed > MOTOR_SPEED_MAX) left_speed = MOTOR_SPEED_MAX;
    if (right_speed > MOTOR_SPEED_MAX) right_speed = MOTOR_SPEED_MAX;

    set_left_motor_speed(left_speed);
    set_right_motor_speed(right_speed);
}

void set_motor_speed_coefficients(float coefficient)
{
    if (coefficient < 0) {
        if (m1_coefficient >= 0.7) {
            m1_coefficient -= 0.025;
        }
        m2_coefficient = 1.0;
    } else {
        if (m2_coefficient >= 0.7) {
            m2_coefficient -= 0.025;
        }
        m1_coefficient = 1.0;
    }
    printf("m1_coefficient = %f, m2_coefficient = %f\n", m1_coefficient, m2_coefficient);
}

typedef struct {
    int         left_speed;
    int         right_speed;
    uint16_t    time;
} dance_motion_t;

void keero_dance(void)
{
    // Define dance moves with left/right track speed and duration.
    dance_motion_t dance_moves[] = {
        // First dance sequence
        {MOTOR_SPEED_MAX, MOTOR_SPEED_MAX, 1000},  // Move forward fast for 1.0 s
        {MOTOR_SPEED_80, -MOTOR_SPEED_80, 800},    // Turn left for 0.8 s
        {-MOTOR_SPEED_60, -MOTOR_SPEED_60, 1200},  // Move backward slowly for 1.2 s
        {-MOTOR_SPEED_80, MOTOR_SPEED_80, 800},    // Turn right for 0.8 s
        {MOTOR_SPEED_80, MOTOR_SPEED_80, 1000},    // Move forward at medium speed for 1.0 s
        {-MOTOR_SPEED_80, MOTOR_SPEED_80, 800},    // Turn right for 0.8 s
        {-MOTOR_SPEED_MAX, -MOTOR_SPEED_MAX, 1000},// Move backward fast for 1.0 s
        {MOTOR_SPEED_80, -MOTOR_SPEED_80, 800},    // Turn left for 0.8 s
        {0, 0, 500},                               // Stop for 0.5 s

        // Reverse sequence to return to the start
        {-MOTOR_SPEED_80, MOTOR_SPEED_80, 800},    // Turn right for 0.8 s
        {MOTOR_SPEED_MAX, MOTOR_SPEED_MAX, 1000},  // Move forward fast for 1.0 s
        {MOTOR_SPEED_80, -MOTOR_SPEED_80, 800},    // Turn left for 0.8 s
        {-MOTOR_SPEED_80, -MOTOR_SPEED_80, 1000},  // Move backward at medium speed for 1.0 s
        {MOTOR_SPEED_80, -MOTOR_SPEED_80, 800},    // Turn left for 0.8 s
        {MOTOR_SPEED_60, MOTOR_SPEED_60, 1200},    // Move forward slowly for 1.2 s
        {-MOTOR_SPEED_80, MOTOR_SPEED_80, 800},    // Turn right for 0.8 s
        {-MOTOR_SPEED_MAX, -MOTOR_SPEED_MAX, 1000},// Move backward fast for 1.0 s
        {0, 0, 500},                               // Stop for 0.5 s
    };

    // Iterate through the move list and execute each action.
    for (int i = 0; i < sizeof(dance_moves) / sizeof(dance_moves[0]); i++) {
        // Control left and right motor speed.
        set_left_motor_speed(dance_moves[i].left_speed);
        set_right_motor_speed(dance_moves[i].right_speed);

        // Hold the move for the specified duration.
        vTaskDelay(dance_moves[i].time / portTICK_PERIOD_MS);

        // Stop motor PWM output.
        set_left_motor_speed(0);
        set_right_motor_speed(0);
    }
}
