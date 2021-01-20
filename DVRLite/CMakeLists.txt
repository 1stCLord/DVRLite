# CMakeList.txt : CMake project for DVRLite, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

find_package(OpenSSL REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(oatpp CONFIG REQUIRED)
#find_package(CURL CONFIG REQUIRED)
find_package(FFmpeg COMPONENTS AVCODEC AVFORMAT AVUTIL AVDEVICE REQUIRED)

# Add source to this project's executable.
file( GLOB_RECURSE DVR_SOURCES *.cpp )
file( GLOB_RECURSE DVR_SOURCES_C *.c )
file( GLOB_RECURSE DVR_HEADERS *.hpp *.h )

add_executable (DVRLite ${DVR_SOURCES} ${DVR_SOURCES_C} ${DVR_HEADERS})

target_compile_options(DVRLite PRIVATE -bigobj)
target_compile_definitions(DVRLite PRIVATE WITH_OPENSSL=1, WITH_DOM=1 ${FFMPEG_DEFINITIONS})
target_include_directories(DVRLite PRIVATE ${OPENSSL_INCLUDE_DIR} ${CMAKE_CURRENT_LIST_DIR} ${FFMPEG_INCLUDE_DIRS})
target_link_libraries(DVRLite PRIVATE jsoncpp_lib oatpp::oatpp OpenSSL::SSL OpenSSL::Crypto ${FFMPEG_LIBRARIES})
# TODO: Add tests and install targets if needed.
