#include "robomongo/core/mongodb/modern/ModernDriverRuntime.h"

#include <mongocxx/instance.hpp>

namespace Robomongo
{
    mongocxx::instance &modernDriverRuntime()
    {
        static mongocxx::instance runtime {};
        return runtime;
    }
}
