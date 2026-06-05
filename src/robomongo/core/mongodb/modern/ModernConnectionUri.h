#pragma once

#include <string>

namespace Robomongo
{
    class ConnectionSettings;

    std::string modernDirectUri(const ConnectionSettings &settings);
}
