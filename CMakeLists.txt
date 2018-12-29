SET(PROJECT_NAME slam)

PROJECT(${PROJECT_NAME})
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
#set(CMAKE_VERBOSE_MAKEFILE ON)


#set(BUILD_TYPE Release)
set(BUILD_TYPE RelWithDebInfo)

set(EXECUTABLE_OUTPUT_PATH bin)
set(LIBRARY_OUTPUT_PATH lib)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# required libraries
#find_package(SuiteParse REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Boost COMPONENTS system thread) 
find_package(OpenCV REQUIRED)

# optional libraries
#find_package(LibZip QUIET)
find_package(Pangolin 0.2 QUIET)




# flags
add_definitions("-DENABLE_SSE")
set(CMAKE_CXX_FLAGS
   "${SSE_FLAGS} -O3 -fPIC -g -std=c++0x -march=native"
#   "${SSE_FLAGS} -O3 -g -std=c++0x -fno-omit-frame-pointer"
)

# Sources files
set(slam_SOURCE_FILES
        ${PROJECT_SOURCE_DIR}/src/util/settings.cpp
        ${PROJECT_SOURCE_DIR}/src/util/Undistort.cpp
        ${PROJECT_SOURCE_DIR}/src/util/globalCalib.cpp
)



include_directories(
  ${PROJECT_SOURCE_DIR}/src 
  ${PROJECT_SOURCE_DIR}/thirdparty/Sophus
  ${EIGEN3_INCLUDE_DIR}
  ${OpenCV_INCLUDE_DIRS}
) 


# decide if we have openCV
if (OpenCV_FOUND)
	message("--- found OpenCV, compiling slam_opencv library.")	
    set(slam_opencv_SOURCE_FILES 	  
	  ${PROJECT_SOURCE_DIR}/src/IO/OpenCV/ImageRW_OpenCV.cpp)
	set(HAS_OPENCV 1)
else ()
	message("--- could not find OpenCV, not compiling slam_opencv library.")
	message("    this means there will be no image display, and image read / load functionality.")	
	set(HAS_OPENCV 0)
endif ()


# decide if we have pangolin
if (Pangolin_FOUND)
	message("--- found PANGOLIN, compiling slam_pangolin library.")
	include_directories( ${Pangolin_INCLUDE_DIRS} ) 
	set(slam_pangolin_SOURCE_FILES 	  
	  ${PROJECT_SOURCE_DIR}/src/IO/Pangolin/PGViewer.cpp)
	set(HAS_PANGOLIN 1)
else ()
	message("--- could not find PANGOLIN, not compiling slam_pangolin library.")
	message("    this means there will be no 3D display / GUI available for dso_dataset.")
	set(slam_pangolin_SOURCE_FILES )
	set(HAS_PANGOLIN 0)
endif ()


# compile main library.
include_directories( ${CSPARSE_INCLUDE_DIR} ${CHOLMOD_INCLUDE_DIR}) 
add_library(slam ${slam_SOURCE_FILES} ${slam_opencv_SOURCE_FILES} ${slam_pangolin_SOURCE_FILES})

#set_property( TARGET slam APPEND_STRING PROPERTY COMPILE_FLAGS -Wall )


# build main executable (only if we have both OpenCV and Pangolin)
if (OpenCV_FOUND AND Pangolin_FOUND)
	message("--- compiling slam.")
	add_executable(slam_dataset ${PROJECT_SOURCE_DIR}/src/main.cpp )
    target_link_libraries(slam_dataset slam boost_system boost_thread cxsparse ${Pangolin_LIBRARIES} ${OpenCV_LIBS})
else()
	message("--- not building slam, since either don't have openCV or Pangolin.")
endif()

