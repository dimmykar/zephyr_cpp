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

#include <stdint.h>
#include <stddef.h>
#include <limits>

#include "drivers/gpio.hpp"

namespace drivers
{

/**
 * @brief           LED driver class
 * @details         Controls the LED in one of the given mode:
 *                        - solid state operation (ON/OFF)
 *                        - blinking with specified ON/OFF periods
 *                      The class is designed so that the LED state is updated
 *                      periodically by some thread or hardware timer
 */
class led_t
{
public:
    /**
     * @brief          The value, at which the LED blinks forever
     */
    static constexpr size_t BLINK_FOREVER = std::numeric_limits<uint32_t>::max();

    /**
     * @brief          Constructor
     * @param[in]      port_ptr Pointer to LED's GPIO Port device handle
     * @param[in]      pin GPIO Pin number in specified GPIO Port
     * @param[in]      is_active_low `true` if LED's GPIO Pin Active state is LOW,
     *                     so as not to care about the inverted logic
     */
    led_t(const device_t *port_ptr, uint8_t pin, bool is_active_low = false);

    /**
     * @brief          Initialize LED
     * @return         `true` on success, `false` if
     *                     - failed to configure LED GPIO as Output
     */
    bool init();

    /**
     * @brief          Set LED to solid ON state
     */
    void turn_on();

    /**
     * @brief          Set LED to solid OFF state
     */
    void turn_off();

    /**
     * @brief          Set LED to blinking state with given configuration
     * @param[in]      on_ms: LED's ON state period in milliseconds
     * @param[in]      off_ms: LED's OFF state period in milliseconds
     * @param[in]      blinks_num: Number of blinks or \ref led_t::BLINK_FOREVER
     *                     in case of endless blinking
     * @param[in]      pend_ms: This value sets blinking pending start timeout
     *                     in milliseconds. Pass `0` to start blinking immediately
     */
    void blink(uint32_t on_ms, uint32_t off_ms, size_t blinks_num = led_t::BLINK_FOREVER,
                   uint32_t pend_ms = 0);

    /**
     * @brief          Set "Silent Blink" mode to active state
     * @details        If the LED is operating in \ref mode_t::BLINK mode,
     *                    the "Silent Blink" mode can be used. In this mode the LED
     *                    is turning OFF, but state counters continues to update
     *                    to save current LED operation state
     */
    void set_silent_blink();

    /**
     * @brief          Set "Silent Blink" mode to inactive state
     * @details        The LED continues to blink according to it's current
     *                     operation status
     */
    void reset_silent_blink();

    /**
     * @brief          Update current LED operation status
     * @note           Update should be done every 1 millisecond
     */
    void update_ms();

private:
    /**
     * @brief          Check LED blinks counter for zeroing
     * @return         'true' if the counter has expired, 'false' if counting is disabled
     *                     or the counter is not zero
     */
    bool is_blinks_cnt_expired();

    /**
     * @brief          Check any counter for zeroing transition
     * @param[in,out]  cnt_ptr: Pointer to counter for checking
     * @return         'true' if zeroing is detected, 'false' if counter is equal zero or
     *                     zeroing transition is not detected
     */
    static bool check_for_counter_zeroing(uint32_t *cnt_ptr);

    /**
     * @brief          Clear blink operation configs and status data
     */
    void reset_blinking();

    /**
     * @brief          LED driver operation configs
     */
    struct config_t
    {
        uint32_t on_timeout_ms;             /*!< LED ON state period in milliseconds */
        uint32_t off_timeout_ms;            /*!< LED OFF state period in milliseconds */
        uint32_t pend_timeout_ms;           /*!< Blinking pending start timeout in milliseconds */
        size_t   blinks_num;                /*!< Number of blinks or \ref led_t::BLINK_FOREVER
                                                     in case of endless blinking */
    } config;

    /**
     * @brief          LED driver operation status
     */
    struct status_t
    {
        uint32_t on_ms;                     /*!< Time to end of turned ON period in milliseconds */
        uint32_t off_ms;                    /*!< Time to end of turned OFF period in milliseconds */
        uint32_t pend_ms;                   /*!< Time to end of blinking pending start in milliseconds */
        size_t   blinks_cnt;                /*!< Blinks counter of the configured LED blinking operation */
    } status;

    /**
     * @brief          LED GPIO Pin instance
     */
    drivers::gpio::gpio_t gpio;

    /**
     * @brief          LED driver operation mode
     */
    enum class mode_t
    {
        SOLID = 0,                          /*!< Solid state operation (ON/OFF) */
        BLINK,                              /*!< Blinking with specified ON/OFF periods */
    } mode;

    /**
     * @brief          Is "Silent Blink" mode active flag
     * @details        `true` if "Silent Blink" mode is active, `false` otherwise
     */
    bool is_silent_blink;
};

} // driver
