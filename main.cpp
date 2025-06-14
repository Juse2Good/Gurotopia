/*
    @copyright gurotopia (c) 25-6-2024
    @version beta-327

    looking for:
    - Indonesian translator
    - reverse engineer
*/
#include "include/pch.hpp"
#include "include/network/compress.hpp" // @note isalzman's compressor
#include "include/event_type/__event_type.hpp"

#include <filesystem>

int main()
{
    printf("\e[38;5;248m nlohmann/JSON \e[1;37m%d.%d.%d \e[0m\n", NLOHMANN_JSON_VERSION_MAJOR, NLOHMANN_JSON_VERSION_MINOR, NLOHMANN_JSON_VERSION_PATCH);
    printf("\e[38;5;248m microsoft/mimalloc \e[1;37mbeta-%d \e[0m\n", MI_MALLOC_VERSION);
    printf("\e[38;5;248m zpl-c/enet \e[1;37m%d.%d.%d \e[0m\n", ENET_VERSION_MAJOR, ENET_VERSION_MINOR, ENET_VERSION_PATCH);

    if (!std::filesystem::exists("worlds")) std::filesystem::create_directory("worlds");
    if (!std::filesystem::exists("players")) std::filesystem::create_directory("players");
    {
        ENetCallbacks callbacks{
            .malloc = &mi_malloc,
            .free = &mi_free,
            .no_memory = []() { printf("\e[1;31mENet memory overflow\e[0m\n"); }
        };
        enet_initialize_with_callbacks(ENET_VERSION, &callbacks);
    } // @note delete callbacks
    server = enet_host_create({
        .host = in6addr_any,
        .port = 17091u
    },
    50zu/*@note less to allocate than setting to MAX*/, 2zu);
    
    server->checksum = enet_crc32;
    enet_host_compress_with_range_coder(server);
    {
        const uintmax_t size = std::filesystem::file_size("items.dat");

        im_data.resize(im_data.size() + size); // @note state + items.dat
        im_data[0zu] = std::byte{ 04 }; // @note 04 00 00 00
        im_data[4zu] = std::byte{ 0x10 }; // @note 16 00 00 00
        im_data[16zu] = std::byte{ 0x08 }; // @note 08 00 00 00
        *reinterpret_cast<std::uintmax_t*>(&im_data[56zu]) = size;

        std::ifstream("items.dat", std::ios::binary)
            .read(reinterpret_cast<char*>(&im_data[60zu]), size);
    } // @note delete size and close file
    cache_items();

    ENetEvent event{};
    while (true)
        while (enet_host_service(server, &event, 50u/*ms*/) > 0)
            if (const auto i = event_pool.find(event.type); i != event_pool.end())
                i->second(event);
    return 0;
}
