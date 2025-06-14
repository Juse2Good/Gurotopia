#include "pch.hpp"
#include "network/packet.hpp"
#include "respawn.hpp"

void respawn(ENetEvent event, const std::string& header) 
{
    gt_packet(*event.peer, true, 0, { 
        "OnSetFreezeState", 
        2 
    });
    gt_packet(*event.peer, true, 0,{ "OnKilled" });
    // @note wait 1900 milliseconds...
    auto &peer = _peer[event.peer];
    gt_packet(*event.peer, true, 1900, {
        "OnSetPos", 
        std::vector<float>{peer->rest_pos.front(), peer->rest_pos.back()}
    });
    gt_packet(*event.peer, true, 1900, { "OnSetFreezeState" });
}