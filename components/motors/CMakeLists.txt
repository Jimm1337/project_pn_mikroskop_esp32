idf_component_register(SRCS "Motors.cpp" "CommandMotor.cpp"
                    INCLUDE_DIRS "include" "../../main/include" "../../main" REQUIRES commands)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Wshadow -Wconversion -Werror)
