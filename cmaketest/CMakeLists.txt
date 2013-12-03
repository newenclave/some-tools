
project(test-cmake)

cmake_minimum_required(VERSION 2.8.8)

find_package(Protobuf REQUIRED )

set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)

find_package( Boost REQUIRED COMPONENTS system date_time thread )

file(GLOB protofiles *.proto)

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")
endif() 

set( GENERATED_FILES_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/gen )
file(MAKE_DIRECTORY ${GENERATED_FILES_OUTPUT}) 

foreach(next_proto ${protofiles})


    GET_FILENAME_COMPONENT(out_FILE ${next_proto} NAME_WE)
    GET_FILENAME_COMPONENT(in_PATH  ${next_proto} PATH)

    set( OUTPUT_HEADER_NAME ${GENERATED_FILES_OUTPUT}/${out_FILE}.pb.h )
    set( OUTPUT_SOURCE_NAME ${GENERATED_FILES_OUTPUT}/${out_FILE}.pb.cc )

    add_custom_command(
        OUTPUT 
                ${OUTPUT_HEADER_NAME} 
                ${OUTPUT_SOURCE_NAME}
        COMMAND 
                 ${PROTOBUF_PROTOC_EXECUTABLE} 
                        --proto_path=${PROTOBUF_INCLUDE_DIRS}
                        --proto_path=${POTOBUF_IMPORT_DIRS}
                        --proto_path=${in_PATH}
                        --cpp_out=${GENERATED_FILES_OUTPUT}
                        ${CMAKE_CURRENT_SOURCE_DIR}/${out_FILE}.proto
        DEPENDS
                ${CMAKE_CURRENT_SOURCE_DIR}/${out_FILE}.proto
        WORKING_DIRECTORY 
                ${GENERATED_FILES_OUTPUT}
        )

    set_source_files_properties(
        ${OUTPUT_HEADER_NAME} ${OUTPUT_SOURCE_NAME}
        PROPERTIES
        GENERATED TRUE
    )

    list(APPEND src_list ${OUTPUT_HEADER_NAME} ${OUTPUT_SOURCE_NAME} )
    list(APPEND src_list ${next_proto})

endforeach()

list(APPEND src_list test.cpp)
include_directories( test-cmake ${Boost_INCLUDE_DIRS} ${GENERATED_FILES_OUTPUT} )
add_executable( test-cmake ${src_list} )

target_link_libraries(test-cmake ${PROTOBUF_LIBRARIES} ${Boost_LIBRARIES})

