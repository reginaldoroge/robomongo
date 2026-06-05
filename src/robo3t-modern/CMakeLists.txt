add_executable(robo3t_driver_probe
    mongo/ModernMongoDriver.cpp
    mongo_driver_probe.cpp)

target_link_libraries(robo3t_driver_probe
    PRIVATE
        Qt5::Core
        MongoCxx::Driver)

target_include_directories(robo3t_driver_probe
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(robo3t_arm_preview MACOSX_BUNDLE
    legacy/LegacyMongoDriver.cpp
    mongo/ModernMongoDriver.cpp
    preview/ArmPreviewWindow.h
    preview/ArmPreviewWindow.cpp
    arm_preview_main.cpp)

target_link_libraries(robo3t_arm_preview
    PRIVATE
        Qt5::Widgets
        MongoCxx::Driver
        PkgConfig::MONGOC_LEGACY
        PkgConfig::BSON_LEGACY)

target_include_directories(robo3t_arm_preview
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(robo3t_arm_preview PROPERTIES
    OUTPUT_NAME "Robo 3T ARM Preview"
    MACOSX_BUNDLE_BUNDLE_NAME "Robo 3T ARM Preview"
    MACOSX_BUNDLE_GUI_IDENTIFIER "local.robo3t.arm.preview"
    BUILD_RPATH "${MONGOC_LEGACY_LIBRARY_DIRS};${BSON_LEGACY_LIBRARY_DIRS}")

if(MONGOC_LEGACY_FOUND AND BSON_LEGACY_FOUND)
    add_executable(robo3t_legacy_driver_probe
        legacy/LegacyMongoDriver.cpp
        legacy_driver_probe.cpp)

    target_link_libraries(robo3t_legacy_driver_probe
        PRIVATE
            Qt5::Core
            PkgConfig::MONGOC_LEGACY
            PkgConfig::BSON_LEGACY)

    target_include_directories(robo3t_legacy_driver_probe
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR})

    set_target_properties(robo3t_legacy_driver_probe PROPERTIES
        BUILD_RPATH "${MONGOC_LEGACY_LIBRARY_DIRS};${BSON_LEGACY_LIBRARY_DIRS}")
endif()
