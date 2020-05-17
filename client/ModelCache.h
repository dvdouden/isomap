#pragma once

#include <vlCore/ResourceDatabase.hpp>
#include <vlCore/Object.hpp>
#include <map>

namespace isomap {
    namespace client {
        class ModelCache {
        public:
            static vl::ResourceDatabase* get( const std::string& model );

            static void clear();

        private:
            static std::map<std::string, vl::ref<vl::ResourceDatabase>> s_cache;
        };

    }
}

