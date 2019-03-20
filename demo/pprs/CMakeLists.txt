file(GLOB sources *.cpp)

set(name pprs)
add_executable(${name} ${sources})

target_link_libraries(${name} lbsr_pprs ${COMMON_LIBS})
