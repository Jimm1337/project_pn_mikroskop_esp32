idf_component_register(SRCS "Motors.cpp" "CommandMotor.cpp" "Stepper.cpp"
                    INCLUDE_DIRS "include" "../../main/include" "../../main" REQUIRES driver commands)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Wshadow -Wconversion -Werror)
