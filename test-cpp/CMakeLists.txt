cmake_minimum_required (VERSION 3.12)

project (JRA-2.1.1_dummy_fmu_test)

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


add_executable(test-deterministic test.cpp)
target_compile_definitions(test-deterministic
  PRIVATE INSTANTIATION_TOKEN="{a67992a0-a385-11eb-aea4-00155d0bce5e}"
  PRIVATE RESOURCE_LOCATION="file:///"
)
target_include_directories(test-deterministic PRIVATE include ${PROJECT_SOURCE_DIR}/../fmi3/include)
target_link_libraries(test-deterministic ${CMAKE_DL_LIBS} 
  ${PROJECT_SOURCE_DIR}/../fmi3/build/temp/Pipeline_deterministic/binaries/${FMI_PLATFORM}/Pipeline_deterministic.so
)


add_executable(test-unpredictable test.cpp)
target_compile_definitions(test-unpredictable
  PRIVATE INSTANTIATION_TOKEN="{58210e20-a83b-11eb-82ba-00155d0450ce}"
  PRIVATE RESOURCE_LOCATION="file:///"
)
target_include_directories(test-unpredictable PRIVATE include ${PROJECT_SOURCE_DIR}/../fmi3/include)
target_link_libraries(test-unpredictable ${CMAKE_DL_LIBS} 
  ${PROJECT_SOURCE_DIR}/../fmi3/build/temp/Pipeline_unpredictable/binaries/${FMI_PLATFORM}/Pipeline_unpredictable.so
)
