# project_pn_mikroskop_esp32
ESP32 app for controlling the microscope and lightning.

## Build guide:
 * Get esp-idf from GitHub
```shell
$ idf.py build
```

## For CLion:
 * add env variable IDF_PATH to cmake

## Troubleshooting:
 * idf.py no python -> make sure 'python' is in PATH
 * idf.py no command -> rerun install.sh and . ./export.sh in esp-idf
 * Clion problems -> refer to https://www.jetbrains.com/help/clion/esp-idf.html