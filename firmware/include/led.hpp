/**
 * @file           : led.hpp
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

#pragma once

#include <zephyr/drivers/gpio.h>

class led_t
{
public:
    led_t();

    bool init(const struct device *port_ptr, gpio_pin_t pin, bool active_low = false);

    bool turn_on();
    bool turn_off();
    bool toggle();

private:
    const struct device *port_ptr;
    gpio_pin_t pin;
    bool is_active_low;
};
