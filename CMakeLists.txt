project(philosophers)
cmake_minimum_required(VERSION 3.12)
set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS "-Wall -Wextra -Werror -pthread")

file(GLOB philo_one "./philo_one/*.c")
file(GLOB philo_two "./philo_two/*.c")
file(GLOB philo_three "./philo_three/*.c")

include_directories(/usr/local/include)
include_directories(philo_one/)
include_directories(philo_two/)
include_directories(philo_three/)

add_executable(philo_one ${philo_one})
add_executable(philo_two ${philo_two})
add_executable(philo_three ${philo_three})
