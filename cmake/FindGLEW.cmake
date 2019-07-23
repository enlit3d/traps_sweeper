#

# Try to find GLEW library and include path.
# Once done this will define
#
# GLEW_FOUND
# GLEW_INCLUDE_DIR
# GLEW_LIBRARY
# GLEW_SOURCE
#

include(FindPackageHandleStandardArgs)

if (WIN32)
    find_path( GLEW_INCLUDE_DIR
        NAMES
            GL/glew.h
        PATHS
            ${PROJECT_SOURCE_DIR}/extern/glew/include
            DOC "The directory where GL/glew.h resides" )
    find_file( GLEW_SOURCE
        NAMES
            glew.c
        PATHS
            ${PROJECT_SOURCE_DIR}/extern/glew/src
            DOC "The directory where GL/glew.c resides" )
    if(ARCH STREQUAL "x86")
      find_library( GLEW_LIBRARY
          NAMES
              glew GLEW glew32s glew32
          PATHS
              ${PROJECT_SOURCE_DIR}/extern/glew/bin/Release
              ${PROJECT_SOURCE_DIR}/extern/glew/lib/Release
              ${PROJECT_SOURCE_DIR}/extern/glew/lib/Release/x86
              ${PROJECT_SOURCE_DIR}/extern/glew/lib/Release/win32
              DOC "The GLEW library")
    else()
      find_library( GLEW_LIBRARY
          NAMES
              glew GLEW glew32s glew32
          PATHS
              ${PROJECT_SOURCE_DIR}/extern/glew/bin/Release
              ${PROJECT_SOURCE_DIR}/extern/glew/lib/Release/x64
              ${PROJECT_SOURCE_DIR}/extern/glew/lib/Release
              DOC "The GLEW library")
    endif()
endif ()

if (${CMAKE_HOST_UNIX})
    find_path( GLEW_INCLUDE_DIR
        NAMES
            GL/glew.h
        PATHS
            ${GLEW_LOCATION}/include
            $ENV{GLEW_LOCATION}/include
            /usr/include
            /usr/local/include
            /sw/include
            /opt/local/include
            NO_DEFAULT_PATH
            DOC "The directory where GL/glew.h resides"
    )
    find_library( GLEW_LIBRARY
        NAMES
            GLEW glew
        PATHS
            ${GLEW_LOCATION}/lib
            $ENV{GLEW_LOCATION}/lib
            /usr/lib64
            /usr/lib
            /usr/local/lib64
            /usr/local/lib
            /sw/lib
            /opt/local/lib
            NO_DEFAULT_PATH
            DOC "The GLEW library")
endif ()

if (GLEW_INCLUDE_DIR AND EXISTS "${GLEW_INCLUDE_DIR}/GL/glew.h")

   file(STRINGS "${GLEW_INCLUDE_DIR}/GL/glew.h" GLEW_4_2 REGEX "^#define GL_VERSION_4_2.*$")
   if (GLEW_4_2)
       SET(OPENGL_4_2_FOUND TRUE)
   else ()
       message(WARNING
       "glew-1.7.0 or newer needed for supporting OpenGL 4.2 dependent features"
       )
   endif ()

   file(STRINGS "${GLEW_INCLUDE_DIR}/GL/glew.h" GLEW_4_3 REGEX "^#define GL_VERSION_4_3.*$")
   if (GLEW_4_3)
       SET(OPENGL_4_3_FOUND TRUE)
   else ()
       message(WARNING
       "glew-1.9.0 or newer needed for supporting OpenGL 4.3 dependent features"
       )
   endif ()

endif ()

if(GLEW_SOURCE)
find_package_handle_standard_args(GLEW DEFAULT_MSG
    GLEW_INCLUDE_DIR
    GLEW_SOURCE
)
else()
find_package_handle_standard_args(GLEW DEFAULT_MSG
    GLEW_INCLUDE_DIR
    GLEW_LIBRARY
)
endif()

mark_as_advanced( GLEW_FOUND )