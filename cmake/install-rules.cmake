install(
    TARGETS stampbc_exe
    RUNTIME COMPONENT stampbc_Runtime
)

if(PROJECT_IS_TOP_LEVEL)
  include(CPack)
endif()
