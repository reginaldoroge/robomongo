#pragma once

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view.hpp>

#include <memory>
#include <string>

namespace Robo3T::Modern {

class ModernMongoDriver {
public:
    ModernMongoDriver();
    ~ModernMongoDriver();

    bsoncxx::document::value makeProbeDocument(const std::string& uriText) const;
    bsoncxx::document::value ping(const std::string& uriText) const;

private:
    class Impl;

    std::unique_ptr<Impl> _impl;
};

std::string toJson(bsoncxx::document::view document);

} // namespace Robo3T::Modern
