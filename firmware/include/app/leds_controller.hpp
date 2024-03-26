/**
 * @file           : leds_controller.hpp
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

#pragma once

#include <vector>
#include <zephyr/kernel.h>
#include "drivers/led.hpp"

enum
{
    ORANGE_LED,
    GREEN_LED,
    RED_LED,
    BLUE_LED
};

class leds_controller_t final
{
public:
    static leds_controller_t &get_instance();

    bool init();

    void init_indication();
    void shutdown_indication();

    void enable_silent_mode();
    void disable_silent_mode();

private:
    leds_controller_t();

    leds_controller_t(const leds_controller_t &) = delete;
    leds_controller_t(leds_controller_t &&) = delete;
    leds_controller_t &operator=(const leds_controller_t &) = delete;
    leds_controller_t &&operator=(leds_controller_t &&) = delete;

    k_tid_t create_thread();
    static void leds_update_thread(void *arg1, void *arg2, void *arg3);

    std::vector<drivers::led_t> leds;

    k_thread thread;
    k_tid_t thread_handle;
};
