#pragma once

#include <vlGraphics/RenderingAbstract.hpp>
#include <vlGraphics/SceneManagerActorTree.hpp>
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

                Area( uint32_t x, uint32_t y, uint32_t w, uint32_t h ) :
                        x( x ),
                        y( y ),
                        w( w ),
                        h( h ) { }

                Area() = default;

                Area( const Area& ) = default;

                ~Area() = default;

                Area& operator=( const Area& ) = default;

                bool contains( uint32_t x, uint32_t y ) const {
                    return x >= this->x && x < this->x + w && y >= this->y && y < this->y + h;
                }
            };

            Terrain() = default;

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
                return m_heightMap;
            }

            uint8_t* slopeMap() const {
                return m_slopeMap;
            }

            uint8_t* oreMap() const {
                return m_oreMap;
            }

            void initRender( vl::RenderingAbstract* rendering );

            void render();

            void updateFog();

            void toggleRenderFog() {
                m_renderFog = !m_renderFog;
            }

            bool isVisible( uint32_t x, uint32_t y ) const {
                return m_fogMap[y * m_width + x] > 1;
            }

            uint8_t occupied( uint32_t x, uint32_t y ) const {
                return m_occupancyMap[y * m_width + x];
            }

            void occupy( uint32_t x, uint32_t y, uint32_t width, uint32_t height ) {
                // occupy area and clear reservation
                for ( uint32_t tileY = y; tileY < y + height; ++tileY ) {
                    for ( uint32_t tileX = x; tileX < x + width; ++tileX ) {
                        m_occupancyMap[tileY * m_width + tileX] =
                                (m_occupancyMap[tileY * m_width + tileX] & ~0b0000'0011u) | 0b0000'0001u;
                    }
                }
            }

            void reserve( uint32_t x, uint32_t y, uint32_t width, uint32_t height ) {
                for ( uint32_t tileY = y; tileY < y + height; ++tileY ) {
                    for ( uint32_t tileX = x; tileX < x + width; ++tileX ) {
                        m_occupancyMap[tileY * m_width + tileX] |= 0b0000'0010u;
                    }
                }
            }

            void unreserve( uint32_t x, uint32_t y, uint32_t width, uint32_t height ) {
                for ( uint32_t tileY = y; tileY < y + height; ++tileY ) {
                    for ( uint32_t tileX = x; tileX < x + width; ++tileX ) {
                        m_occupancyMap[tileY * m_width + tileX] &= ~0b0000'0010u;
                    }
                }
            }

            void highLight( const Area& area, const vl::fvec4& color ) {
                m_highlightArea = area;
                m_highlightColor = color;
                m_renderHighlight = true;
            }

            void addHighlight( const Area& area, const vl::fvec4& color ) {
                m_highLightAreas.push_back( std::pair<Area, vl::fvec4>( area, color ) );
            }

            void clearHighlight() {
                m_highLightAreas.clear();
            }

            uint8_t getCorner( uint32_t x, uint32_t y, uint32_t c ) const {
                return m_heightMap[y * m_width + x] -
                       (uint8_t( m_slopeMap[y * m_width + x] >> uint32_t( c ) ) & 0b0000'0001u);
            }

        private:
            uint8_t getCornerSafe( int x, int y, int c ) const;

            uint32_t m_width = 0;
            uint32_t m_height = 0;
            uint32_t m_fogUpdateMapScale = 64;
            uint32_t m_fogUpdateMapWidth = 0;
            uint32_t m_fogUpdateMapHeight = 0;

            uint8_t* m_heightMap = nullptr;
            uint8_t* m_slopeMap = nullptr;
            uint8_t* m_oreMap = nullptr;
            uint8_t* m_fogMap = nullptr;
            uint8_t* m_fogUpdateMap = nullptr;
            uint8_t* m_occupancyMap = nullptr;

            bool m_renderFog = true;
            bool m_renderHighlight = false;

            Area m_highlightArea;
            vl::fvec4 m_highlightColor;

            std::vector<std::pair<Area, vl::fvec4>> m_highLightAreas;

            // TODO: Separate render code from game logic
            // We don't need the AI data structures to be renderable
            vl::ref<vl::SceneManagerActorTree> m_sceneManager;
        };
    }
}




