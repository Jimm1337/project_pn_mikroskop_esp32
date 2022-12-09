idf_component_register(SRCS "Motors.cpp"
                    INCLUDE_DIRS "include" "../actions/include" "../../main/include")

target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Wshadow -Wconversion -Werror)
