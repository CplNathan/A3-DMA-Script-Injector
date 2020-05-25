#include "a3main.h"
#include "a3values.h"

void a3::init(arguments arguments, std::string scriptcontent)
{
    FILE *out = stdout;
    pid_t pid;

    FILE *pipe = popen("pidof qemu-system-x86_64", "r");
    fscanf(pipe, "%d", &pid);
    pclose(pipe);

    try
    {
        WinContext ctx(pid);
        ctx.processList.Refresh();

        WinProcess *ArmaProc = ctx.processList.FindProcNoCase("arma3_x64.exe");
        WinProcess *BattleyeProc = ctx.processList.FindProcNoCase("arma3battleye.exe");

        uint64_t l_armaBase = ArmaProc->modules.GetModuleInfo("Arma3_x64.exe")->info.baseAddress;
        uint64_t l_battleyeBase = BattleyeProc->modules.GetModuleInfo("Arma3BattlEye.exe")->info.baseAddress;

        if (l_armaBase && l_battleyeBase)
        {
            if (arguments.noinfistar)
                    AnticheatKiller(ArmaProc, l_armaBase);
            
            if (arguments.filepatching)
            {
                if (FilepatchingEnable(ArmaProc, l_armaBase))
                {
                    printf("Enabled FilePatching!\n");
                }
                else
                {
                    printf("Error while enabling FilePatching!\n");
                }
            }

            // Script Injection
            ArmaStringRestore Restore = InjectScript(ArmaProc, l_armaBase, scriptcontent);

            // Restore countdown
            uint64_t timeStart = clock();
            int lastRestoreUI = 30;
            while ((Restore.IsValid && arguments.restorehook) || arguments.noinfistar)
            {
                if (arguments.noinfistar)
                    AnticheatKiller(ArmaProc, l_armaBase);

                if (Restore.IsValid && arguments.restorehook)
                {
                    if ((clock() - timeStart) / CLOCKS_PER_SEC >= 30)
                    {
                        Restore.Restore();
                        printf("Restored EH\n");
                    }
                    else
                    {
                        int thisRestoreUI = std::abs((int)((clock() - timeStart) / CLOCKS_PER_SEC - 30));
                        if (thisRestoreUI != lastRestoreUI)
                        {
                            lastRestoreUI = thisRestoreUI;
                            printf("Restoring EH in: %d\n", thisRestoreUI);
                        }
                    }
                }
            } // END WHILE
            printf("Nothing left to do, exiting.\n");
        }
        else
        {
            printf("Arma Found: %s\tBattlEye Found: %s\nExiting\n", l_armaBase ? "Yes" : "No", l_battleyeBase ? "Yes" : "No");
        }
    }
    catch (VMException &e)
    {
        fprintf(out, "Initialization error: %d\n", e.value);
    }

    fclose(out);
}

ArmaStringRestore a3::InjectScript(WinProcess *armaProc, uint64_t l_armaBase, std::string l_script)
{
    uint64_t l_World = armaProc->Read<uint64_t>(l_armaBase + Arma3::Offsets::World);

    uint64_t l_CameraOn = armaProc->Read<uint64_t>(l_World + Arma3::Offsets::CameraOn);
    l_CameraOn = armaProc->Read<uint64_t>(l_CameraOn + 0x8);

    uint64_t l_EventHandlerTable = armaProc->Read<uint64_t>(l_CameraOn + Arma3::Offsets::EventHandlerTable);
    int l_EventHandlerSize = armaProc->Read<int>(l_CameraOn + Arma3::Offsets::EventHandlerSize);

    ArmaStringRestore Restore;

    for (int i = 0; i < l_EventHandlerSize; i++)
    {
        uint64_t l_CurrentEventHandlerIndex = l_EventHandlerTable + (i * 0x28);

        uint64_t l_EventHandlerContainer = armaProc->Read<uint64_t>(l_CurrentEventHandlerIndex + 0x10);
        uint64_t l_EventHandlerTrigger = l_CurrentEventHandlerIndex + 0x8;

        ArmaString l_ArmaString = ArmaString(armaProc, l_EventHandlerContainer);

        int scriptSize = l_script.size();

        if (l_ArmaString.l_aStringLen > scriptSize && l_ArmaString.l_aStringLen > 0)
        {
            Restore = ArmaStringRestore(armaProc, l_ArmaString);

            armaProc->Write((l_EventHandlerTrigger), Arma3::EventHandlerEnum::InventoryOpened);
            l_ArmaString.WriteString(l_script);

            printf("Event Handler Set\n");

            break;
        }
        else
        {
            printf("Script too large for event handler container: %d/%d at index %d\n", scriptSize, l_ArmaString.l_aStringLen, i);
        }
    }

    return Restore;
}

void a3::AnticheatKiller(WinProcess *armaProc, uint64_t l_armaBase)
{
    uint64_t l_World = armaProc->Read<uint64_t>(l_armaBase + Arma3::Offsets::World);

    uint64_t l_scriptVM = armaProc->Read<uint64_t>(l_World + Arma3::Offsets::scriptVM);
    int l_scriptVMSize = armaProc->Read<int>(l_World + Arma3::Offsets::scriptVMSize);

    for (int i = 0; i < l_scriptVMSize; i++)
    {
        uint64_t l_script = armaProc->Read<uint64_t>(l_scriptVM + (i * 0x10));

        ArmaString l_ArmaString = ArmaString(armaProc, l_script + Arma3::Offsets::scriptVMString);

        std::string result = l_ArmaString.ReadString();
        for (int i = 0; i < (int)Arma3::anticheatterms.size(); i++)
        {
            std::string curTerm = Arma3::anticheatterms[i];
            if (result.find(curTerm) != std::string::npos)
            {
                armaProc->Write(l_script + 0x508, true);
                printf("Disabling anticheat thread at index %d for term %s\n", i, curTerm.c_str());
                break;
            }
        }
    }
}

/*
std::vector<std::string> a3::DumpScripts(WinProcess *armaProc, uint64_t l_armaBase)
{
    uint64_t l_World = armaProc->Read<uint64_t>(l_armaBase + Arma3::Offsets::World);

    uint64_t l_scriptVM = armaProc->Read<uint64_t>(l_World + Arma3::Offsets::scriptVM);
    int l_scriptVMSize = armaProc->Read<int>(l_World + Arma3::Offsets::scriptVMSize);

    for (int i = 0; i < l_scriptVMSize; i++)
    {
        uint64_t l_script = armaProc->Read<uint64_t>(l_scriptVM + (i * 0x10));

        ArmaString l_ArmaString = ArmaString(armaProc, l_script + Arma3::Offsets::scriptVMString);

        std::string result = l_ArmaString.ReadString();

    }
}
*/

bool a3::FilepatchingEnable(WinProcess *armaProc, uint64_t l_armaBase)
{
    uintptr_t values[11] = { 0 };

    for (size_t i = 0; i < 11; i++)
    {
        if (!armaProc->Read(l_armaBase + Arma3::FilePatching::BLOCKBEGIN + i * sizeof(uintptr_t), &values[i], sizeof(uintptr_t)))
            return false;
    }

    uint64_t CONSTANT1 = Arma3::FilePatching::CONSTANT1;
    uint64_t CONSTANT2 = Arma3::FilePatching::CONSTANT1;
    armaProc->Write(l_armaBase + Arma3::FilePatching::BLOCKBEGIN, &CONSTANT1, sizeof(uintptr_t));
    armaProc->Write(l_armaBase + Arma3::FilePatching::BLOCKBEGIN + sizeof(uintptr_t), &CONSTANT2, sizeof(uintptr_t));

    uintptr_t value1 = values[2] + Arma3::FilePatching::OFFSET1;
    armaProc->Write(l_armaBase + Arma3::FilePatching::BLOCKBEGIN + 2 * sizeof(uintptr_t), &value1, sizeof(uintptr_t));
    armaProc->Write(l_armaBase + Arma3::FilePatching::BLOCKBEGIN + 4 * sizeof(uintptr_t), &value1, sizeof(uintptr_t));
    uintptr_t value2 = l_armaBase + Arma3::FilePatching::OFFSET2;
    armaProc->Write(l_armaBase + Arma3::FilePatching::BLOCKBEGIN + 3 * sizeof(uintptr_t), &value2, sizeof(uintptr_t));

    uintptr_t value3 = values[8] + Arma3::FilePatching::OFFSET3;
    armaProc->Write(l_armaBase + Arma3::FilePatching::BLOCKBEGIN + 8 * sizeof(uintptr_t), &value3, sizeof(uintptr_t));
    armaProc->Write(l_armaBase + Arma3::FilePatching::BLOCKBEGIN + 10 * sizeof(uintptr_t), &value3, sizeof(uintptr_t));
    uintptr_t value4 = l_armaBase + Arma3::FilePatching::OFFSET4;
    armaProc->Write(l_armaBase + Arma3::FilePatching::BLOCKBEGIN + 9 * sizeof(uintptr_t), &value4, sizeof(uintptr_t));

    return true;
}

/*
void a3::FilepatchingDisable(WinProcess *armaProc, uint64_t l_armaBase)
{
    for (size_t i = 0; i < 11; i++)
    {
        armaProc->Write(l_armaBase + Arma3::FilePatching::BLOCKBEGIN + i * sizeof(uintptr_t), &values[i], sizeof(uintptr_t));
    }
}
*/
