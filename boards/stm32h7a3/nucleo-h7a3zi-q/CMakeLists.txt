add_subdirectory(src)

set_property(GLOBAL APPEND PROPERTY NUTTX_BOARD_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src)

if(CONFIG_BOARD_LATE_INITIALIZE)
  set_property(GLOBAL PROPERTY LD_SCRIPT
               ${CMAKE_CURRENT_SOURCE_DIR}/scripts/flash.ld)
endif()
