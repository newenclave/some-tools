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


    get_filename_component(out_FILE ${next_proto} NAME_WE)
    get_filename_component(in_PATH  ${next_proto} PATH)

    set( OUTPUT_FILES 
                ${GENERATED_FILES_OUTPUT}/${out_FILE}.pb.h 
                ${GENERATED_FILES_OUTPUT}/${out_FILE}.pb.cc 
       )

    add_custom_command(
        COMMAND 
                ${PROTOBUF_PROTOC_EXECUTABLE} 
                        --proto_path=${PROTOBUF_INCLUDE_DIRS}
                        --proto_path=${POTOBUF_IMPORT_DIRS}
                        --proto_path=${in_PATH}
                        --cpp_out=${GENERATED_FILES_OUTPUT}
                        ${next_proto}
        WORKING_DIRECTORY
                ${GENERATED_FILES_OUTPUT}
        OUTPUT
                ${OUTPUT_FILES}
        DEPENDS
                ${next_proto}
        )

    set_source_files_properties(
        ${OUTPUT_FILES} 
        PROPERTIES
        GENERATED TRUE
    )

    list( APPEND src_list ${OUTPUT_FILES})
    list( APPEND src_list ${next_proto})

endforeach()

list(APPEND src_list test.cpp)
include_directories( test-cmake ${Boost_INCLUDE_DIRS} ${GENERATED_FILES_OUTPUT})
add_executable( test-cmake ${src_list} )

target_link_libraries(test-cmake ${PROTOBUF_LIBRARIES} ${Boost_LIBRARIES})

#c:\test-cmake\build>cmake -G "Visual Studio 9 2008"  -DBOOST_INCLUDEDIR="C:\SDK\boost\Boost_1.53\include" -DBOOST_LIBRARYDIR="C:\SDK\boost\Boost_1.53\lib
#" -DPROTOBUF_INCLUDE_DIR="C:\SDK\ProtoBuf\ProtoBuf_2.4.1\include" -DPROTOBUF_LIBRARY="C:\SDK\ProtoBuf\libprotobuf-mt.lib" -DPROTOBUF_PROTOC_EXECUTABLE="C:\SDK\Prot
#oBuf\ProtoBuf_2.4.1\bin\protoc.exe" ../
