cmake_minimum_required(VERSION 3.13)

set(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS -pthread)

project(Cpp)

add_executable(main src/main.cpp src/ood.cpp src/ood.h src/map.cpp src/Memory/move_and_forward.h src/initializer_list.cc src/Memory/class_member_dtor_order.cc src/Memory/std_move.h src/DeducingTypes/template_type_deduction.h)
