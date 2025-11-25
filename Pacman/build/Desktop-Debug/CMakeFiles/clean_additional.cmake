# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/Pacman_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/Pacman_autogen.dir/ParseCache.txt"
  "Pacman_autogen"
  )
endif()
