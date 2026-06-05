#include "mongo/ModernMongoDriver.h"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/json.hpp>

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>

namespace Robo3T::Modern {

class ModernMongoDriver::Impl {
public:
    mongocxx::uri parseUri(const std::string& uriText) const
    {
        return mongocxx::uri{uriText.empty() ? mongocxx::uri::k_default_uri : uriText};
    }

private:
    mongocxx::instance _driverInstance;
};

ModernMongoDriver::ModernMongoDriver()
    : _impl(std::make_unique<Impl>())
{
}

ModernMongoDriver::~ModernMongoDriver() = default;

bsoncxx::document::value ModernMongoDriver::makeProbeDocument(const std::string& uriText) const
{
    auto uri = _impl->parseUri(uriText);

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;
    return make_document(
        kvp("driver", "mongocxx"),
        kvp("arch", "arm64"),
        kvp("uri", uri.to_string()));
}

bsoncxx::document::value ModernMongoDriver::ping(const std::string& uriText) const
{
    auto client = mongocxx::client{_impl->parseUri(uriText)};

    using bsoncxx::builder::basic::kvp;
    using bsoncxx::builder::basic::make_document;
    return client["admin"].run_command(make_document(kvp("ping", 1)));
}

std::string toJson(bsoncxx::document::view document)
{
    return bsoncxx::to_json(document);
}

} // namespace Robo3T::Modern
