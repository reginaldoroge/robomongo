#include "robomongo/core/mongodb/modern/ModernConnectionUri.h"

#include <cctype>
#include <iomanip>
#include <sstream>

#include "robomongo/core/settings/ConnectionSettings.h"
#include "robomongo/core/settings/CredentialSettings.h"

namespace
{
    std::string urlEncode(const std::string &value)
    {
        std::ostringstream encoded;
        encoded << std::uppercase << std::hex;

        for (unsigned char ch : value) {
            if (std::isalnum(ch) || ch == '-' || ch == '_' || ch == '.' || ch == '~') {
                encoded << ch;
                continue;
            }

            encoded << '%' << std::setw(2) << std::setfill('0') << static_cast<int>(ch);
        }

        return encoded.str();
    }
}

namespace Robomongo
{
    std::string modernDirectUri(const ConnectionSettings &settings)
    {
        std::ostringstream uri;
        uri << "mongodb://";

        if (settings.hasEnabledPrimaryCredential()) {
            const auto *credential = settings.primaryCredential();
            uri << urlEncode(credential->userName()) << ':'
                << urlEncode(credential->userPassword()) << '@';
        }

        uri << settings.serverHost() << ':' << settings.serverPort();

        if (settings.hasEnabledPrimaryCredential()) {
            const auto *credential = settings.primaryCredential();
            uri << "/?authSource=" << urlEncode(credential->databaseName());
        }

        return uri.str();
    }
}
