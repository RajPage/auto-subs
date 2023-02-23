cmake_minimum_required(VERSION 3.14)
project(auto-subs C)

add_subdirectory(lib/glfw)

if(APPLE)
    list(APPEND EXTRA_LIBS "-framework OpenGL")
elseif(WIN32)
    list(APPEND EXTRA_LIBS "-lglu32 -lopengl32")
    set(CMAKE_EXE_LINKER_FLAGS "-std=gnu99 -static -static-libgcc mwindows")
else()
    list(APPEND EXTRA_LIBS "-lGL -lGLU -lX11")
endif()

add_executable(auto-subs src/main.c)
target_link_libraries(auto-subs glfw ${EXTRA_LIBS})