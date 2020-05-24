#pragma once

#include <stdint.h>
#include <string>
#include <vector>

namespace Arma3
{
    std::vector<std::string> anticheatterms{"infiSTAR", "cheat", "infistar", "hacker", "spanker", "faggot", "menucheat", "createTask = 'STR';", "(findDisplay 46)closeDisplay 0", "BAN", "KICK", "(findDisplay 0)closeDisplay 0", "illegal", "_worldspace = player getVariable['AHworldspace',[]];", "PVAHR_0", "_puid = _this select 0;_name = _this select 1;", "_btnSave ctrlSetText 'PlayerUID:';", "SPY-GLASS", "he/she was trying to access commanding menu", "SPYGLASS-FLAG", "SpyGlass", "SPY_fnc_notifyAdmins", "SPY_fnc_cookieJar", "MenuBasedHack", "Cheater Flagged", "STAR", "infi"};

    const struct Offsets
    {
        static const uint64_t World = 0x258C9D0;
        static const uint64_t CameraOn = 0x26F0;
        static const uint64_t EventHandlerTable = 0x608;
        static const uint64_t EventHandlerSize = 0x610;
        static const uint64_t EventHandlerDist = 0x18;

        static const uint64_t scriptVM = 0x1450;
        static const uint64_t scriptVMSize = 0x1458;
        static const uint64_t scriptVMString = 0x488;

        static const uint64_t NetworkManager = 0x254A660;
    } Offsets;

    /*
    * See the Unknown Cheats thread for more information about this.
    * https://www.unknowncheats.me/forum/arma-3-a/327117-enable-filepatching.html
    */
    const struct FilePatching
    {
        static const uint64_t BLOCKBEGIN = 0x25BF510;
        static const uint64_t CONSTANT1 = 0x00000001DD5D645B;
        static const uint64_t CONSTANT2 = 0xBF204ABFDD5D645C;
        static const uint64_t OFFSET1 = 0xFFFFFFFFFD420FC0;
        static const uint64_t OFFSET2 = 0x137650;
        static const uint64_t OFFSET3 = 0xFFFFFFFFFFFFFFA0;
        static const uint64_t OFFSET4 = 0x1DED90;
    } FilePatching;

    enum EventHandlerEnum
    {
        // Other event handler ID's?
        InventoryOpened = 10
    };
} // namespace Arma3