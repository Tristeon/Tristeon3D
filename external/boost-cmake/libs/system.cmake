_add_boost_lib(
  NAME system
  SOURCES
    ${BOOST_SOURCE}/libs/system/error_code.cpp
  DEFINE_PRIVATE
    BOOST_SYSTEM_STATIC_LINK=1
)