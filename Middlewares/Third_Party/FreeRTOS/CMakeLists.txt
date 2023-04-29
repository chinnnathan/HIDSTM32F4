# pull library name from directory name
get_filename_component(libname ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" libname ${libname})

message(STATUS "Adding the libname: ${libname}")

set(libdeps
    "${libdeps}"
    ${CMAKE_CURRENT_LIST_DIR}/Source/include
    ${CMAKE_CURRENT_LIST_DIR}/Source/CMSIS_RTOS
    ${CMAKE_CURRENT_LIST_DIR}/Source/portable/GCC/ARM_CM4F
)

# Define library
set ( localSources 
    ${CMAKE_CURRENT_LIST_DIR}/Source/croutine.c
    ${CMAKE_CURRENT_LIST_DIR}/Source/event_groups.c
    ${CMAKE_CURRENT_LIST_DIR}/Source/list.c
    ${CMAKE_CURRENT_LIST_DIR}/Source/queue.c
    ${CMAKE_CURRENT_LIST_DIR}/Source/stream_buffer.c
    ${CMAKE_CURRENT_LIST_DIR}/Source/tasks.c
    ${CMAKE_CURRENT_LIST_DIR}/Source/timers.c
    ${CMAKE_CURRENT_LIST_DIR}/Source/CMSIS_RTOS/cmsis_os.c
    ${CMAKE_CURRENT_LIST_DIR}/Source/portable/MemMang/heap_4.c
    ${CMAKE_CURRENT_LIST_DIR}/Source/portable/GCC/ARM_CM4F/port.c  
)

list( APPEND srcs 
    "${localSources}"
)
set (srcs "${srcs}" PARENT_SCOPE)
set (libdeps "${libdeps}" PARENT_SCOPE)
