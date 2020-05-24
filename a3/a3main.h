#pragma once

#include "main.h"
#include "hlapi/hlapi.h"

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
    
        armaProc->Write(this->l_aStringPtr, &ArmaStringValue[0], l_aStringLen);
    }
    
    std::string ReadString()
    {
        std::string result;
        result.resize(this->l_aStringLen);
    
        armaProc->Read(this->l_aStringPtr, &result[0], this->l_aStringLen);
    
        return result;
    }
};

struct ArmaStringRestore
{
    WinProcess *armaProc;
    
    ArmaString RefArmaString;
    std::string SavedString;

    bool IsValid;
    
    ArmaStringRestore(){ this->IsValid = false; };
    ~ArmaStringRestore()
    {
        Restore();
    }
    
    ArmaStringRestore(WinProcess *armaProc, ArmaString InArmaString)
    {
        this->armaProc = armaProc;
    
        this->RefArmaString = InArmaString;
        this->SavedString = RefArmaString.ReadString();
    }
    
    void Restore()
    {
        if (!IsValid)
            return;

        RefArmaString.WriteString(SavedString);
        IsValid = false;
    }
};

class a3
{
public:
    static void init(arguments arguments, std::string scriptcontent);

private:
    static ArmaStringRestore InjectScript(WinProcess *armaProc, uint64_t l_armaBase, std::string l_script);
    static void AnticheatKiller(WinProcess *armaProc, uint64_t l_armaBase);
    //static std::vector<std::string> DumpScripts(WinProcess *armaProc, uint64_t l_armaBase);
    static bool FilepatchingEnable(WinProcess *armaProc, uint64_t l_armaBase);
    //static void FilepatchingDisable(WinProcess *armaProc, uint64_t l_armaBase);
};