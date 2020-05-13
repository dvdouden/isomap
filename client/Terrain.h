#pragma once

#include <vlGraphics/RenderingAbstract.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
#include "../common/FootPrint.h"
#include "../common/TerrainData.h"
#include "../common/types.h"

namespace isomap {
    namespace client {
        class Terrain {
        public:
            struct Area {
                uint32_t x = 0;
                uint32_t y = 0;
                uint32_t w = 0;
                uint32_t h = 0;
                const common::FootPrint* footPrint = nullptr;

                Area( uint32_t x, uint32_t y, uint32_t w, uint32_t h ) :
                        x( x ),
                        y( y ),
                        w( w ),
                        h( h ),
                        footPrint( nullptr ) { }

                Area( uint32_t x, uint32_t y, const common::FootPrint* footPrint ) :
                        x( x ),
                        y( y ),
                        w( footPrint->width() ),
                        h( footPrint->height() ),
                        footPrint( footPrint ) { }

                Area() = default;

                Area( const Area& ) = default;

                ~Area() = default;

                Area& operator=( const Area& ) = default;

                bool contains( uint32_t x, uint32_t y ) const {
                    if ( x >= this->x && x < this->x + w && y >= this->y && y < this->y + h ) {
                        if ( footPrint != nullptr ) {
                            return footPrint->get( x - this->x, y - this->y ) != 0;
                        }
                        return true;
                    }
                    return false;
                }
            };

            Terrain( uint32_t width, uint32_t height );

            ~Terrain() = default;

            Terrain( const Terrain& ) = delete;

            const Terrain& operator=( const Terrain& ) = delete;

            void processMessage( common::TerrainMessage* msg );

            uint32_t width() const {
                return m_width;
            }

            uint32_t height() const {
                return m_height;
            }

            uint8_t* heightMap() const {
                return m_data.heightMap;
            }

            uint8_t* slopeMap() const {
                return m_data.slopeMap;
            }

            uint8_t* oreMap() const {
                return m_data.oreMap;
            }

            uint8_t* occupancyMap() const {
                return m_data.occupancyMap;
            }

            uint8_t occupancy( uint32_t x, uint32_t y ) const {
                return m_data.occupancyMap[y * m_data.mapWidth + x];
            }

            uint8_t* pathMap() const {
                return m_data.pathMap;
            }

            void initRender( vl::RenderingAbstract* rendering );

            void disableRendering();

            void enableRendering();

            void render();

            void updateFog();

            void toggleRenderFog() {
                m_renderFog = !m_renderFog;
            }

            void toggleRenderOccupancy() {
                m_renderOccupancy = !m_renderOccupancy;
            }

            bool isVisible( uint32_t x, uint32_t y ) const {
                return m_fogMap[y * m_width + x] > 1;
            }

            uint8_t occupied( uint32_t x, uint32_t y ) const {
                return m_data.occupancyMap[y * m_width + x];
            }

            void occupy( uint32_t worldX, uint32_t worldY, const common::FootPrint* footPrint );

            void vacate( uint32_t worldX, uint32_t worldY, const common::FootPrint* footPrint );

            void reserve( uint32_t worldX, uint32_t worldY, const common::FootPrint* footPrint );

            void unreserve( uint32_t worldX, uint32_t worldY, const common::FootPrint* footPrint );

            void highLight( const Area& area, const vl::fvec4& color ) {
                m_highlightArea = area;
                m_highlightColor = color;
                m_renderHighlight = true;
            }

            void addHighlight( const Area& area, const vl::fvec4& color ) {
                m_highLightAreas.emplace_back( area, color );
            }

            void clearHighlight() {
                m_renderHighlight = false;
                m_highLightAreas.clear();
            }

            uint8_t getCorner( uint32_t x, uint32_t y, uint32_t c ) const {
                return m_data.heightMap[y * m_width + x] -
                       (uint8_t( m_data.slopeMap[y * m_width + x] >> uint32_t( c ) ) & 0b0000'0001u);
            }

            void addStructure( Structure* structure );

            void removeStructure( Structure* structure );

            Structure* getStructureAt( uint32_t x, uint32_t y );

            void addUnit( Unit* unit );

            void removeUnit( Unit* unit );

            void updateUnit( Unit* unit, uint32_t oldX, uint32_t oldY );

            Unit* getUnitAt( uint32_t x, uint32_t y );

        private:
            uint8_t getCornerSafe( int x, int y, int c ) const;

            std::vector<uint32_t> getChunks( Structure* structure );

            uint32_t getChunk( uint32_t x, uint32_t y );

            void removeUnitFromChunk( Unit* unit, uint32_t chunk );

            uint32_t m_width = 0;
            uint32_t m_height = 0;
            uint32_t m_fogUpdateMapScale = 64;
            uint32_t m_fogUpdateMapWidth = 0;
            uint32_t m_fogUpdateMapHeight = 0;

            common::TerrainData m_data;

            std::vector<uint8_t> m_fogMap;
            std::vector<uint8_t> m_fogUpdateMap;

            // FIXME: shared code with server!
            uint32_t m_chunkSize = 16;
            std::vector<std::vector<Structure*>> m_structures;
            std::vector<std::vector<Unit*>> m_units;

            bool m_renderFog = false;
            bool m_renderHighlight = false;
            bool m_renderOccupancy = false;

            Area m_highlightArea;
            vl::fvec4 m_highlightColor;

            std::vector<std::pair<Area, vl::fvec4>> m_highLightAreas;

            // TODO: Separate render code from game logic
            // We don't need the AI data structures to be renderable
            vl::ref<vl::SceneManagerActorTree> m_sceneManager;
        };
    }
}




