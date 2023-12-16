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

#include "led.hpp"

#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>

led_t::led_t()
    : port_ptr(nullptr), pin(0)
{

}

bool led_t::init(const struct device *port_ptr, gpio_pin_t pin, bool active_low)
{
    int ret;

    if (!device_is_ready(port_ptr)) {
        return false;
    }

    ret = gpio_pin_configure(port_ptr, pin, GPIO_OUTPUT_INACTIVE);
    if (ret < 0) {
        return false;
    }

    this->port_ptr = port_ptr;
    this->pin = pin;
    this->is_active_low = active_low;

    return true;
}

bool led_t::turn_on()
{
    if (gpio_pin_set(this->port_ptr, this->pin, this->is_active_low ? 0 : 1) < 0) {
        return false;
    }

    return true;
}

bool led_t::turn_off()
{
    if (gpio_pin_set(this->port_ptr, this->pin, this->is_active_low ? 1 : 0) < 0) {
        return false;
    }

    return true;
}

bool led_t::toggle()
{
    if (gpio_pin_toggle(this->port_ptr, this->pin) < 0) {
        return false;
    }

    return true;
}
