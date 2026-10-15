# Arcava

[English](README.md) | **Español**

Visualizador de audio de consola, rápido y sin dependencias, escrito en C++17.

Arcava captura audio, ejecuta una FFT sobre una ventana deslizante y dibuja
barras de frecuencia en escala logarítmica que caen y se desvanecen como un
analizador de espectro clásico. Es un programa autocontenido, sin dependencias
en tiempo de ejecución más allá de `parec` (PulseAudio/PipeWire) opcional para
la captura en vivo.

## Características

- Analizador de espectro FFT en tiempo real a 60 fps
- Bandas de frecuencia logarítmicas (50 Hz - 10 kHz)
- Caída y suavizado suaves para un rebote natural
- Ajuste automático de ganancia con seguimiento de picos
- Render a pantalla completa del terminal, sin saltos de scroll
- Barras ajustadas automáticamente al ancho del terminal, o fijas con `-b`
- Ancho y separación de barras configurables
- Se redimensiona en vivo con `SIGWINCH`
- Lee el audio del sistema, o cualquier fuente PCM por stdin

## Capturas

<img width="616" height="268" alt="image" src="https://github.com/user-attachments/assets/12f0c339-3251-4d00-aa60-9dd89e2a6398" />

## Instalación

Cloná y compilá:

```sh
git clone https://github.com/Ars-byte/Arcava.git
cd Arcava
make
```

Después poné el binario en tu `PATH`:

```sh
mkdir -p ~/.local/bin
cp Arcava ~/.local/bin/
```

También hay binarios precompilados y el tarball del código en la
[página de releases](https://github.com/Ars-byte/Arcava/releases/latest).

## Compilación

```sh
make
```

o manualmente:

```sh
g++ -O2 -std=c++17 -Wall -Wextra src/*.cpp -o Arcava -lm
```

## Uso

Si stdin es una terminal, Arcava captura el audio del sistema automáticamente
a través de PulseAudio/PipeWire (`parec`):

```sh
./Arcava
```

En caso contrario lee PCM crudo de 16 bits little-endian con signo desde stdin,
así que podés pipear cualquier fuente:

```sh
parec --format=s16le --rate=44100 --channels=2 | ./Arcava
arecord -f S16_LE -r 44100 -c 2 | ./Arcava -r 44100 -c 2
ffmpeg -i song.flac -f s16le -ac 2 -ar 44100 - | ./Arcava -r 44100 -c 2
```

### Opciones

```
-r <rate>      frecuencia de muestreo (por defecto 44100)
-c <channels>  canales de la entrada (por defecto 2)
-d <device>    dispositivo de parec (por defecto @DEFAULT_MONITOR@)
-b <bars>      cantidad de barras (0 = auto, ajusta al ancho del terminal)
-w <width>     ancho de cada barra en celdas (por defecto 1)
-g <gap>       separación mínima entre barras en celdas (por defecto 1)
-h             muestra la ayuda
```

Presioná `Ctrl+C` para salir.

## Cómo funciona

El audio se lee en una ventana deslizante de 4096 muestras que avanza
`rate / 60` muestras por frame, dando un refresco de 60 fps con solapamiento.
Se aplica una ventana de Hann, se calcula una FFT radix-2 en el lugar y el
espectro de magnitud se agrupa en bandas espaciadas logarítmicamente. Cada
banda se normaliza contra una estimación de pico que decae, se filtra con un
umbral para eliminar el ruido, y pasa por una caída parabólica y un integrador
IIR para que las barras suban rápido y bajen suave. El resultado se dibuja con
caracteres de bloque Unicode y se vuelca al terminal en una sola escritura por
frame.

## Estructura del proyecto

```
.
├── Makefile
├── README.md
├── README.es.md
├── scripts
│   └── package-release.sh
├── src
│   ├── main.cpp        punto de entrada, CLI y bucle de render
│   ├── audio.hpp/.cpp  captura: parec en vivo o PCM por stdin
│   ├── fft.hpp/.cpp    FFT radix-2 en el lugar
│   ├── analyzer.hpp/.cpp  bandas, normalización y suavizado
│   ├── render.hpp/.cpp    render de barras en el terminal
│   └── term.hpp/.cpp      tamaño del terminal, salida cruda y señales
└── previews
    └── preview.svg
```

## Requisitos

- Un compilador de C++17 (g++ o clang++)
- Un backend de audio que provea `parec` para la captura en vivo (opcional)

## Licencia

MIT
