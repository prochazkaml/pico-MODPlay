# pico-MODPlay

A simple .mod player for the Raspberry Pi Pico based on my [MODPlay library](https://github.com/prochazkaml/MODPlay).

In the default configuration, the audio buffer is 64 kB, as it also lights up the LED anytime it's rendering new audio, so the huge buffer is used to make the flashing more visible.
Of course, you can tune it down to something more "sane" (1 kB should do the job just fine) and disable the LED.

The audio is generated at 44.1 kHz and is internally calculated using 16 bit samples, which is then shifted down to 12 bits for PWM output (left channel is on pin 8, while the right channel is on pin 9).

## See it in action

[Here's a demo of this program playing music from Sanity's Interference demo.](https://youtu.be/WsvjrJQ4Oxw)

## Why?

Because I was bored.

And because I needed to prove a point to my ICT teacher that my library could indeed run on a stock, non-overclocked Pico.

## Limitations

The main one is the available memory. As MODPlay modifies the song data when it's initialized, it has to be copied to RAM first, and there is not a ton of it on the Pico
(measly 264 kilobytes, and not all of that is available for the song – some is taken up by the audio buffer, the stack etc.).

And it is still quite CPU intensive (when measuring the LED with a high-speed camera, it yielded a result of <30 % CPU usage (buffer size = 64 kB = 16k stereo samples),
but extra headroom is recommended for smooth operation), so it is recommended to dedicate the entire second core to MODPlay if possible
(unless you either overclock the Pico or implement some kind of clever (and fast!) task-switching).

The first core is also always woken up 44100 times per second so that it can update the PWM peripheral with the rendered audio. It could be done better (mainly with a DMA), but for a simple demo, this approach works fine.

## Building instructions

Just paste these commands into a Linux shell after you have installed the [Raspberry Pi Pico C/C++ SDK](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html).

It will clone this repository, fetch the MODPlay submodule and build it. Of course, besides the Pico SDK, you'll also need `git`, `cmake` and `make`.

```
git clone --recurse-submodules https://github.com/prochazkaml/pico-MODPlay
mkdir pico-MODPlay/build
cd pico-MODPlay/build
cmake ..
make -j4
```

After that, you should see `main.uf2` in the build directory.
Just plug your Pico into a free USB slot while holding down the BOOTSEL button, mount it and copy this file over.

## Credits

This repo contains (in `tune.h`) music from Sanity's Interference demo (composed by Virgill). Grab it [here](https://modarchive.org/index.php?request=view_by_moduleid&query=68804).
