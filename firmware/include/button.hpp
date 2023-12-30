/**
 * @file           : button.hpp
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

#pragma once

#include <stdint.h>
#include <stddef.h>

#include "gpio.hpp"

namespace driver
{

/**
 * @brief           Push button driver class
 */
class button_t
{

public:
    /**
     * @brief          Constructor
     * @param[in]      port_ptr Pointer to button's GPIO Port device handle
     * @param[in]      pin GPIO Pin number in specified GPIO Port
     * @param[in]      is_active_low `true` if button's GPIO Pin Active state is LOW,
     *                     so as not to care about the inverted logic
     */
    button_t(const device_t *port_ptr, uint8_t pin, bool is_active_low = false);

    /**
     * @brief          Initialize button
     * @param[in]      gpio_pull Button GPIO Pin bias pull
     * @param[in]      irq_trigger Button GPIO Pin interrupt trigger source
     * @return         `true` on success, `false` if
     *                     - failed to configure button GPIO as Input
     *                     - failed to configure button GPIO IRQ Trigger
     */
    bool init(driver::gpio::pin_pull_t gpio_pull, driver::gpio::pin_irq_trigger_t irq_trigger);

    /**
     * @brief          Get current button state
     * @return         `true` if button is pressed now, `false` otherwise
     */
    bool is_pressed();

private:
    /**
     * @brief          Button Push IRQ Handler
     * @param[in]      arg Argument with pointer to \ref button_t class instance
     */
    static void push_irq_callback(void *arg);

    /**
     * @brief          Button GPIO Pin instance
     */
    driver::gpio::gpio_t gpio;

    /**
     * @brief          Last button press timestamp, ms
     */
    int64_t press_tstamp;
};

} // driver
