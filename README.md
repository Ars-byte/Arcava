# Arcava

**English** | [Español](README.es.md)

A fast, dependency-free console audio visualizer written in C++17.

Arcava captures audio, runs an FFT over a sliding window and paints log-spaced
frequency bars that fall and decay like a classic spectrum analyzer. It is a
single self-contained program with no runtime dependencies beyond an optional
`parec` (PulseAudio/PipeWire) for live capture.



## Features

- Real-time FFT spectrum analyzer at 60 fps
- Logarithmic frequency bands (50 Hz - 10 kHz)
- smooth falloff and decay for a natural bounce
- Auto-gain peak tracking
- Full-terminal-height rendering, no scroll jitter
- Bars auto-sized to the terminal width, or fixed with `-b`
- Adjustable bar width and spacing
- Resizes live on `SIGWINCH`
- Reads live system audio, or any PCM source from stdin

## Previews

<img width="616" height="268" alt="image" src="https://github.com/user-attachments/assets/12f0c339-3251-4d00-aa60-9dd89e2a6398" />


## Install

Clone and build:

```sh
git clone https://github.com/Ars-byte/Arcava.git
cd Arcava
make
```

Then put the binary somewhere in your `PATH`:

```sh
mkdir -p ~/.local/bin
cp Arcava ~/.local/bin/
```

Prebuilt binaries and the source tarball are also on the
[releases page](https://github.com/Ars-byte/Arcava/releases/latest).

## Build

```sh
make
```

or manually:

```sh
g++ -O2 -std=c++17 -Wall -Wextra src/*.cpp -o Arcava -lm
```

## Usage

If stdin is a terminal, Arcava captures system audio automatically through
PulseAudio/PipeWire (`parec`):

```sh
./Arcava
```

Otherwise it reads raw signed 16-bit little-endian PCM from stdin, so you can
pipe any source:

```sh
parec --format=s16le --rate=44100 --channels=2 | ./Arcava
arecord -f S16_LE -r 44100 -c 2 | ./Arcava -r 44100 -c 2
ffmpeg -i song.flac -f s16le -ac 2 -ar 44100 - | ./Arcava -r 44100 -c 2
```

### Options

```
-r <rate>      sample rate (default 44100)
-c <channels>  channels of the input (default 2)
-d <device>    parec device (default @DEFAULT_MONITOR@)
-b <bars>      number of bars (0 = auto, fit terminal width)
-w <width>     width of each bar in cells (default 1)
-g <gap>       minimum gap between bars in cells (default 1)
-h             show help
```

Press `Ctrl+C` to quit.

## How it works

Audio is read into a sliding window of 4096 samples that advances by
`rate / 60` samples per frame, giving a 60 fps refresh with overlap. A Hann
window is applied, an in-place radix-2 FFT is computed, and the magnitude
spectrum is folded into logarithmically spaced bands. Each band is normalized
against a decaying peak estimate, gated to remove noise, then passed through a
parabolic falloff and an IIR integrator so bars rise fast and fall smoothly.
The result is drawn as Unicode block characters and flushed to the terminal in
a single write per frame.

## Project layout

```
.
├── Makefile
├── README.md
├── scripts
│   └── package-release.sh
├── src
│   ├── main.cpp        entry point, CLI and render loop
│   ├── audio.hpp/.cpp  capture: live parec or stdin PCM
│   ├── fft.hpp/.cpp    in-place radix-2 FFT
│   ├── analyzer.hpp/.cpp  bands, normalization and smoothing
│   ├── render.hpp/.cpp    terminal bar rendering
│   └── term.hpp/.cpp      terminal size, raw output and signals
└── previews
    └── preview.svg
```

## Requirements

- A C++17 compiler (g++ or clang++)
- An audio backend providing `parec` for live capture (optional)

## License

MIT
