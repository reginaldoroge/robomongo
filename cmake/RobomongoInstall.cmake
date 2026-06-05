# Note for maintainers
# --------------------
#
# Do not use absolute paths in DESTINATION arguments for install() command.
# Because the same install code will be executed again by CPack. And CPack will
# change internally CMAKE_INSTALL_PREFIX to point to some temporary folder
# for package content.
#
#


# Temporary change
set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install"
    CACHE STRING "Install path prefix, prepended onto install directories"
    FORCE)

if(SYSTEM_LINUX)
    set(bin_dir             bin)
    set(lib_dir             lib)
    set(resources_dir       share)
    set(license_dir         .)

    set(qt_plugins_dir      ${lib_dir})
    set(qt_conf_dir         ${bin_dir})
    set(qt_conf_plugins     "../lib")
elseif(SYSTEM_MACOSX)
    set(bundle_name         "dino-robomongo.app")
    set(contents_path       ${bundle_name}/Contents)

    set(bin_dir             ${contents_path}/MacOS)
    set(styles_dir          ${contents_path}/MacOS/styles)
    set(lib_dir             ${contents_path}/Frameworks)
    set(resources_dir       ${contents_path}/Resources)
    set(license_dir         ${resources_dir})

    set(qt_plugins_dir      ${contents_path}/PlugIns/Qt)
    set(qt_conf_dir         ${resources_dir})
    set(qt_conf_plugins     "PlugIns/Qt")
elseif(SYSTEM_WINDOWS)
    set(bin_dir             .)
    set(styles_dir          ${bin_dir}/styles)
    set(lib_dir             .)
    set(resources_dir       ./resources)
    set(license_dir         .)

    set(qt_plugins_dir      ${lib_dir})
    set(qt_conf_dir         ${bin_dir})
    set(qt_conf_plugins     .)
endif()

# Generate qt.conf file
configure_file(
    "${CMAKE_SOURCE_DIR}/install/qt.conf.in"
    "${CMAKE_BINARY_DIR}/qt.conf")

# Install qt.conf file
install(
    FILES       "${CMAKE_BINARY_DIR}/qt.conf"
    DESTINATION "${qt_conf_dir}")

# Install OpenSSL dynamic lib files
if(NOT SYSTEM_WINDOWS)
    get_target_property(OPENSSL_SSL_LIBRARY ssl IMPORTED_LOCATION)
    get_target_property(OPENSSL_CRYPTO_LIBRARY crypto IMPORTED_LOCATION)
    get_filename_component(OPENSSL_SSL_LIBRARY_REAL "${OPENSSL_SSL_LIBRARY}" REALPATH)
    get_filename_component(OPENSSL_CRYPTO_LIBRARY_REAL "${OPENSSL_CRYPTO_LIBRARY}" REALPATH)
endif()

if(SYSTEM_WINDOWS)
    install(
        FILES 
        "${OpenSSL_DIR}/libssl-1_1-x64.dll"
        "${OpenSSL_DIR}/libcrypto-1_1-x64.dll"
        DESTINATION ${bin_dir})
elseif(SYSTEM_MACOSX)
    install(
        FILES 
        "${OPENSSL_SSL_LIBRARY_REAL}"
        "${OPENSSL_CRYPTO_LIBRARY_REAL}"
        DESTINATION ${lib_dir})
elseif(SYSTEM_LINUX)
    install(
        FILES 
        "${OPENSSL_SSL_LIBRARY_REAL}"
        "${OPENSSL_CRYPTO_LIBRARY_REAL}"
        DESTINATION ${lib_dir})         
endif()

# Install binary
install(
    TARGETS robomongo
    RUNTIME DESTINATION ${bin_dir}
    BUNDLE DESTINATION .)

# Install license, copyright and changelogs files
install(
    FILES
        ${CMAKE_SOURCE_DIR}/LICENSE
        ${CMAKE_SOURCE_DIR}/COPYRIGHT
        ${CMAKE_SOURCE_DIR}/CHANGELOG
        ${CMAKE_SOURCE_DIR}/DESCRIPTION
    DESTINATION ${license_dir})

# Install common dependencies
SET(QT_LIBS Core Gui Widgets PrintSupport Network Xml)
set(QT_STYLES_DIR ${Qt5Core_DIR}/../../../plugins/styles/)
set(QT_BIN_DIR ${Qt5Core_DIR}/../../../bin/)
set(QT_RESOURCES_DIR ${Qt5Core_DIR}/../../../resources/)

if(NOT SYSTEM_MACOSX)
    install_qt_lib(${QT_LIBS})
    install_qt_plugins(QGifPlugin QICOPlugin)
    install_icu_libs()
endif()

if(SYSTEM_LINUX)
    install_qt_lib(XcbQpa DBus)
    install_qt_plugins(
        QXcbIntegrationPlugin)
        
    install(
        FILES
            "/usr/lib/x86_64-linux-gnu/libstdc++.so.6"
            "/usr/lib/x86_64-linux-gnu/libstdc++.so.6.0.28"              
        DESTINATION ${lib_dir})
elseif(SYSTEM_MACOSX)
    # Install icon
    install(
        FILES       "${CMAKE_SOURCE_DIR}/install/macosx/robomongo.icns"
        DESTINATION "${resources_dir}")

    find_program(MACDEPLOYQT_EXECUTABLE
        NAMES macdeployqt
        HINTS "${QT_BIN_DIR}" "/opt/homebrew/opt/qt@5/bin" "/usr/local/opt/qt@5/bin")

    set(MONGOCXX_RUNTIME_LIBRARY "${MONGOCXX_LIBDIR}/libmongocxx._noabi.dylib")
    set(BSONCXX_RUNTIME_LIBRARY "${BSONCXX_LIBDIR}/libbsoncxx._noabi.dylib")
    find_file(MONGOC2_RUNTIME_LIBRARY NAMES libmongoc2.2.dylib
        PATHS "/opt/homebrew/opt/mongo-c-driver/lib" "/usr/local/opt/mongo-c-driver/lib")
    find_file(BSON2_RUNTIME_LIBRARY NAMES libbson2.2.dylib
        PATHS "/opt/homebrew/opt/mongo-c-driver/lib" "/usr/local/opt/mongo-c-driver/lib")
    find_file(ZSTD_RUNTIME_LIBRARY NAMES libzstd.1.dylib
        PATHS "/opt/homebrew/opt/zstd/lib" "/usr/local/opt/zstd/lib")

    foreach(required_runtime
            MACDEPLOYQT_EXECUTABLE
            MONGOCXX_RUNTIME_LIBRARY
            BSONCXX_RUNTIME_LIBRARY
            MONGOC2_RUNTIME_LIBRARY
            BSON2_RUNTIME_LIBRARY
            ZSTD_RUNTIME_LIBRARY)
        if(NOT EXISTS "${${required_runtime}}")
            message(FATAL_ERROR "Required macOS runtime file is missing: ${required_runtime}")
        endif()
    endforeach()

    configure_file(
        "${CMAKE_SOURCE_DIR}/cmake/RobomongoMacBundleRuntime.cmake.in"
        "${CMAKE_BINARY_DIR}/RobomongoMacBundleRuntime.cmake"
        @ONLY)
    install(SCRIPT "${CMAKE_BINARY_DIR}/RobomongoMacBundleRuntime.cmake")
elseif(SYSTEM_WINDOWS)
    install_qt_plugins(
        QWindowsIntegrationPlugin
        QMinimalIntegrationPlugin
        QOffscreenIntegrationPlugin)

    # Install Styles
    install(FILES "${QT_STYLES_DIR}/qwindowsvistastyle.dll" DESTINATION ${styles_dir})

    # Install runtime libraries:
    # msvcp120.dll
    # msvcr120.dll
    set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION .)
    include(InstallRequiredSystemLibraries)
endif()
