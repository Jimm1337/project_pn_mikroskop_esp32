idf_component_register(SRCS "wifi.cpp"
                    INCLUDE_DIRS "include"
        REQUIRES nvs_flash esp_netif esp_event esp_wifi)
