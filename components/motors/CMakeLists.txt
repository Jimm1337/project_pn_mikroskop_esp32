idf_component_register(SRCS "motors.cpp"
                    INCLUDE_DIRS "include")

target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Wshadow -Wconversion -Werror)
