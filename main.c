/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "pico/multicore.h"

#include "MODPlay/modplay.h"
#include "tune.h"

#define AUDIO_L 8
#define AUDIO_R 9

#define BUFFERSIZE 16384 // 512 samples per channel

uint16_t samples[BUFFERSIZE * 2]; // double-buffered

volatile uint32_t rendered = 0, played = 0;

void second_core() {
	while(1) {
		while(rendered == (played & BUFFERSIZE));

		gpio_put(PICO_DEFAULT_LED_PIN, 1);

		RenderMOD(samples + rendered, BUFFERSIZE >> 1);

		gpio_put(PICO_DEFAULT_LED_PIN, 0);

		rendered ^= BUFFERSIZE;
	}
}

void pwm_interrupt_handler() {
	uint8_t slice = pwm_gpio_to_slice_num(AUDIO_L);

	pwm_clear_irq(slice);

	pwm_set_chan_level(slice, PWM_CHAN_A, (samples[played++] ^ 0x8000) >> 4);
	pwm_set_chan_level(slice, PWM_CHAN_B, (samples[played++] ^ 0x8000) >> 4);

	played &= (BUFFERSIZE * 2 - 1);
}

int main() {
	InitMOD(tune, 44100);

	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

	gpio_set_function(AUDIO_L, GPIO_FUNC_PWM);
	gpio_set_function(AUDIO_R, GPIO_FUNC_PWM);

	uint slice_num = pwm_gpio_to_slice_num(AUDIO_L);

    pwm_clear_irq(slice_num);
    pwm_set_irq_enabled(slice_num, true);
    // set the handle function above
    irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_interrupt_handler); 
    irq_set_enabled(PWM_IRQ_WRAP, true);

	// Attempt 44.1 kHz
	pwm_set_wrap(slice_num, clock_get_hz(clk_sys) / 44100);

	pwm_set_chan_level(slice_num, PWM_CHAN_A, 2048);
	pwm_set_chan_level(slice_num, PWM_CHAN_B, 2048);

	pwm_set_enabled(slice_num, true);

	multicore_launch_core1(second_core);

	while(1);
}
