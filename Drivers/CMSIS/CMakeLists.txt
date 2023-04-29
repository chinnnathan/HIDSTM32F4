# pull library name from directory name
get_filename_component(libname ${CMAKE_CURRENT_SOURCE_DIR} NAME)
string(REPLACE " " "_" libname ${libname})

# Define library
set ( srcs 
    ${CMAKE_CURRENT_LIST_DIR}/
)