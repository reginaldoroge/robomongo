#pragma once

#include <string>

namespace Robo3T::Modern {

class LegacyMongoDriver {
public:
    LegacyMongoDriver();
    ~LegacyMongoDriver();

    std::string ping(const std::string& uriText) const;
};

} // namespace Robo3T::Modern
