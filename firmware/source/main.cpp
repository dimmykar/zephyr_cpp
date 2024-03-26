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

#include "leds_controller.hpp"
#include "button.hpp"

using namespace driver;

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

/**
 * @brief          The application main loop
 * @return         `0`, but in normal operation the function no returns
 */
int main(void)
{
    LOG_INF("Hello from Zephyr RTOS");

    struct gpio_dt_spec user_button_dt = GPIO_DT_SPEC_GET(DT_ALIAS(sw0), gpios);
    driver::button_t user_btn{user_button_dt.port, user_button_dt.pin};
    user_btn.init(driver::gpio::pin_pull_t::Float, driver::gpio::pin_irq_trigger_t::EdgeToActive);

    leds_controller_t &leds_ctrl = leds_controller_t::get_instance();
    if (!leds_ctrl.init()) {
        LOG_ERR("Failed to initialize leds controller");
        return 0;
    }

    bool is_silent = false;
    for (;;)
    {
        if (user_btn.is_pressed()) {
            is_silent ? leds_ctrl.enable_silent_mode() : leds_ctrl.disable_silent_mode();
            is_silent = !is_silent;
        }

        k_msleep(100U);
    }

    return 0;
}
