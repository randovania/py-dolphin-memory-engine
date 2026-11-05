#pragma once

#include <string>
#include <vector>

#include "../Common/CommonTypes.h"
#include "../Common/MemoryCommon.h"
#include "DolphinStatus.h"
#include "IDolphinProcess.h"

namespace DolphinComm
{
class DolphinInstance
{
public:
    DolphinInstance() = default;
    ~DolphinInstance();

    DolphinInstance(const DolphinInstance&) = delete;
    DolphinInstance& operator=(const DolphinInstance&) = delete;

    void init();
    void free();
    void hook(const int pid);
    void unHook();
    bool readFromRAM(const u32 offset, char* buffer, const size_t size, const bool withBSwap);
    bool writeToRAM(const u32 offset, const char* buffer, const size_t size, const bool withBSwap);
    int getPID();
    u64 getEmuRAMAddressStart();
    DolphinStatus getStatus();
    std::string getLastErrorMessage();
    bool isARAMAccessible();
    u64 getARAMAddressStart();
    bool isMEM2Present();
    size_t getRAMTotalSize();
    Common::MemOperationReturnCode readEntireRAM(char* buffer);
    std::string getFormattedValueFromMemory(const u32 ramIndex, Common::MemType memType,
        size_t memSize, Common::MemBase memBase, bool memIsUnsigned);
    bool isValidConsoleAddress(const u32 address);

    bool isValid() const;

private:
    IDolphinProcess* m_instance = nullptr;
    DolphinStatus m_status = DolphinStatus::UNHOOKED;
};

// Stateless dolphin process enumeration that does not depend on or allocate any DolphinInstance state.
std::vector<int> GetProcessIDs(const std::vector<std::string>& names = {});
}