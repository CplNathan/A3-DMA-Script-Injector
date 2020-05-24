#include "vmread/hlapi/hlapi.h"
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <vector>
 
enum EventHandlerEnum
{
    // Other event handler ID's?
    InventoryOpened = 10
};
 
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
 
std::vector<std::string> anticheatterms{"infiSTAR", "cheat", "infistar", "hacker", "spanker", "faggot", "menucheat", "createTask = 'STR';", "(findDisplay 46)closeDisplay 0", "BAN", "KICK", "(findDisplay 0)closeDisplay 0", "illegal", "_worldspace = player getVariable['AHworldspace',[]];", "PVAHR_0", "_puid = _this select 0;_name = _this select 1;", "_btnSave ctrlSetText 'PlayerUID:';", "SPY-GLASS", "he/she was trying to access commanding menu", "SPYGLASS-FLAG", "SpyGlass", "SPY_fnc_notifyAdmins", "SPY_fnc_cookieJar", "MenuBasedHack", "Cheater Flagged", "STAR", "infi"};
uint64_t restoreTime = 30;
 
// ----- Beep Boop ----- //
uint64_t g_armaBase;
uint64_t g_battleyeBase;
 
std::string g_scriptData;
 
uint64_t g_NetworkManager;
uint64_t g_NetworkClient;
uint64_t g_NetworkCallback;
 
struct ArmaString
{
    WinProcess *armaProc;
 
    uint64_t l_aString;
    int l_aStringLen;
    uint64_t l_aStringPtr;
 
    ArmaString(){};
 
    ArmaString(WinProcess *armaProc, uint64_t ArmaString)
    {
        this->armaProc = armaProc;
        this->l_aString = armaProc->Read<uint64_t>(ArmaString + 0x10);
        this->l_aStringLen = armaProc->Read<int>(l_aString + 0x8);
        this->l_aStringPtr = l_aString + 0x10;
    }
 
    void WriteString(std::string ArmaStringValue)
    {
        ArmaStringValue.append(this->l_aStringLen - ArmaStringValue.size(), ' ');
 
        armaProc->Write(this->l_aStringPtr, ArmaStringValue.data(), l_aStringLen);
    }
 
    std::string ReadString()
    {
        std::string result;
        result.resize(this->l_aStringLen);
 
        armaProc->Read(this->l_aStringPtr, result.data(), this->l_aStringLen);
 
        return result;
    }
};
 
struct ArmaStringRestore
{
    WinProcess *armaProc;
 
    ArmaString RefArmaString;
 
    std::string SavedString;
 
    bool Valid;
 
    ArmaStringRestore() { this->Valid = false; };
    ~ArmaStringRestore()
    {
        Restore();
    }
 
    ArmaStringRestore(WinProcess *armaProc, ArmaString InArmaString)
    {
        this->armaProc = armaProc;
 
        this->RefArmaString = InArmaString;
        this->SavedString = RefArmaString.ReadString();
 
        this->Valid = true;
    }
 
    void Restore()
    {
        if (!Valid)
            return;
 
        RefArmaString.WriteString(SavedString);
 
        this->Valid = false;
    }
};
 
static bool InjectScript(WinProcess *armaProc, ArmaStringRestore& Restore)
{
    bool Success;

    uint64_t g_World = armaProc->Read<uint64_t>(g_armaBase + Offsets.World);
 
    uint64_t g_CameraOn = armaProc->Read<uint64_t>(g_World + Offsets.CameraOn);
    g_CameraOn = armaProc->Read<uint64_t>(g_CameraOn + 0x8);
 
    uint64_t g_EventHandlerTable = armaProc->Read<uint64_t>(g_CameraOn + Offsets.EventHandlerTable);
    int g_EventHandlerSize = armaProc->Read<int>(g_CameraOn + Offsets.EventHandlerSize);
 
    for (int i = 0; i < g_EventHandlerSize; i++)
    {
        uint64_t g_CurrentEventHandlerIndex = g_EventHandlerTable + (i * 0x28);
 
        uint64_t g_EventHandlerContainer = armaProc->Read<uint64_t>(g_CurrentEventHandlerIndex + 0x10);
        uint64_t g_EventHandlerTrigger = g_CurrentEventHandlerIndex + 0x8;
 
        ArmaString l_ArmaString = ArmaString(armaProc, g_EventHandlerContainer);
 
        int scriptSize = g_scriptData.size();
 
        if (l_ArmaString.l_aStringLen > scriptSize && l_ArmaString.l_aStringLen > 0)
        {
            Restore = ArmaStringRestore(armaProc, l_ArmaString);
 
            armaProc->Write((g_EventHandlerTrigger), InventoryOpened);
            l_ArmaString.WriteString(g_scriptData);
 
            printf("Event Handler Set\n");
            Success = true;
 
            break;
        }
        else
        {
            printf("Script too large for event handler container: %d/%d at index %d\n", scriptSize, l_ArmaString.l_aStringLen, i);
        }
    }
 
    return Success;
}
 
static void AnticheatKiller(WinProcess *armaProc)
{
    uint64_t g_World = armaProc->Read<uint64_t>(g_armaBase + Offsets.World);
 
    uint64_t g_scriptVM = armaProc->Read<uint64_t>(g_World + Offsets.scriptVM);
    int g_scriptVMSize = armaProc->Read<int>(g_World + Offsets.scriptVMSize);
 
    for (int i = 0; i < g_scriptVMSize; i++)
    {
        uint64_t l_script = armaProc->Read<uint64_t>(g_scriptVM + (i * 0x10));
 
        ArmaString l_ArmaString = ArmaString(armaProc, l_script + Offsets.scriptVMString);
 
        std::string result = l_ArmaString.ReadString();
        for (int i = 0; i < (int)anticheatterms.size(); i++)
        {
            if (result.find(anticheatterms[i]) != std::string::npos)
            {
                armaProc->Write(l_script + 0x508, true);
                printf("Disabling anticheat thread at index: %d", i);
                break;
            }
        }
    }
}
 
static void init(bool i, bool r)
{
    FILE *out = stdout;
    pid_t pid;
 
    FILE *pipe = popen("pidof qemu-system-x86_64", "r");
    fscanf(pipe, "%d", &pid);
    pclose(pipe);
 
    try
    {
        WinContext ctx = WinContext(pid);
        ctx.processList.Refresh();
 
        WinProcess *ArmaProc = ctx.processList.FindProcNoCase("arma3_x64.exe");
        WinProcess *BattleyeProc = ctx.processList.FindProcNoCase("arma3battleye.exe");
 
        g_armaBase = ArmaProc->modules.GetModuleInfo("Arma3_x64.exe")->info.baseAddress;
        g_battleyeBase = BattleyeProc->modules.GetModuleInfo("Arma3BattlEye.exe")->info.baseAddress;
 
        if (g_armaBase && g_battleyeBase)
        {
            ArmaStringRestore Restore;
            bool Injected = InjectScript(ArmaProc, Restore);
 
            if (!Injected) // Failed to inject
                return;
 
            uint64_t timeStart = clock();
            int lastRestoreUI = restoreTime;
            while (1)
            {
                if (i)
                    AnticheatKiller(ArmaProc);
 
                if (Restore.Valid && r)
                {
                    if ((clock() - timeStart) / CLOCKS_PER_SEC >= restoreTime)
                    {
                        Restore.Restore();
                        printf("Restored EH\n");
                    }
                    else
                    {
                        int thisRestoreUI = std::abs((int)((clock() - timeStart) / CLOCKS_PER_SEC - restoreTime));
                        if (thisRestoreUI != lastRestoreUI)
                        {
                            lastRestoreUI = thisRestoreUI;
                            printf("Restoring EH in: %d\n", thisRestoreUI);
                        }
                    }
                }
 
                if ((!Restore.Valid || !r) && !i)
                {
                    printf("Nothing left to do, exiting.\n");
                    break;
                }
            }
        }
        else
        {
            printf("Arma Found: %s\tBattlEye Found: %s\nExiting\n", g_armaBase ? "Yes" : "No", g_battleyeBase ? "Yes" : "No");
        }
    }
    catch (VMException &e)
    {
        fprintf(out, "Initialization error: %d\n", e.value);
    }
 
    fclose(out);
}
 
int main(int argc, char *argv[])
{
    std::string scriptdir;
 
    bool AnticheatKiller = false;
    bool RestoreHook = false;
 
    bool success = false;
 
    if (argc == 4)
    {
        scriptdir = argv[1];
 
        AnticheatKiller = (std::tolower(argv[2][0]) == 'y');
        RestoreHook = (std::tolower(argv[3][0]) == 'y');
 
        FILE *scriptfile = fopen(scriptdir.c_str(), "r");
 
        printf("Loading with settings - Script Location: %s (%s)\tKill Infistar: %s\n", argv[1], scriptfile ? "Found" : "Not Found", AnticheatKiller ? "Yes" : "No");
 
        if (scriptfile)
        {
            fseek(scriptfile, 0, SEEK_END);
            long fsize = ftell(scriptfile);
            fseek(scriptfile, 0, SEEK_SET);
 
            g_scriptData.resize(fsize + 1);
            fread(g_scriptData.data(), 1, fsize, scriptfile);
 
            success = true;
        }
 
        fclose(scriptfile);
    }
    else
    {
        printf("%s \"~/Documents/scripttorun.txt\" (kill infistar? y/else) (restore hook? y/else)\n\n", argv[0]);
        printf("Kill Infistar/AC:\tKills script threads that have specific keywords relating to anti-cheat. (can break gamemode specific code)\n");
        printf("Restore Hook:\tRestores the original event handler that was hooked after some time. (sometimes we can hook important game-play EH's and break gameplay)\n");
    }
 
    if (success)
        init(AnticheatKiller, RestoreHook);
 
    return 0;
}