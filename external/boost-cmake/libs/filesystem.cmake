_add_boost_lib(
  NAME filesystem
  SOURCES
    ${BOOST_SOURCE}/libs/filesystem/codecvt_error_category.cpp
    ${BOOST_SOURCE}/libs/filesystem/operations.cpp
    ${BOOST_SOURCE}/libs/filesystem/path.cpp
    ${BOOST_SOURCE}/libs/filesystem/path_traits.cpp
    ${BOOST_SOURCE}/libs/filesystem/portability.cpp
    ${BOOST_SOURCE}/libs/filesystem/unique_path.cpp
    ${BOOST_SOURCE}/libs/filesystem/utf8_codecvt_facet.cpp
    ${BOOST_SOURCE}/libs/filesystem/windows_file_codecvt.cpp
  DEFINE_PRIVATE
    BOOST_FILESYSTEM_STATIC_LINK=1
  LINK
    Boost::system
)