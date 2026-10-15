# Arcava

A fast, dependency-free console audio visualizer written in C++17.

Arcava captures audio, runs an FFT over a sliding window and paints log-spaced
frequency bars that fall and decay like a classic spectrum analyzer. It is a
single self-contained program with no runtime dependencies beyond an optional
`parec` (PulseAudio/PipeWire) for live capture.

```
   █                                                              ▅         
   █                          ▆                                   █         
   █   ▄              █      █        █                          █         
   █   █   ▅   ▄      █   ▆  █   ▄    █   ▃   ▂   ▂   ▁   ▁      ▁   ▁      
```

## Previews

![Arcava preview](previews/preview.svg)

## Install

Download the precompiled binary and drop it into your `PATH`:

```sh
mkdir -p ~/.local/bin
wget https://github.com/Ars-byte/Arcava/releases/latest/download/Arcava -O ~/.local/bin/Arcava
chmod +x ~/.local/bin/Arcava
```

Or download the full tarball (source + binary) and run it:

```sh
wget https://github.com/Ars-byte/Arcava/releases/latest/download/Arcava-v1.0.0.tar.gz
tar xzf Arcava-v1.0.0.tar.gz
cd Arcava-v1.0.0
./Arcava
```

Make sure `~/.local/bin` is in your `PATH`.

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
