if(__add_ami_dependencies)
	return()
endif()
set(__add_ami_dependencies ON)


find_package(ZLIB REQUIRED)

# Libzip
# ------
set(AMI_LIBZIP_VERSION 1.1.3)
set(LIBZIP_URL         "${THIRD_PARTY_DIRECTORY}/libzip-${AMI_LIBZIP_VERSION}.tar.gz")
set(LIBZIP_CHECKSUM    SHA256=1faa5a524dd4a12c43b6344e618edce1bf8050dfdb9d0f73f3cc826929a002b0)
set(LIBZIP_PREFIX      "${CMAKE_CURRENT_BINARY_DIR}/LIBZIP")
set(LIBZIP_INSTALL_DIR "${LIBZIP_PREFIX}/install")
set(LIBZIP_CONFIGURE   <SOURCE_DIR>/configure --enable-static --with-pic --prefix=<INSTALL_DIR> )
set(LIBZIP_INCLUDE_DIRS "${LIBZIP_INSTALL_DIR}/include")
set(LIBZIP_LIBRARIES
  "${LIBZIP_INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}zip${CMAKE_STATIC_LIBRARY_SUFFIX}")

ExternalProject_Add(LIBZIP
  URL               ${LIBZIP_URL}
  URL_HASH          ${LIBZIP_CHECKSUM}
  BUILD_IN_SOURCE   ON
  CONFIGURE_COMMAND ${LIBZIP_CONFIGURE}
  PREFIX            "${LIBZIP_PREFIX}"
  UPDATE_COMMAND    ""
  INSTALL_DIR       ${LIBZIP_INSTALL_DIR}
  BUILD_BYPRODUCTS  ${LIBZIP_LIBRARIES}
)


add_dependencies(LIB_AMI_STATIC LIBZIP)

target_link_libraries(LIB_AMI_STATIC INTERFACE ${LIBZIP_LIBRARIES})
target_link_libraries(LIB_AMI_STATIC INTERFACE ${ZLIB_LIBRARIES})

target_include_directories(LIB_AMI_STATIC PUBLIC ${LIBZIP_INSTALL_DIR}/include)
target_include_directories(LIB_AMI_STATIC PUBLIC ${LIBZIP_INSTALL_DIR}/lib/libzip/include)

