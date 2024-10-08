# ---- local ----

# find_package(fetalib REQUIRED)
# message("-- Package \"fetalib\" found: ${fetalib_FOUND}")
# target_link_libraries(stampbc_lib fetalib::fetalib)

# ---- remote ----

include(FetchContent)

FetchContent_Declare(
  fetalib
  GIT_REPOSITORY https://github.com/SPoovathumkadavil/fetalib.git
  GIT_TAG v0.1.5
)
FetchContent_MakeAvailable(fetalib)
target_link_libraries(stampbc_lib PUBLIC fetalib::cli)

FetchContent_Declare(
  pbtokenizer
  GIT_REPOSITORY https://github.com/SPoovathumkadavil/pbtokenizer.git
  GIT_TAG v0.1.9.1
)
FetchContent_MakeAvailable(pbtokenizer)
target_link_libraries(stampbc_lib PUBLIC pbtokenizer::tokenizer)

# ---- raylib ----
# FetchContent_Declare(
#     raylib
#     GIT_REPOSITORY https://github.com/raysan5/raylib.git
# )
# FetchContent_MakeAvailable(raylib)
# target_link_libraries(stampbc_lib raylib)

# ---- fmt ----
# FetchContent_Declare(
#   fmt
#   GIT_REPOSITORY https://github.com/fmtlib/fmt
#   GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281) # 10.2.1
# FetchContent_MakeAvailable(fmt)
# target_link_libraries(stampbc_lib fmt::fmt)
