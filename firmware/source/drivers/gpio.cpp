/**
 * @file           : gpio.cpp
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

#include "drivers/gpio.hpp"

#include <zephyr/kernel.h>

using namespace drivers::gpio;

gpio_t::gpio_t(const device_t *port_ptr, uint8_t pin, bool is_active_low)
    : port_ptr(port_ptr), pin(pin), is_active_low(is_active_low)
{
    // Raise abort() if std::bad_alloc exception throws with -fno-exceptions
    this->irq_ctx = std::make_unique<gpio_irq_wrapper_t>();
}

bool gpio_t::config_as_output(pin_output_mode_t omode, pin_active_state_t init_state, pin_output_slew_t speed)
{
    if (!device_is_ready(this->port_ptr)) {
        return false;
    }

    gpio_flags_t output_flags = (init_state == pin_active_state_t::Inactive) ? GPIO_OUTPUT_INACTIVE : GPIO_OUTPUT_ACTIVE;
    if (omode == pin_output_mode_t::OpenDrain) {
        output_flags |= GPIO_OPEN_DRAIN;
    }
    if (this->is_active_low) {
        output_flags |= GPIO_ACTIVE_LOW;
    }

    // TODO Configure slew rate
    ARG_UNUSED(speed);

    int32_t ret = gpio_pin_configure(this->port_ptr, this->pin, output_flags);
    if (ret < 0) {
        return false;
    }

    return true;
}

void gpio_t::set()
{
    gpio_pin_set(this->port_ptr, this->pin, 1);
}

void gpio_t::reset()
{
    gpio_pin_set(this->port_ptr, this->pin, 0);
}

void gpio_t::toggle()
{
    gpio_pin_toggle(this->port_ptr, this->pin);
}

bool gpio_t::config_as_input(pin_pull_t pull)
{
    if (!device_is_ready(this->port_ptr)) {
        return false;
    }

    gpio_flags_t input_flags = GPIO_INPUT;
    if (pull == pin_pull_t::PullUp) {
        input_flags |= GPIO_PULL_UP;
    }
    else if (pull == pin_pull_t::PullDown) {
        input_flags |= GPIO_PULL_DOWN;
    }
    if (this->is_active_low) {
        input_flags |= GPIO_ACTIVE_LOW;
    }

    int32_t ret = gpio_pin_configure(this->port_ptr, this->pin, input_flags);
    if (ret < 0) {
        return false;
    }

    return true;
}

pin_state_t gpio_t::read_state()
{
    return (gpio_pin_get_raw(this->port_ptr, this->pin) == 1) ? pin_state_t::Set : pin_state_t::Reset;
}

pin_active_state_t gpio_t::read_active_state()
{
    return (gpio_pin_get(this->port_ptr, this->pin) == 1) ? pin_active_state_t::Active : pin_active_state_t::Inactive;
}

bool gpio_t::attach_irq(gpio_irq_handler_fn irq_handler, void *irq_handler_arg, pin_irq_trigger_t irq_trigger)
{
    if (irq_handler == nullptr) {
        return false;
    }

    if (!(gpio_pin_is_input(this->port_ptr, this->pin) == 1)) {
        return false;
    }

    gpio_flags_t edge_flags = 0;
    if (irq_trigger == pin_irq_trigger_t::EdgeToActive) {
        edge_flags |= GPIO_INT_EDGE_TO_ACTIVE;
    }
    else if (irq_trigger == pin_irq_trigger_t::EdgeToInactive) {
        edge_flags |= GPIO_INT_EDGE_TO_INACTIVE;
    }
    else if (irq_trigger == pin_irq_trigger_t::EdgeAny) {
        edge_flags |= GPIO_INT_EDGE_BOTH;
    }
    int32_t ret = gpio_pin_interrupt_configure(this->port_ptr, this->pin, edge_flags);
    if (ret < 0) {
        return false;
    }

    this->irq_ctx->irq_handler = irq_handler;
    this->irq_ctx->arg = irq_handler_arg;
    gpio_init_callback(&this->irq_ctx->cb_ctx, gpio_t::pin_irq_handler, BIT(this->pin));
    ret = gpio_add_callback(this->port_ptr, &this->irq_ctx->cb_ctx);
    if (ret < 0) {
        return false;
    }

    return true;
}

bool gpio_t::detach_irq()
{
    if (!(gpio_pin_is_input(this->port_ptr, this->pin) == 1)) {
        return false;
    }

    int32_t ret = gpio_remove_callback(this->port_ptr, &this->irq_ctx->cb_ctx);
    if (ret < 0) {
        return false;
    }

    return true;
}

void gpio_t::pin_irq_handler(const device_t *port, gpio_callback_t *cb, gpio_port_pins_t pins)
{
    struct gpio_irq_wrapper_t *container = CONTAINER_OF(cb, gpio_irq_wrapper_t, cb_ctx);
    container->irq_handler(container->arg);
}
