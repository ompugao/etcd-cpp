# -*- coding: utf-8 -*-
include_directories(${ETCD_CPP_INCLUDE_LOCAL_DIRS})# ${CURL_INCLUDE_DIRS} ${libzmq_INCLUDE_DIRS})
link_directories(${ETCD_CPP_LINK_DIRS})# ${libzmq_LIBRARY_DIRS})

set(SOURCE_FILES client.cpp)

add_library(libetcd-cpp SHARED ${SOURCE_FILES})
if( EXTRA_MSVC_DEPEND )
  message(STATUS "adding msvc_boost dependency")
  add_dependencies(libetcd-cpp ${EXTRA_MSVC_DEPEND})
endif()
set_target_properties(libetcd-cpp PROPERTIES COMPILE_FLAGS "${EXTRA_COMPILE_FLAGS} ${Boost_CFLAGS} -DETCD_CPP_DLL_EXPORTS -DETCD_CPP_DLL" LINK_FLAGS ""
  OUTPUT_NAME etcd-cpp${ETCD_CPP_LIBRARY_SUFFIX}
  SOVERSION 0 # always have it 0 since we're including the soversion as part of the library name
  VERSION ${ETCD_CPP_VERSION}
  CLEAN_DIRECT_OUTPUT 1
  )
target_link_libraries(libetcd-cpp ${Boost_THREAD_LIBRARY} ${Boost_DATE_TIME_LIBRARY}  ${EXTRA_LIBRARIES})# ${libzmq_LIBRARIES})
if( MSVC )
  install(TARGETS libetcd-cpp RUNTIME DESTINATION bin LIBRARY DESTINATION bin ARCHIVE DESTINATION lib${LIB_SUFFIX})
else()
  install(TARGETS libetcd-cpp DESTINATION lib${LIB_SUFFIX})
endif()

macro(build_sample name)
  add_executable(${name} ${name}.cpp)                                                                                                                          
  add_dependencies(${name} libetcd-cpp)
  target_link_libraries (${name} libetcd-cpp boost_program_options pthread)
  install(TARGETS ${name} DESTINATION bin)
endmacro(build_sample)

build_sample(main)
