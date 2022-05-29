# pico-MODPlay

A simple .mod player for the Raspberry Pi Pico based on my [MODPlay library](https://github.com/prochazkaml/MODPlay).

In the default configuration, the audio buffer is 64 kB, as it also lights up the LED anytime it's rendering new audio, so the huge buffer is used to make the flashing more visible.
Of course, you can tune it down to something more "sane" (1 kB should do the job just fine).

## Why?

Because I was bored.

And because I needed to prove a point to my ICT teacher that my library could indeed run on a stock, non-overclocked Pico. That's all there is to it.

## Limitations

The main one is the available memory. As MODPlay modifies the song data when it's initialized, it has to be copied to RAM first, and there is not a ton of it on the Pico
(measly 264 kilobytes, and not all of that is available for the song – some is taken up by the audio buffer, the stack etc.).

And it is still quite CPU intensive, so you might just as well forget about the second core for any uses other than MODPlay while playing on a non-overclocked Pico.
If you overclock, you might get some headroom for other stuff.

## Building instructions

Just paste these commands into a Linux shell after you have installed the [Raspberry Pi Pico C/C++ SDK](https://www.raspberrypi.com/documentation/microcontrollers/c_sdk.html).

It will clone this repository, fetch the MODPlay submodule and build it. Of course, besides the Pico SDK, you'll also need `git`, `cmake` and `make`.

```
git clone --recurse-submodules https://github.com/prochazkaml/pico-MODPlay
mkdir MODPlay/build
cd MODPlay/build
cmake ..
make -j4
```

After that, you should see `main.uf2` in the build directory.
Just plug your Pico into a free USB slot while holding down the BOOTSEL button, mount it and copy this file over.

## Credits

This repo contains (in `tune.h`) music from Sanity's Interference demo (composied by Virgill). Grab it [here](https://modarchive.org/index.php?request=view_by_moduleid&query=68804).
