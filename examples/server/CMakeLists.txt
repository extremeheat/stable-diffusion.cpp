set(TARGET sds)
set(CMAKE_CXX_STANDARD 23)

add_executable(${TARGET} main.cpp)
install(TARGETS ${TARGET} RUNTIME)
target_link_libraries(${TARGET} PRIVATE stable-diffusion ${CMAKE_THREAD_LIBS_INIT})
target_compile_features(${TARGET} PUBLIC cxx_std_11)

if (NOT MSVC)
    target_compile_options(${TARGET} PUBLIC -Wno-deprecated-declarations)
endif()