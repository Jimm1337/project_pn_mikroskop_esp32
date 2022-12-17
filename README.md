# project_pn_mikroskop_esp32

ESP32 app for controlling the microscope and lightning.

## Command Schema:

* LEDS: l(LED)(R)(G)(B)(A), Length: l-3-2-2-2, RGBA in hex
* MOTORS: m(AXIS)(STEPS), Length: m-1-?, AXIS == 'x'||'y'||'z'

### Examples:

* LEDS: l00112121200
* MOTORS: mx-1000, my100

## Setup:
Use [Config file](main/pn_config.h) to configure all settings. Especially Wi-Fi params.

## Build guide:
Get esp-idf from GitHub.

* Build

```shell
$ idf.py build
```

* Flash

```shell
$ idf.py -p (PORT) flash
```

* Monitor

```shell
$ idf.py monitor
```

### For CLion:

* add env variable IDF_PATH to cmake

## Troubleshooting:
 * idf.py no python -> make sure 'python' is in PATH
 * idf.py no command -> rerun install.sh and . ./export.sh in esp-idf
 * Clion problems -> refer to https://www.jetbrains.com/help/clion/esp-idf.html
