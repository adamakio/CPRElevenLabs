cmake_minimum_required(VERSION 3.10)

set(CMAKE_TOOLCHAIN_FILE "C:/Users/16478/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "Vcpkg toolchain file")

project(CPRElevenLabs)

find_package(cpr CONFIG REQUIRED)
find_package(portaudio CONFIG REQUIRED)

add_executable(CPRElevenLabs CPRElevenLabs.cpp CPRElevenLabs.h)

target_link_libraries(CPRElevenLabs PRIVATE cpr::cpr)
target_link_libraries(CPRElevenLabs PRIVATE portaudio)
