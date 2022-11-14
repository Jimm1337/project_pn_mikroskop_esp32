idf_component_register(SRCS "project_pn_mikroskop_esp32.cpp"
                    INCLUDE_DIRS ".")

target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Wshadow -Wconversion -Werror)
