cmake_minimum_required(VERSION 3.12)

project(JRA-2.1.1_dummy_fmu)

set(MODEL_NAMES Pipeline_deterministic Pipeline_unpredictable)

set(FMI_VERSION 3)

if(MSVC)
  string(REPLACE "/MD"  "/MT"  CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
  string(REPLACE "/MDd" "/MTd" CMAKE_C_FLAGS_DEBUG   "${CMAKE_C_FLAGS_DEBUG}")
  add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

if(WIN32)
  set(FMI_PLATFORM windows)
elseif(APPLE)
  set(FMI_PLATFORM darwin)
else()
  set(FMI_PLATFORM linux)
endif()

if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
  set(FMI_PLATFORM x86_64-${FMI_PLATFORM})
else()
  set(FMI_PLATFORM i686-${FMI_PLATFORM})
endif()

MESSAGE("## FMI_PLATFORM: " ${FMI_PLATFORM})

add_compile_definitions(FMI_VERSION=${FMI_VERSION})

## If the FMU is is compiled in a static link library, every "real" function name
## is constructed by prepending the function name by "FMI3_FUNCTION_PREFIX". For
## FMUs compiled in a DLL/sharedObject, the "actual" function names are used and
## "FMI3_FUNCTION_PREFIX" must not be defined.
#add_compile_definitions(FMI3_FUNCTION_PREFIX=${MODEL_NAME}_)

foreach(MODEL_NAME ${MODEL_NAMES})

  set(TARGET_NAME ${MODEL_NAME})

  SET(HEADERS
    ${PROJECT_SOURCE_DIR}/fmus/${MODEL_NAME}/${MODEL_NAME}.h
    ${PROJECT_SOURCE_DIR}/include/fmi3Functions.h
    ${PROJECT_SOURCE_DIR}/include/fmi3FunctionTypes.h
    ${PROJECT_SOURCE_DIR}/include/fmi3PlatformTypes.h
    ${PROJECT_SOURCE_DIR}/include/FMUMode.h
    ${PROJECT_SOURCE_DIR}/include/AllowedFMUMode.h
    ${PROJECT_SOURCE_DIR}/include/InstanceBase.h
  )

  SET(SOURCES
    ${PROJECT_SOURCE_DIR}/fmus/${MODEL_NAME}/${MODEL_NAME}.cpp
    ${PROJECT_SOURCE_DIR}/src/fmi${FMI_VERSION}Functions.cpp
    ${PROJECT_SOURCE_DIR}/src/AllowedFMUMode.cpp
    ${PROJECT_SOURCE_DIR}/src/InstanceBase.cpp
  )

  add_library(${TARGET_NAME} SHARED
    ${HEADERS}
    ${SOURCES}
    ${PROJECT_SOURCE_DIR}/fmus/${MODEL_NAME}/FMI${FMI_VERSION}.xml
  )

  target_compile_definitions(${TARGET_NAME}
     PRIVATE INSTANCE_TYPE=${MODEL_NAME}
     PRIVATE INSTANCE_TYPE_INCLUDE="${MODEL_NAME}.h"
  )

  file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dist)

  target_include_directories(${TARGET_NAME} PRIVATE include ${PROJECT_SOURCE_DIR}/fmus/${MODEL_NAME})

  set(FMU_BUILD_DIR ${PROJECT_BINARY_DIR}/temp/${MODEL_NAME})

  set_target_properties(${TARGET_NAME} PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY         "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
      RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
      RUNTIME_OUTPUT_DIRECTORY_RELEASE "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
      LIBRARY_OUTPUT_DIRECTORY         "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
      LIBRARY_OUTPUT_DIRECTORY_DEBUG   "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
      LIBRARY_OUTPUT_DIRECTORY_RELEASE "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
      ARCHIVE_OUTPUT_DIRECTORY         "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
      ARCHIVE_OUTPUT_DIRECTORY_DEBUG   "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
      ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${FMU_BUILD_DIR}/binaries/${FMI_PLATFORM}"
  )

  set_target_properties(${TARGET_NAME} PROPERTIES PREFIX "")
  set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME ${MODEL_NAME})

  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib)

  # modelDescription.xml
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_CURRENT_SOURCE_DIR}/fmus/${MODEL_NAME}/FMI${FMI_VERSION}.xml
    "${FMU_BUILD_DIR}/modelDescription.xml"
  )

  set(ARCHIVE_FILES "modelDescription.xml" "binaries")

  # create ZIP archive
  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E tar "cfv" ${CMAKE_CURRENT_BINARY_DIR}/dist/${MODEL_NAME}.fmu --format=zip
    ${ARCHIVE_FILES}
    WORKING_DIRECTORY ${FMU_BUILD_DIR}
  )

endforeach(MODEL_NAME)
