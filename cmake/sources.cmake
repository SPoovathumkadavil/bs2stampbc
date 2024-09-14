
# ---- declare library ----

file(GLOB_RECURSE stampbc_lib_sources CONFIGURE_DEPENDS "src/stampbc-lib/*.cpp")

add_library(
    stampbc_lib OBJECT
    ${stampbc_lib_sources}
)

target_include_directories(
    stampbc_lib ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/stampbc-lib>"
)

target_compile_features(stampbc_lib PUBLIC cxx_std_17)

# ---- declare executable ----

file(GLOB_RECURSE stampbc_sources CONFIGURE_DEPENDS "src/stampbc/*.cpp")

add_executable(stampbc_exe ${stampbc_sources})
add_executable(stampbc::exe ALIAS stampbc_exe)

set_property(TARGET stampbc_exe PROPERTY OUTPUT_NAME stampbc)

target_compile_features(stampbc_exe PRIVATE cxx_std_17)

target_link_libraries(stampbc_exe PRIVATE stampbc_lib)

target_include_directories(
    stampbc_exe ${warning_guard}
    PUBLIC
    "\$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/stampbc>"
)

# ---- add external libraries ----

include(cmake/external-lib.cmake)