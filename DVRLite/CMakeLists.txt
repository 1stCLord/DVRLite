# CMakeList.txt : CMake project for DVRLite, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

find_package(OpenSSL REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)
find_package(oatpp CONFIG REQUIRED)
#find_package(CURL CONFIG REQUIRED)
if(MSVC)
find_package(FFmpeg COMPONENTS AVCODEC AVFORMAT AVUTIL AVDEVICE REQUIRED)
else()
find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h REQUIRED)
find_library(AVCODEC_LIBRARY avcodec REQUIRED)
find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h REQUIRED)
find_library(AVFORMAT_LIBRARY avformat REQUIRED)
find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h REQUIRED)
find_library(AVUTIL_LIBRARY avutil REQUIRED)
find_path(AVDEVICE_INCLUDE_DIR libavdevice/avdevice.h REQUIRED)
find_library(AVDEVICE_LIBRARY avdevice REQUIRED)
set(FFMPEG_INCLUDE_DIRS ${AVCODEC_INCLUDE_DIR} ${AVFORMAT_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR} ${AVDEVICE_INCLUDE_DIR})
set(FFMPEG_LIBRARIES ${AVCODEC_LIBRARY} ${AVFORMAT_LIBRARY} ${AVUTIL_LIBRARY} ${AVDEVICE_LIBRARY})
endif()

# Add source to this project's executable.
file( GLOB_RECURSE DVR_SOURCES *.cpp )
file( GLOB_RECURSE DVR_SOURCES_C *.c )
file( GLOB_RECURSE DVR_HEADERS *.hpp *.h )

add_executable (DVRLite ${DVR_SOURCES} ${DVR_SOURCES_C} ${DVR_HEADERS})

if(MSVC)
target_compile_options(DVRLite PRIVATE -bigobj)
endif()
target_compile_definitions(DVRLite PRIVATE WITH_OPENSSL=1, WITH_DOM=1 ${FFMPEG_DEFINITIONS})
target_include_directories(DVRLite PRIVATE ${OPENSSL_INCLUDE_DIR} ${CMAKE_CURRENT_LIST_DIR} ${FFMPEG_INCLUDE_DIRS})
target_link_libraries(DVRLite PRIVATE jsoncpp_lib oatpp::oatpp OpenSSL::SSL OpenSSL::Crypto ${FFMPEG_LIBRARIES})
install(TARGETS DVRLite DESTINATION bin)
# TODO: Add tests and install targets if needed.
