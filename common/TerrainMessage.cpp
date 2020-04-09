#include "TerrainMessage.h"

namespace isomap {
    namespace common {

        TerrainMessage* TerrainMessage::createMsg(uint32_t width, uint32_t height) {
            auto* msg = new TerrainMessage( Create );
            msg->m_width = width;
            msg->m_height = height;
            return msg;
        }

        TerrainMessage* TerrainMessage::updateMsg(std::vector<Cell> &cells) {
            auto* msg = new TerrainMessage( Update );
            // probably should be using the move operator here
            std::swap( msg->m_cells, cells );
            return msg;
        }
    }
}