file(GLOB sources *.cpp)

set(name psp)
add_executable(${name} ${sources})

target_link_libraries(${name} lbsr_psp ${COMMON_LIBS})
