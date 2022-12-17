idf_component_register(SRCS "Actions.cpp" INCLUDE_DIRS "include" "../../main/include" "../../main")

target_compile_options(${COMPONENT_LIB} PRIVATE -Wall -Wextra -Wshadow -Wconversion -Werror)
