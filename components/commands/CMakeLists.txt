idf_component_register(SRCS "Command.cpp"
        INCLUDE_DIRS "include" "../../main/include" REQUIRES motors lighting)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Wshadow -Wconversion -Werror)