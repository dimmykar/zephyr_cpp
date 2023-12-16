/**
 * @file           : leds_controller.cpp
 * @author         : Dmitry Karasev <karasevsdmitry@yandex.ru>
 * @brief          : Board LEDs Controller driver
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

#include "leds_controller.hpp"

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

#define LED_ORANGE_NODE DT_ALIAS(led0)
#define LED_GREEN_NODE  DT_ALIAS(led1)
#define LED_RED_NODE    DT_ALIAS(led2)
#define LED_BLUE_NODE   DT_ALIAS(led3)

namespace
{

struct gpio_dt_spec orange_led_dt = GPIO_DT_SPEC_GET(LED_ORANGE_NODE, gpios);
struct gpio_dt_spec green_led_dt = GPIO_DT_SPEC_GET(LED_GREEN_NODE, gpios);
struct gpio_dt_spec red_led_dt = GPIO_DT_SPEC_GET(LED_RED_NODE, gpios);
struct gpio_dt_spec blue_led_dt = GPIO_DT_SPEC_GET(LED_BLUE_NODE, gpios);

}

leds_controller_t::leds_controller_t()
{
    this->orange_led.init(orange_led_dt.port, orange_led_dt.pin);
    this->green_led.init(green_led_dt.port, green_led_dt.pin);
    this->red_led.init(red_led_dt.port, red_led_dt.pin);
    this->blue_led.init(blue_led_dt.port, blue_led_dt.pin);
}

leds_controller_t &leds_controller_t::get_instance()
{
    static leds_controller_t leds_ctrl{};
    return leds_ctrl;
}

bool leds_controller_t::init()
{
    return true;
}

void leds_controller_t::toggle_all()
{
    this->orange_led.toggle();
    this->green_led.toggle();
    this->red_led.toggle();
    this->blue_led.toggle();
}