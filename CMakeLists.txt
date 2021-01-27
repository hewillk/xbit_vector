cmake_minimum_required(VERSION 3.14)

project(
    xbit_vector
    VERSION 0.0.1
    DESCRIPTION "Space-efficient containers for dibit and quadbit values"
    HOMEPAGE_URL "https://github.com/hewillk/xbit_vector"
    LANGUAGES CXX
)

# ---- Warning guard ----

# Protect dependents from this project's warnings if the guard isn't disabled
set(xbit_vector_warning_guard SYSTEM)
if(xbit_vector_INCLUDE_WITHOUT_SYSTEM)
  set(xbit_vector_warning_guard "")
endif()

# ---- Declare library ----

add_library(xbit_vector INTERFACE)
add_library(xbit_vector::xbit_vector ALIAS xbit_vector)

target_include_directories(
    xbit_vector
    ${xbit_vector_warning_guard}
    INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
)

target_compile_features(xbit_vector INTERFACE cxx_std_20)

# ---- Install ----

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(xbit_vector_include_directory "${CMAKE_INSTALL_INCLUDEDIR}/xbit_vector")

install(
    DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
    DESTINATION "${xbit_vector_include_directory}"
    COMPONENT xbit_vector_Development
)

install(
    TARGETS xbit_vector
    EXPORT xbit_vectorTargets
    INCLUDES DESTINATION "${xbit_vector_include_directory}"
)

write_basic_package_version_file(
    xbit_vector-config-version.cmake
    COMPATIBILITY SameMajorVersion
    ARCH_INDEPENDENT
)

set(xbit_vector_install_cmakedir "${CMAKE_INSTALL_LIBDIR}/cmake/xbit_vector")

install(
    FILES "${PROJECT_BINARY_DIR}/xbit_vector-config-version.cmake"
    DESTINATION "${xbit_vector_install_cmakedir}"
    COMPONENT xbit_vector_Development
)

install(
    EXPORT xbit_vectorTargets
    FILE xbit_vector-config.cmake
    NAMESPACE xbit_vector::
    DESTINATION "${xbit_vector_install_cmakedir}"
    COMPONENT xbit_vector_Development
)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  include(CPack)
endif()
