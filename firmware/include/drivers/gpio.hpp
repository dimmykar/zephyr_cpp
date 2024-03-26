/**
 * @file           : gpio.hpp
 * @author         : Dmitry Karasev <karasevsdmitry@yandex.ru>
 * @brief          : MCU General Purpose IO (GPIO) peripheral driver
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

#include <stdio.h>
#include <memory>
#include <zephyr/drivers/gpio.h>

using device_t = struct device;
using gpio_callback_t = struct gpio_callback;
using gpio_irq_handler_fn = void (*)(void *);

namespace drivers
{

namespace gpio
{

/**
 * @brief           Possible GPIO Pin hardware states
 */
enum class pin_state_t
{
    Reset,                                  /*!< GPIO Pin is in LOW state */
    Set                                     /*!< GPIO Pin is in HIGH state */
};

/**
 * @brief           Possible GPIO Pin logical level states
 * @details         Logical state takes into account \ref gpio_t::is_active_low flag
 */
enum class pin_active_state_t
{
    Inactive,                               /*!< GPIO Pin is in Logical 0 state */
    Active                                  /*!< GPIO Pin is in Logical 1 state */
};

/**
 * @brief           Possible GPIO Pin Output Mode configurations
 */
enum class pin_output_mode_t
{
    PushPull,                               /*!< GPIO Pin operates in "Push-Pull" mode */
    OpenDrain                               /*!< GPIO Pin operates in "Open Drain" mode */
};

/**
 * @brief           Possible GPIO Pin Slew Rate configurations
 * @details         Determines how quickly the pin switches to a new state
 */
enum class pin_output_slew_t
{
    Slow,                                   /*!< GPIO Pin switches slowly (good for solid LEDs, buttons, etc.) */
    Medium,                                 /*!< GPIO Pin switches with medium speed (good for slow peripherals) */
    Fast,                                   /*!< GPIO Pin switches fast (good for fast peripherals) */
    VeryFast                                /*!< GPIO Pin switches very fast (good for high-speed peripherals) */
};

/**
 * @brief           Possible GPIO Pin Input Bias configurations
 */
enum class pin_pull_t
{
    Float,                                  /*!< GPIO Pin is in floating input state */
    PullUp,                                 /*!< GPIO Pin is pulled to HIGH level */
    PullDown                                /*!< GPIO Pin is pulled to LOW level */
};

/**
 * @brief           Possible GPIO Pin interrupt trigger configurations
 */
enum class pin_irq_trigger_t
{
    EdgeToActive,                           /*!< Detect Edge to Active state (Rising Edge in case of Active HIGH) */
    EdgeToInactive,                         /*!< Detect Edge to Inactive state (Rising Edge in case of Active LOW) */
    EdgeAny                                 /*!< Detect any switches to both states */
};

/**
 * @brief           Handle for attached to GPIO Pin IRQ Handler Callback
 */
struct gpio_irq_wrapper_t
{
    gpio_callback_t cb_ctx;                 /*!< IRQ Handler Callback context */
    gpio_irq_handler_fn irq_handler;        /*!< Pointer to attached IRQ Handler callback */
    void *arg;                              /*!< Argument for attached IRQ Handler callback */
};

/**
 * @brief           GPIO Pin driver class
 * @details         Controls specified GPIO Pin operation in Digital Input
 *                      or Digital Output mode
 * @todo            Implement Analog Input mode APIs
 */
class gpio_t
{
public:
    /**
     * @brief          Constructor
     * @param[in]      port_ptr Pointer to GPIO Port device handle
     * @param[in]      pin GPIO Pin number in specified GPIO Port
     * @param[in]      is_active_low `true` if GPIO Pin Active state is LOW,
     *                     so as not to care about the inverted logic
     */
    gpio_t(const device_t *port_ptr, uint8_t pin, bool is_active_low = false);

    /**
     * @brief          Configure GPIO Pin as Output
     * @param[in]      omode GPIO Pin Output mode
     * @param[in]      init_state GPIO Pin initial state after configuring
     * @param[in]      speed GPIO Pin slew rate
     * @return         `true` if GPIO Pin configured successfully,
     *                     `false` if
     *                         - GPIO Port is not exists
     *                         - GPIO Pin configuration failed
     */
    bool config_as_output(pin_output_mode_t omode, pin_active_state_t init_state = pin_active_state_t::Inactive,
                              pin_output_slew_t speed = pin_output_slew_t::Slow);

    /**
     * @brief          Set GPIO Pin to configured Active State
     */
    void set();

    /**
     * @brief          Reset GPIO Pin to configured Inactive State
     */
    void reset();

    /**
     * @brief          Toggle current state of GPIO Pin
     */
    void toggle();

    /**
     * @brief          Configure GPIO Pin as Input
     * @param[in]      pull GPIO Pin bias pull
     * @return         `true` if GPIO Pin configured successfully,
     *                     `false` if
     *                         - GPIO Port is not exists
     *                         - GPIO Pin configuration failed
     */
    bool config_as_input(pin_pull_t pull);

    /**
     * @brief          Read current physical state of GPIO Pin
     * @return         Member of \ref pin_state_t
     */
    pin_state_t read_state();

    /**
     * @brief          Read current configured Active state of GPIO Pin
     * @return         Member of \ref pin_active_state_t
     */
    pin_active_state_t read_active_state();

    /**
     * @brief          Enable Interrupt for GPIO Pin and attach IRQ Handler callback for it
     * @param[in]      irq_handler Pointer to IRQ Handler callback
     * @param[in]      irq_handler_arg Argument for attached IRQ Handler callback
     * @param[in]      irq_trigger GPIO Pin interrupt trigger source
     * @return         `true` if interrupt configured successfully,
     *                     `false` if
     *                         - Passed nullptr IRQ Handler callback
     *                         - GPIO Pin is not configured as Input
     *                         - Failed to attach IRQ Handler callback
     */
    bool attach_irq(gpio_irq_handler_fn irq_handler, void *irq_handler_arg, pin_irq_trigger_t irq_trigger);

    /**
     * @brief          Disable Interrupt for GPIO Pin and detach its IRQ Handler callback
     * @return         `true` if IRQ Handler callback detached successfully, `false` otherwise
     */
    bool detach_irq();

private:
    /**
     * @brief          Common IRQ Handler callback
     * @details        Callback with signature required by Zephyr GPIO API.
     *                     Delegates IRQ Handling to configured IRQ Handler callback
     * @param[in]      port Pointer to GPIO Port device handle
     * @param[in]      cb Pointer to IRQ Handler Callback context
     * @param[in]      pins A bit mask of relevant pins for the IRQ Handler callback
     */
    static void pin_irq_handler(const device_t *port, gpio_callback_t *cb, gpio_port_pins_t pins);

    /**
     * @brief          Pointer to controlling GPIO Port device handle
     */
    const device_t *port_ptr;

    /**
     * @brief          Controlling GPIO Pin of the specified GPIO Port
     */
    uint8_t pin;

    /**
     * @brief          `true` if GPIO Pin Active state is LOW,
     *                     `false` if GPIO Pin Active state is HIGH
     */
    bool is_active_low;

    /**
     * @brief          Pointer to IRQ Handler callback wrapper handle
     */
    std::unique_ptr<gpio_irq_wrapper_t> irq_ctx;
};

} // gpio

} // driver
