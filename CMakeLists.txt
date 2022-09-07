cmake_minimum_required(VERSION 3.22)
project(WordChecker C)

set(CMAKE_C_STANDARD 11)

# added compiler flags
# -DEVAL -Wall -Werror -std=gnu11 -O2 -pipe -static
set(GCC_COVERAGE_COMPILE_FLAGS "-DEVAL -Wall -Werror -std=gnu11 -O2 -pipe")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
# end compiler flags
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fsanitize=address -static-libsan")
add_executable(WordChecker main.c)
