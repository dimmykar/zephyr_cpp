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

#include "app/leds_controller.hpp"

#include <zephyr/kernel.h>
#include <zephyr/kernel/thread_stack.h>
#include <zephyr/drivers/gpio.h>

using namespace drivers;
using namespace drivers::gpio;

namespace
{

K_THREAD_STACK_DEFINE(thread_stack, 1024);

}

leds_controller_t::leds_controller_t()
{
    struct gpio_dt_spec orange_led_dt = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
    struct gpio_dt_spec green_led_dt = GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios);
    struct gpio_dt_spec red_led_dt = GPIO_DT_SPEC_GET(DT_ALIAS(led2), gpios);
    struct gpio_dt_spec blue_led_dt = GPIO_DT_SPEC_GET(DT_ALIAS(led3), gpios);

    this->leds.emplace_back(orange_led_dt.port, orange_led_dt.pin); // ORANGE_LED
    this->leds.emplace_back(green_led_dt.port, green_led_dt.pin); // GREEN_LED
    this->leds.emplace_back(red_led_dt.port, red_led_dt.pin); // RED_LED
    this->leds.emplace_back(blue_led_dt.port, blue_led_dt.pin); // BLUE_LED

    for (auto &led : this->leds) {
        led.init();
    }
}

leds_controller_t &leds_controller_t::get_instance()
{
    static leds_controller_t leds_ctrl{};
    return leds_ctrl;
}

bool leds_controller_t::init()
{
    this->thread_handle = this->create_thread();
    this->init_indication();
    return true;
}

void leds_controller_t::init_indication()
{
    this->leds[ORANGE_LED].blink(2 * 110U, 3 * 110U, led_t::BLINK_FOREVER, 0);
    this->leds[RED_LED].blink(2 * 110U, 3 * 110U, led_t::BLINK_FOREVER, 1 * 110U);
    this->leds[BLUE_LED].blink(2 * 110U, 3 * 110U, led_t::BLINK_FOREVER, 2 * 110U);
    this->leds[GREEN_LED].blink(2 * 110U, 3 * 110U, led_t::BLINK_FOREVER, 3 * 110U);
}

void leds_controller_t::shutdown_indication()
{
    this->leds[ORANGE_LED].turn_off();
    this->leds[RED_LED].turn_off();
    this->leds[BLUE_LED].turn_off();
    this->leds[GREEN_LED].turn_off();
}

void leds_controller_t::enable_silent_mode()
{
    this->leds[ORANGE_LED].set_silent_blink();
    this->leds[RED_LED].set_silent_blink();
    this->leds[BLUE_LED].set_silent_blink();
    this->leds[GREEN_LED].set_silent_blink();
}

void leds_controller_t::disable_silent_mode()
{
    this->leds[ORANGE_LED].reset_silent_blink();
    this->leds[RED_LED].reset_silent_blink();
    this->leds[BLUE_LED].reset_silent_blink();
    this->leds[GREEN_LED].reset_silent_blink();
}

k_tid_t leds_controller_t::create_thread()
{
    k_tid_t tid;

    // k_thread_stack_t *stack_ptr = k_thread_stack_alloc(256, 0);
    // if (stack_ptr == nullptr) {
    //     return nullptr;
    // }

    tid = k_thread_create(&this->thread,
                          thread_stack, K_THREAD_STACK_SIZEOF(thread_stack),
                          // stack_ptr, 256,
                          leds_controller_t::leds_update_thread,
                          this, nullptr, nullptr,
                          4, 0, K_NO_WAIT);

    return tid;
}

void leds_controller_t::leds_update_thread(void *arg1, void *arg2, void *arg3)
{
    ARG_UNUSED(arg2);
    ARG_UNUSED(arg3);

    leds_controller_t *instance_ptr = reinterpret_cast<leds_controller_t *>(arg1);

    for (;;) {
        for (auto &led : instance_ptr->leds) {
            led.update_ms();
        }
        k_msleep(1U);
    }
}
