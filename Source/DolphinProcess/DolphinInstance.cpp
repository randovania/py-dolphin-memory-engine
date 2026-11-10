#include "DolphinInstance.h"
#include <cstddef>
#include <string>
#include <vector>
#include "DolphinStatus.h"
#include "IDolphinProcess.h"
#ifdef __linux__
#include "Linux/LinuxDolphinProcess.h"
#elif _WIN32
#include "Windows/WindowsDolphinProcess.h"
#elif __APPLE__
#include "Mac/MacDolphinProcess.h"
#endif

#include <memory>

#include "../Common/CommonUtils.h"
#include "../Common/MemoryCommon.h"

namespace DolphinComm
{
namespace {
IDolphinProcess* CreateBackend()
{
#ifdef __linux__
    return new LinuxDolphinProcess();
#elif _WIN32
    return new WindowsDolphinProcess();
#elif __APPLE__
    return new MacDolphinProcess();
#endif
}
}

std::vector<int> GetProcessIDs(const std::vector<std::string>& names)
{
    IDolphinProcess* tempProcess = CreateBackend();
    if (!tempProcess)
        return {};

    std::vector<int> pids = tempProcess->getProcessIDs(names);
    delete tempProcess;
    return pids;
}

DolphinInstance::~DolphinInstance()
{
    free();
}

void DolphinInstance::init()
{
    Common::UpdateMemoryValues();
    if (m_instance == nullptr)
    {
        m_instance = CreateBackend();
    }
}

void DolphinInstance::free()
{
    delete m_instance;
    m_instance = nullptr;
}

void DolphinInstance::hook(const int pid)
{
    init();
    if (!m_instance->setPID(pid))
    {
        m_status = DolphinStatus::NOT_FOUND;
    }
    else if (!m_instance->obtainEmuRAMInformations())
    {
        m_status = DolphinStatus::EMULATION_NOT_STARTED;
    }
    else 
    {
        m_status = DolphinStatus::HOOKED;
    }
}

void DolphinInstance::unHook()
{
    delete m_instance;
    m_instance = nullptr;
    m_status = DolphinStatus::UNHOOKED;
}

DolphinStatus DolphinInstance::getStatus()
{
    return m_status;
}

std::string DolphinInstance::getLastErrorMessage()
{
    return m_instance ? m_instance->getLastErrorMessage() : std::string();
}

bool DolphinInstance::readFromRAM(const u32 offset, char* buffer, const size_t size, const bool withBSwap)
{
    return m_instance ? m_instance->readFromRAM(offset, buffer, size, withBSwap) : false;
}

bool DolphinInstance::writeToRAM(const u32 offset, const char* buffer, const size_t size, const bool withBSwap)
{
    return m_instance ? m_instance->writeToRAM(offset, buffer, size, withBSwap) : false;
}

int DolphinInstance::getPID()
{
    return m_instance ? m_instance->getPID() : -1;
}

u64 DolphinInstance::getEmuRAMAddressStart()
{
    return m_instance ? m_instance->getEmuRAMAddressStart() : 0;
}

bool DolphinInstance::isARAMAccessible()
{
    return m_instance ? m_instance->isARAMAccessible() : false;
}

u64 DolphinInstance::getARAMAddressStart()
{
    return m_instance ? m_instance->getARAMAddressStart() : 0;
}

bool DolphinInstance::isMEM2Present()
{
    return m_instance ? m_instance->isMEM2Present() : false;
}

bool DolphinInstance::isValidConsoleAddress(const u32 address)
{
    if (!isValid())
        return false;

    if (address >= Common::MEM1_START && address < Common::GetMEM1End())
        return true;

    if (isMEM2Present() && (address >= Common::MEM2_START && address < Common::GetMEM2End()))
        return true;

    if (isARAMAccessible() && (address >= Common::ARAM_START && address < Common::ARAM_END))
        return true;

    return false;
}

bool DolphinInstance::isValid() const
{
    return m_status == DolphinStatus::HOOKED;
}

size_t DolphinInstance::getRAMTotalSize()
{
    if (isMEM2Present())
    {
        return Common::GetMEM1SizeReal() + Common::GetMEM2SizeReal();
    }
    else if (isARAMAccessible())
    {
        return Common::GetMEM1SizeReal() + Common::ARAM_SIZE;
    }
    else
    {
        return Common::GetMEM1SizeReal();
    }
}

Common::MemOperationReturnCode DolphinInstance::readEntireRAM(char* buffer)
{
    // MEM2, if enabled, is read right after MEM1 in the buffer so both regions are contigous
    if (isMEM2Present())
    {
        if (!readFromRAM(Common::dolphinAddrToOffset(Common::MEM1_START, false), 
                    buffer, Common::GetMEM1SizeReal(), false))
            return Common::MemOperationReturnCode::operationFailed;

        // Read Wii extra RAM
        if (!readFromRAM(Common::dolphinAddrToOffset(Common::MEM2_START, false), 
                    buffer + Common::GetMEM1SizeReal(), Common::GetMEM2SizeReal(), false))
            return Common::MemOperationReturnCode::operationFailed;
    }
    else if (isARAMAccessible())
    {
        // Read ARAM
        if (!readFromRAM(Common::dolphinAddrToOffset(Common::ARAM_START, true), 
                    buffer, Common::ARAM_SIZE, false))
            return Common::MemOperationReturnCode::operationFailed;

        // Read GameCube and Wii basic RAM
        if (!readFromRAM(Common::dolphinAddrToOffset(Common::MEM1_START, true), 
                    buffer + Common::ARAM_SIZE, Common::GetMEM1SizeReal(), false))
            return Common::MemOperationReturnCode::operationFailed;
    }
    else
    {
        if (!readFromRAM(Common::dolphinAddrToOffset(Common::MEM1_START, false), 
                    buffer, Common::GetMEM1SizeReal(), false))
            return Common::MemOperationReturnCode::operationFailed;
    }

    return Common::MemOperationReturnCode::OK;
}

std::string DolphinInstance::getFormattedValueFromMemory(const u32 ramIndex, Common::MemType memType,
        size_t memSize, Common::MemBase memBase, bool memIsUnsigned)
{
    std::unique_ptr<char[]> buffer(new char[memSize]);
    readFromRAM(ramIndex, buffer.get(), memSize, false);
    return Common::formatMemoryToString(buffer.get(), memType, memSize, memBase, memIsUnsigned,
        Common::shouldBeBSwappedForType(memType));
}
}