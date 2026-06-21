cmake_minimum_required(VERSION 3.20)
project(kshooteditor)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(kshooteditor src/EditorMain.cpp)

# Assuming NocoUI and other deps are handled similarly to kshootmania
# We'll need to link them properly later, for now just a stub to check build
