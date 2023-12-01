/**
 * @file           : main.cpp
 * @author         : Dmitry Karasev <karasevsdmitry@yandex.ru>
 * @brief          : Main Firmware entry point
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 Dmitry Karasev
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <stddef.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>

#define LED0_NODE DT_ALIAS(led0)

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

/**
 * @brief          The application main loop
 * @return         `0`, but in normal operation the function no returns
 */
int main(void)
{
    int ret;

    LOG_INF("Hello from Zephyr RTOS");

    if (!gpio_is_ready_dt(&led)) {
        return 0;
    }

    ret = gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);
    if (ret < 0) {
        return 0;
    }

    for (;;) {
        ret = gpio_pin_toggle_dt(&led);
        if (ret < 0) {
            return 0;
        }

        k_msleep(100U);
    }

    return 0;
}
