#include "DolphinAccessor.h"
#include "DolphinInstance.h"

#include <cstring>
#include "../Common/MemoryCommon.h"

namespace DolphinComm
{
DolphinInstance DolphinAccessor::m_defaultInstance;

void DolphinAccessor::init()
{
  m_defaultInstance.init();
}

void DolphinAccessor::free()
{
  m_defaultInstance.free();
}

void DolphinAccessor::hook(const int pid)
{
  m_defaultInstance.hook(pid);
}

void DolphinAccessor::unHook()
{
  m_defaultInstance.unHook();
}

std::vector<int> DolphinAccessor::getProcessIDs(const std::vector<std::string>& names)
{
  return DolphinComm::GetProcessIDs(names);
}

DolphinStatus DolphinAccessor::getStatus()
{
  return m_defaultInstance.getStatus();
}

std::string DolphinAccessor::getLastErrorMessage()
{
  return m_defaultInstance.getLastErrorMessage();
}

bool DolphinAccessor::readFromRAM(const u32 offset, char* buffer, const size_t size,
                                  const bool withBSwap)
{
  return m_defaultInstance.readFromRAM(offset, buffer, size, withBSwap);
}

bool DolphinAccessor::writeToRAM(const u32 offset, const char* buffer, const size_t size,
                                 const bool withBSwap)
{
  return m_defaultInstance.writeToRAM(offset, buffer, size, withBSwap);
}

int DolphinAccessor::getPID()
{
  return m_defaultInstance.getPID();
}

u64 DolphinAccessor::getEmuRAMAddressStart()
{
  return m_defaultInstance.getEmuRAMAddressStart();
}

bool DolphinAccessor::isARAMAccessible()
{
  return m_defaultInstance.isARAMAccessible();
}

u64 DolphinAccessor::getARAMAddressStart()
{
  return m_defaultInstance.getARAMAddressStart();
}

bool DolphinAccessor::isMEM2Present()
{
  return m_defaultInstance.isMEM2Present();
}

bool DolphinAccessor::isValidConsoleAddress(const u32 address)
{
  return m_defaultInstance.isValidConsoleAddress(address);
}

size_t DolphinAccessor::getRAMTotalSize()
{
  return m_defaultInstance.getRAMTotalSize();
}

Common::MemOperationReturnCode DolphinAccessor::readEntireRAM(char* buffer)
{
  return m_defaultInstance.readEntireRAM(buffer);
}

std::string DolphinAccessor::getFormattedValueFromMemory(const u32 ramIndex, 
    Common::MemType memType, size_t memSize, Common::MemBase memBase, bool memIsUnsigned)
{
  return m_defaultInstance.getFormattedValueFromMemory(ramIndex, memType, memSize, 
    memBase, memIsUnsigned);
}
}  // namespace DolphinComm
