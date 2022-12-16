idf_component_register(SRCS "Wifi.cpp"
                    INCLUDE_DIRS "include" "../../main/include" "../../main"
        REQUIRES nvs_flash esp_netif esp_event esp_wifi commands)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Wshadow -Wconversion -Werror)
