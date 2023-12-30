/**
 * @file           : button.cpp
 * @author         : Dmitry Karasev <karasevsdmitry@yandex.ru>
 * @brief          : Button driver
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

#include "button.hpp"

#include <zephyr/kernel.h>

using namespace driver;
using namespace driver::gpio;

button_t::button_t(const device_t *port_ptr, uint8_t pin, bool is_active_low)
    : gpio{port_ptr, pin, is_active_low}, press_tstamp{0}
{
}

bool button_t::init(driver::gpio::pin_pull_t gpio_pull, driver::gpio::pin_irq_trigger_t irq_trigger)
{
    if (!this->gpio.config_as_input(gpio_pull))
    {
        return false;
    }

    if (!this->gpio.attach_irq(button_t::push_irq_callback, this, irq_trigger))
    {
        return false;
    }

    return true;
}

bool button_t::is_pressed()
{
    return (this->gpio.read_active_state() == pin_active_state_t::Active) &&
               (k_uptime_get() - this->press_tstamp) > 100U;
}

void button_t::push_irq_callback(void *arg)
{
    button_t *instance_ptr = reinterpret_cast<button_t *>(arg);

    instance_ptr->press_tstamp = k_uptime_get();
}
