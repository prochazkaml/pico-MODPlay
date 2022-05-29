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

/*
 * Rules for picking the correct audio pins:
 * - The left channel pin must have an even number, the right one must have an odd number.
 * - They must be consecutive.
 * 
 * Valid combinations are: 0 and 1, 2 and 3, ..., 8 and 9... you get the idea.
 */

#define AUDIO_L 8
#define AUDIO_R 9

#define AUDIO_SLICE ((AUDIO_L >> 1u) & 7u)

#define BUFFERSIZE 16384 // 8192 samples per channel

uint16_t samples[BUFFERSIZE * 2]; // double-buffered

volatile uint32_t rendered = 0, played = 0;

void second_core() {
	while(1) {
		// Wait until the target buffer becomes outdated

		while(rendered == (played & BUFFERSIZE));

		// Render new audio!

		gpio_put(PICO_DEFAULT_LED_PIN, 1);

		RenderMOD(samples + rendered, BUFFERSIZE >> 1);

		gpio_put(PICO_DEFAULT_LED_PIN, 0);

		// Switch the buffers

		rendered ^= BUFFERSIZE;
	}
}

void pwm_interrupt_handler() {
	// Acknowledge the interrupt and output data from the audio buffer

	pwm_clear_irq(AUDIO_SLICE);

	pwm_set_chan_level(AUDIO_SLICE, PWM_CHAN_A, (samples[played++] ^ 0x8000) >> 4);
	pwm_set_chan_level(AUDIO_SLICE, PWM_CHAN_B, (samples[played++] ^ 0x8000) >> 4);

	// Limit the audio buffer pointer, so it doesn't run off anywhere

	played &= (BUFFERSIZE * 2 - 1);
}

int main() {
	// Initialize the library

	InitMOD(tune, 44100);

	// Initialize the internal LED

	gpio_init(PICO_DEFAULT_LED_PIN);
	gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

	// Initialize PWM and set up the interrupt @ 44.1 kHz

	gpio_set_function(AUDIO_L, GPIO_FUNC_PWM);
	gpio_set_function(AUDIO_R, GPIO_FUNC_PWM);

    pwm_clear_irq(AUDIO_SLICE);
    pwm_set_irq_enabled(AUDIO_SLICE, true);

    irq_set_exclusive_handler(PWM_IRQ_WRAP, pwm_interrupt_handler); 
    irq_set_enabled(PWM_IRQ_WRAP, true);

	pwm_set_wrap(AUDIO_SLICE, clock_get_hz(clk_sys) / 44100);
	pwm_set_enabled(AUDIO_SLICE, true);

	// Start the player!

	multicore_launch_core1(second_core);

	// Ideally, we should do something useful here, but this is just a demo, so who cares

	while(1);
}
