find_package(PkgConfig REQUIRED)

pkg_check_modules(MONGOCXX REQUIRED IMPORTED_TARGET libmongocxx)
pkg_check_modules(BSONCXX REQUIRED IMPORTED_TARGET libbsoncxx)
pkg_check_modules(MONGOC_LEGACY IMPORTED_TARGET libmongoc-1.0)
pkg_check_modules(BSON_LEGACY IMPORTED_TARGET libbson-1.0)

add_library(MongoCxx::Driver INTERFACE IMPORTED)
target_link_libraries(MongoCxx::Driver
    INTERFACE
        PkgConfig::MONGOCXX
        PkgConfig::BSONCXX)

set(MongoCxx_FOUND TRUE)
