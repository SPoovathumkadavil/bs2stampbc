cmake_minimum_required(VERSION 3.16)

include(cmake/prelude.cmake)

project(
    stampbc
    VERSION 0.1.0
    DESCRIPTION ""
    HOMEPAGE_URL ""
    LANGUAGES CXX
)

include(cmake/project-is-top-level.cmake)
include(cmake/variables.cmake)

# ---- source files ----

include(cmake/sources.cmake)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
  include(cmake/install-rules.cmake)
endif()

# ---- Developer mode ----

if(NOT stampbc_DEVELOPER_MODE)
  return()
elseif(NOT PROJECT_IS_TOP_LEVEL)
  message(
      AUTHOR_WARNING
      "Developer mode is intended for developers of project"
  )
endif()

include(cmake/dev-mode.cmake)
