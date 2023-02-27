cmake_minimum_required(VERSION 3.14)
project(auto-subs C)

add_subdirectory(lib/glfw)
add_subdirectory(lib/FFmpeg)

add_definitions(-DGL_SILENCE_DEPRECATION)

if(APPLE)
    list(APPEND EXTRA_LIBS
        "-framework OpenGL")
elseif(WIN32)
    list(APPEND EXTRA_LIBS
        "-lglu32 -lopengl32")
    set(CMAKE_EXE_LINKER_FLAGS 
        "-std=gnu99 -static -static-libgcc mwindows")
else()
    list(APPEND EXTRA_LIBS
        "-lGL -lGLU -lX11")
endif()

list(APPEND SOURCES
    # src/main.c 
    # src/demux.c
    # src/load_frame.c
    src/av_player.c
    src/allocate_frame.c
    src/find_stream.c
    src/load_frame.c
    src/open_codec.c
    src/open_input_file.c
    # src/open_output_file.c
    # src/transcode_aac.c
)

add_executable(auto-subs ${SOURCES})
target_link_libraries(auto-subs FFmpeg glfw ${EXTRA_LIBS})