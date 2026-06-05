#include "robomongo/core/mongodb/modern/ModernConnectionProbe.h"

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/uri.hpp>

#include "robomongo/core/mongodb/modern/ModernConnectionUri.h"
#include "robomongo/core/mongodb/modern/ModernDriverRuntime.h"
#include "robomongo/core/settings/ConnectionSettings.h"
#include "robomongo/core/settings/SshSettings.h"
#include "robomongo/core/settings/SslSettings.h"

#ifndef MODERN_MONGOCXX_VERSION
#define MODERN_MONGOCXX_VERSION "unknown"
#endif

namespace
{
    int intField(const bsoncxx::document::view &doc, const char *name)
    {
        auto element = doc[name];
        if (!element)
            return 0;

        if (element.type() == bsoncxx::type::k_int32)
            return element.get_int32().value;

        if (element.type() == bsoncxx::type::k_int64)
            return static_cast<int>(element.get_int64().value);

        return 0;
    }

    std::string stringField(const bsoncxx::document::view &doc, const char *name)
    {
        auto element = doc[name];
        if (!element || element.type() != bsoncxx::type::k_string)
            return {};

        return std::string(element.get_string().value);
    }
}

namespace Robomongo
{
    ModernDriverInfo ModernConnectionProbe::probe(const ConnectionSettings &settings) const
    {
        ModernDriverInfo info;

        if (settings.isReplicaSet() || settings.sshSettings()->enabled() ||
            settings.sslSettings()->sslEnabled()) {
            return info;
        }

        try {
            modernDriverRuntime();

            auto client = mongocxx::client(mongocxx::uri(modernDirectUri(settings)));

            using bsoncxx::builder::basic::kvp;
            using bsoncxx::builder::basic::make_document;
            auto result = client["admin"].run_command(make_document(kvp("hello", 1)));
            auto view = result.view();

            info.available = true;
            info.maxWireVersion = intField(view, "maxWireVersion");
            info.serverVersion = stringField(view, "version");
            info.modernServer = info.maxWireVersion >= 17;

            if (info.modernServer) {
                info.driverName = "Modern driver";
                info.driverVersion = MODERN_MONGOCXX_VERSION;
            }
        } catch (const std::exception &ex) {
            info.error = ex.what();
        }

        return info;
    }
}
