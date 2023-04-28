# pull library name from directory name
get_filename_component(libname ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" libname ${libname})

message(STATUS "Adding the libname: ${libname}")

set(libdeps
    "${libdeps}"
    ${CMAKE_CURRENT_LIST_DIR}/Inc
)

# Define library
set ( srcs
    "${srcs}"
    ${CMAKE_CURRENT_LIST_DIR}/Src/main.c
    ${CMAKE_CURRENT_LIST_DIR}/Src/freertos.c
    ${CMAKE_CURRENT_LIST_DIR}/Src/stm32f4xx_it.c
    ${CMAKE_CURRENT_LIST_DIR}/Src/stm32f4xx_hal_msp.c
    ${CMAKE_CURRENT_LIST_DIR}/Src/stm32f4xx_hal_timebase_tim.c
    ${CMAKE_CURRENT_LIST_DIR}/Src/system_stm32f4xx.c
)

set (srcs "${srcs}" PARENT_SCOPE)
set (libdeps "${libdeps}" PARENT_SCOPE)
