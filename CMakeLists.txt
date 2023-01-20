cmake_minimum_required(VERSION 3.5)
project(sync_files
        VERSION 2.0.1
        DESCRIPTION "Vergelicht files miteinander wie wenpas file search"
        LANGUAGES CXX)

add_executable(sync_files_binary main.cpp)
# target_compile_features(sync_files_binary PRIVATE cxx_std_14)

