cmake_minimum_required (VERSION 2.6)

set( ENV{PKG_CONFIG_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/local/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}:")
if (UNIX)
    find_package(PkgConfig REQUIRED)
endif()

project(euglenamovie)

macro(CHECK_MODULE module_name define)
  set(${define} 0)
  if(PKG_CONFIG_FOUND)
    set(ALIAS               ALIASOF_${module_name})
    set(ALIAS_FOUND                 ${ALIAS}_FOUND)
    set(ALIAS_INCLUDE_DIRS   ${ALIAS}_INCLUDE_DIRS)
    set(ALIAS_LIBRARY_DIRS   ${ALIAS}_LIBRARY_DIRS)
    set(ALIAS_LIBRARIES         ${ALIAS}_LIBRARIES)

    PKG_CHECK_MODULES(${ALIAS} ${module_name})

    if(${ALIAS_FOUND})
      set(${define} 1)
      foreach(P "${ALIAS_INCLUDE_DIRS}")
        if(${P})
          list(APPEND MODULE_INCLUDE_DIRS ${${P}})
        endif()
      endforeach()

      foreach(P "${ALIAS_LIBRARY_DIRS}")
        if(${P})
          list(APPEND MODULE_LIBRARY_DIRS ${${P}})
        endif()
      endforeach()
      list(APPEND MODULE_LIBRARIES ${${ALIAS_LIBRARIES}})
    endif()
  endif()
endmacro()

CHECK_MODULE(libturbojpeg HAVE_TURBO_JPEG)

foreach(P ${MODULE_INCLUDE_DIRS})
    include_directories(${P})
endforeach()

if( NOT WIN32 )
    set( UNIXFLAGS "-std=c++11" )
endif(WIN32)

link_directories(${MODULE_LIBRARY_DIRS})
add_executable(euglenamovie moviemaker.cpp )
target_link_libraries(euglenamovie ${MODULE_LIBRARIES})
install (TARGETS euglenamovie DESTINATION bin)
