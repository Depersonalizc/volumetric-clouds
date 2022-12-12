# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "CMakeFiles/StaticGLEW_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/StaticGLEW_autogen.dir/ParseCache.txt"
  "CMakeFiles/volumetric-clouds_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/volumetric-clouds_autogen.dir/ParseCache.txt"
  "StaticGLEW_autogen"
  "volumetric-clouds_autogen"
  )
endif()
