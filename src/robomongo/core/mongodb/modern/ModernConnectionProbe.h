#pragma once

#include <string>

namespace Robomongo
{
    class ConnectionSettings;

    struct ModernDriverInfo
    {
        bool available = false;
        bool modernServer = false;
        int maxWireVersion = 0;
        std::string serverVersion;
        std::string driverName = "Legacy driver";
        std::string driverVersion = "4.2";
        std::string error;
    };

    class ModernConnectionProbe
    {
    public:
        ModernDriverInfo probe(const ConnectionSettings &settings) const;
    };
}
