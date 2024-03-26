/**
 * @file           : led.cpp
 * @author         : Dmitry Karasev <karasevsdmitry@yandex.ru>
 * @brief          : LED driver
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

#include "drivers/led.hpp"

#include <functional>
#include <zephyr/kernel.h>

using namespace drivers;
using namespace drivers::gpio;

led_t::led_t(const device_t *port_ptr, uint8_t pin, bool is_active_low)
    : gpio{port_ptr, pin, is_active_low},
      mode{drivers::led_t::mode_t::SOLID},
      is_silent_blink{false}
{
    this->reset_blinking();
}

bool led_t::init()
{
    if (!this->gpio.config_as_output(pin_output_mode_t::PushPull, pin_active_state_t::Inactive)) {
        return false;
    }

    return true;
}

void led_t::turn_on()
{
    this->mode = drivers::led_t::mode_t::SOLID;
    this->reset_blinking();
    this->gpio.set();
}

void led_t::turn_off()
{
    this->mode = drivers::led_t::mode_t::SOLID;
    this->reset_blinking();
    this->gpio.reset();
}

void led_t::set_silent_blink()
{
    if (this->mode != drivers::led_t::mode_t::BLINK)
        return;

    this->is_silent_blink = true;
}

void led_t::reset_silent_blink()
{
    if (this->mode != drivers::led_t::mode_t::BLINK)
        return;

    this->is_silent_blink = false;
}

void led_t::blink(uint32_t on_ms, uint32_t off_ms, size_t blinks_num, uint32_t pend_ms)
{
    this->gpio.reset();

    this->mode = drivers::led_t::mode_t::BLINK;

    this->config.on_timeout_ms = on_ms;
    this->config.off_timeout_ms = off_ms;
    this->config.pend_timeout_ms = pend_ms;
    this->config.blinks_num = blinks_num;

    this->status.on_ms = on_ms;
    this->status.off_ms = off_ms;
    this->status.pend_ms = pend_ms;
    this->status.blinks_cnt = blinks_num;

    if ((this->config.pend_timeout_ms == 0) && (this->config.blinks_num != 0)) {
        this->gpio.set();
    }
}

void led_t::update_ms()
{
    if (this->mode == drivers::led_t::mode_t::SOLID) {
        return;
    }

    std::function<void(void)> turn_on_cb = !this->is_silent_blink ? std::bind(&gpio_t::set, &this->gpio)
                                                                  : std::bind(&gpio_t::reset, &this->gpio);

    /* Check pending start  */
    if (this->status.pend_ms != 0) {
        if (led_t::check_for_counter_zeroing(&this->status.pend_ms))
            turn_on_cb();

        return;
    }

    /* Handle ON period */
    if (led_t::check_for_counter_zeroing(&this->status.on_ms)) {
        this->gpio.reset();

        if (this->is_blinks_cnt_expired())
            return;

        this->status.off_ms = this->config.off_timeout_ms;
    }

    /* Handle OFF period */
    if (led_t::check_for_counter_zeroing(&this->status.off_ms)) {
        turn_on_cb();
        this->status.on_ms = this->config.on_timeout_ms;
    }
}

bool led_t::is_blinks_cnt_expired()
{
    /* If the counter is set to an infinite value, then we don't check */
    if (this->config.blinks_num == led_t::BLINK_FOREVER)
        return false;

    if (!led_t::check_for_counter_zeroing(&this->status.blinks_cnt))
        return false;

    return true;
}

bool led_t::check_for_counter_zeroing(uint32_t *cnt_ptr)
{
    if (*cnt_ptr != 0) {
        if (--(*cnt_ptr) == 0)
            return true;
    }

    return false;
}

void led_t::reset_blinking()
{
    this->config.on_timeout_ms = 0;
    this->config.off_timeout_ms = 0;
    this->config.pend_timeout_ms = 0;
    this->config.blinks_num = 0;

    this->status.on_ms = 0;
    this->status.off_ms = 0;
    this->status.pend_ms = 0;
    this->status.blinks_cnt = 0;

    this->reset_silent_blink();
}
