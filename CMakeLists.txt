#
# Copyright 2025, Aleksander Płomiński
# Created from scratch, based on the original work by Ata Ilhan Kokturk.
# Licensed under the MIT License.
# See LICENSE file for detailed information.
#

cmake_minimum_required(VERSION 3.31)
project(NotDefteri_CPP)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(NotDefteri_CPP NotDefteri_CPP/NotDefteri_CPP.cpp NotDefteri_CPP/AppFunctions.cpp NotDefteri_CPP/NoteStruct.cpp)