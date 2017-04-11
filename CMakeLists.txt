cmake_minimum_required(VERSION 2.6)
project(hxx_proj)

#set debug
#set(CMAKE_BUILD_TYPE "Debug")

# Save the command line compile commands in the build output
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)
# View the makefile commands during build
#set(CMAKE_VERBOSE_MAKEFILE on)

#need cxx11 feature
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()
#find opengl,need not
find_package(OpenGL REQUIRED)
set(DEPENDENCIES realsense ${OPENGL_LIBRARIES})

#find opencv 3.2 package,version unsure
find_package(OpenCV 3.2 REQUIRED)
#find_package(OpenCV REQUIRED)
# 添加cmake模块以使用g2o
list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules )
find_package( G2O REQUIRED )
find_package( CSparse REQUIRED )

if(WIN32)
    add_subdirectory(third_party/glfw)
    list(APPEND DEPENDENCIES glfw3)
else()
    # Find glfw header
    find_path(GLFW_INCLUDE_DIR NAMES GLFW/glfw3.h
        PATHS /usr/X11R6/include
              /usr/include/X11
              /opt/graphics/OpenGL/include
              /opt/graphics/OpenGL/contrib/libglfw
              /usr/local/include
              /usr/include/GL
              /usr/include
    )
    # Find glfw library
    find_library(GLFW_LIBRARIES NAMES glfw glfw3
            PATHS /usr/lib64
                  /usr/lib
                  /usr/lib/${CMAKE_LIBRARY_ARCHITECTURE}
                  /usr/local/lib64
                  /usr/local/lib
                  /usr/local/lib/${CMAKE_LIBRARY_ARCHITECTURE}
                  /usr/X11R6/lib
    )
    list(APPEND DEPENDENCIES m ${GLFW_LIBRARIES} ${LIBUSB1_LIBRARIES})
    include_directories(${GLFW_INCLUDE_DIR} 
			${OpenCV_INCLUDE_DIRS}
			${G2O_INCLUDE_DIRS}
			${CSPARSE_INCLUDE_DIR}
			"/usr/local/include/eigen3/"
			)
endif()

add_executable(getPic getPic.cpp)
target_link_libraries(getPic ${DEPENDENCIES})

add_executable(main main.cpp )
target_link_libraries(main 
		      ${DEPENDENCIES} 
		      ${OpenCV_LIBS}
		      g2o_core g2o_stuff g2o_types_sba g2o_csparse_extension
		      ${CSPARSE_LIBRARY}
		      )

# add_executable(test pose_estimation.cpp)
# target_link_libraries( test 
#    ${OpenCV_LIBS}
#    g2o_core g2o_stuff g2o_types_sba g2o_csparse_extension 
#    ${CSPARSE_LIBRARY}
# )

install(TARGETS 
	getPic
	main
	RUNTIME DESTINATION bin)
