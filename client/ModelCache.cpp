#include <vlGraphics/Actor.hpp>
#include <vlGraphics/Geometry.hpp>
#include "ModelCache.h"

namespace isomap {
    namespace client {

        std::map<std::string, vl::ref<vl::ResourceDatabase>> ModelCache::s_cache;

        vl::ResourceDatabase* isomap::client::ModelCache::get( const std::string& model ) {
            auto db = s_cache.find( model );
            if ( db == s_cache.end() ) {
                // load it
                std::string path = "models/";
                path += model;
                path += ".obj";
                vl::ref<vl::ResourceDatabase> resource_db = vl::loadResource( path.c_str(), false );
                if ( !resource_db || resource_db->count<vl::Actor>() == 0 ) {
                    VL_LOG_ERROR << "No data found.\n";
                    return nullptr;
                }
                for ( auto& ires : resource_db->resources() ) {
                    auto* act = ires->as<vl::Actor>();
                    if ( !act )
                        continue;
                    auto* geom = act->lod( 0 )->as<vl::Geometry>();
                    geom->computeNormals();
                }

                s_cache[model] = resource_db;
                return resource_db.get();
            } else {
                return db->second.get();
            }
        }

        void ModelCache::clear() {
            s_cache.clear();
        }

    }
}

