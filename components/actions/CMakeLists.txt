idf_component_register(SRCS "Actions.cpp" "Command.cpp" "CommandMotor.cpp" "CommandLight.cpp"
                    INCLUDE_DIRS "include" "../../main/include")

target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Wshadow -Wconversion -Werror)
