#include "legacy/LegacyMongoDriver.h"

#include <bson/bson.h>
#include <mongoc/mongoc.h>

#include <stdexcept>

namespace Robo3T::Modern {

LegacyMongoDriver::LegacyMongoDriver()
{
    mongoc_init();
}

LegacyMongoDriver::~LegacyMongoDriver()
{
    mongoc_cleanup();
}

std::string LegacyMongoDriver::ping(const std::string& uriText) const
{
    auto const uri = uriText.empty() ? "mongodb://localhost:27017" : uriText.c_str();
    mongoc_client_t* client = mongoc_client_new(uri);
    if (!client) {
        throw std::runtime_error("Failed to create legacy MongoDB client.");
    }

    bson_error_t error;
    bson_t command;
    bson_t reply;
    bson_init(&command);
    BSON_APPEND_INT32(&command, "ping", 1);

    bool const ok = mongoc_client_command_simple(client, "admin", &command, nullptr, &reply, &error);

    bson_destroy(&command);
    mongoc_client_destroy(client);

    if (!ok) {
        bson_destroy(&reply);
        throw std::runtime_error(error.message);
    }

    char* json = bson_as_canonical_extended_json(&reply, nullptr);
    std::string result = json ? json : "{}";
    bson_free(json);
    bson_destroy(&reply);
    return result;
}

} // namespace Robo3T::Modern
