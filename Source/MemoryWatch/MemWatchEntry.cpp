#include "MemWatchEntry.h"

#include <algorithm>
#include <cstring>
#include <ios>
#include <sstream>
#include <utility>

#include "../Common/CommonUtils.h"
#include "../DolphinProcess/DolphinAccessor.h"

MemWatchEntry::MemWatchEntry(const std::string& label, const u32 consoleAddress,
                             const Common::MemType type, const Common::MemBase base,
                             const bool isUnsigned, const size_t length,
                             const bool isBoundToPointer)
    : m_label(label), m_consoleAddress(consoleAddress), m_type(type), m_isUnsigned(isUnsigned),
      m_base(base), m_boundToPointer(isBoundToPointer), m_length(length)
{
  m_memory = new char[getSizeForType(m_type, m_length)];
}

MemWatchEntry::MemWatchEntry()
{
  m_type = Common::MemType::type_byte;
  m_base = Common::MemBase::base_decimal;
  m_memory = new char[sizeof(u8)];
  *m_memory = 0;
  m_isUnsigned = false;
  m_consoleAddress = 0x80000000;
}

MemWatchEntry::MemWatchEntry(MemWatchEntry* entry)
    : m_label(entry->m_label), m_consoleAddress(entry->m_consoleAddress), m_type(entry->m_type),
      m_isUnsigned(entry->m_isUnsigned), m_base(entry->m_base),
      m_boundToPointer(entry->m_boundToPointer), m_pointerOffsets(entry->m_pointerOffsets),
      m_length(entry->m_length), m_isValidPointer(entry->m_isValidPointer)
{
  m_memory = new char[getSizeForType(entry->getType(), entry->getLength())];
  std::memcpy(m_memory, entry->getMemory(), getSizeForType(entry->getType(), entry->getLength()));
}

MemWatchEntry::MemWatchEntry(const MemWatchEntry& other)
  : m_targetProcess(other.m_targetProcess), 
    m_label(other.m_label),
    m_consoleAddress(other.m_consoleAddress),
    m_lock(other.m_lock),
    m_type(other.m_type),
    m_base(other.m_base),
    m_isUnsigned(other.m_isUnsigned),
    m_boundToPointer(other.m_boundToPointer),
    m_pointerOffsets(other.m_pointerOffsets),
    m_isValidPointer(other.m_isValidPointer),
    m_length(other.m_length),
    m_freezeMemSize(other.m_freezeMemSize)
{
  size_t memSize = Common::getSizeForType(m_type, m_length);
  m_memory = new char[memSize];
  if (other.m_memory)
    std::memcpy(m_memory, other.m_memory, memSize);
  else
    std::fill(m_memory, m_memory + memSize, 0);

  if (other.m_freezeMemory && m_freezeMemSize > 0)
  {
    m_freezeMemory = new char[m_freezeMemSize];
    std::memcpy(m_freezeMemory, other.m_freezeMemory, m_freezeMemSize);
  }
  else 
  {
    m_freezeMemory = nullptr;
  }
}

MemWatchEntry::MemWatchEntry(MemWatchEntry&& other) noexcept
  : m_targetProcess(other.m_targetProcess), 
    m_label(std::move(other.m_label)),
    m_consoleAddress(other.m_consoleAddress),
    m_lock(other.m_lock),
    m_type(other.m_type),
    m_base(other.m_base),
    m_isUnsigned(other.m_isUnsigned),
    m_boundToPointer(other.m_boundToPointer),
    m_pointerOffsets(std::move(other.m_pointerOffsets)),
    m_isValidPointer(other.m_isValidPointer),
    m_memory(other.m_memory),
    m_freezeMemory(other.m_freezeMemory),
    m_freezeMemSize(other.m_freezeMemSize),
    m_length(other.m_length)
{
  other.m_memory = nullptr;
  other.m_freezeMemory = nullptr;
  other.m_targetProcess = nullptr;
}

MemWatchEntry& MemWatchEntry::operator=(const MemWatchEntry& other)
{
  if (this == &other)
    return *this;

  delete[] m_memory;
  delete[] m_freezeMemory;

  m_targetProcess = other.m_targetProcess;
  m_label = other.m_label;
  m_consoleAddress = other.m_consoleAddress;
  m_lock = other.m_lock; 
  m_type = other.m_type;
  m_base = other.m_base; 
  m_isUnsigned = other.m_isUnsigned; 
  m_boundToPointer = other.m_boundToPointer;
  m_pointerOffsets = other.m_pointerOffsets; 
  m_isValidPointer = other.m_isValidPointer;
  m_memory = other.m_memory; 
  m_length = other.m_length;
  m_freezeMemory = other.m_freezeMemory; 
  m_freezeMemSize = other.m_freezeMemSize;

  size_t memSize = Common::getSizeForType(m_type, m_length);
  m_memory = new char[memSize];
  if (other.m_memory)
    std::memcpy(m_memory, other.m_memory, memSize);
  else
    std::fill(m_memory, m_memory + memSize, 0);

  if (other.m_freezeMemory && m_freezeMemSize > 0)
  {
    m_freezeMemory = new char[m_freezeMemSize];
    std::memcpy(m_freezeMemory, other.m_freezeMemory, m_freezeMemSize);
  }
  else 
  {
    m_freezeMemory = nullptr;
  }

  return *this;
}

MemWatchEntry& MemWatchEntry::operator=(MemWatchEntry&& other) noexcept
{
  if (this == &other)
    return *this;

  delete[] m_memory;
  delete[] m_freezeMemory;

  m_targetProcess = other.m_targetProcess;
  m_label = std::move(other.m_label);
  m_consoleAddress = other.m_consoleAddress;
  m_lock = other.m_lock; 
  m_type = other.m_type;
  m_base = other.m_base; 
  m_isUnsigned = other.m_isUnsigned; 
  m_boundToPointer = other.m_boundToPointer;
  m_pointerOffsets = std::move(other.m_pointerOffsets); 
  m_isValidPointer = other.m_isValidPointer;
  m_memory = other.m_memory; 
  m_freezeMemory = other.m_freezeMemory; 
  m_freezeMemSize = other.m_freezeMemSize;
  m_length = other.m_length;

  other.m_memory = nullptr;
  other.m_freezeMemory = nullptr;
  other.m_targetProcess = nullptr;

  return *this;
}

MemWatchEntry::~MemWatchEntry()
{
  delete[] m_memory;
  delete[] m_freezeMemory;
}

std::string MemWatchEntry::getLabel() const
{
  return m_label;
}

size_t MemWatchEntry::getLength() const
{
  return m_length;
}

Common::MemType MemWatchEntry::getType() const
{
  return m_type;
}

u32 MemWatchEntry::getConsoleAddress() const
{
  return m_consoleAddress;
}

bool MemWatchEntry::isLocked() const
{
  return m_lock;
}

bool MemWatchEntry::isBoundToPointer() const
{
  return m_boundToPointer;
}

Common::MemBase MemWatchEntry::getBase() const
{
  return m_base;
}

int MemWatchEntry::getPointerOffset(const int index) const
{
  return m_pointerOffsets.at(index);
}

std::vector<int> MemWatchEntry::getPointerOffsets() const
{
  return m_pointerOffsets;
}

size_t MemWatchEntry::getPointerLevel() const
{
  return m_pointerOffsets.size();
}

char* MemWatchEntry::getMemory() const
{
  return m_memory;
}

bool MemWatchEntry::isUnsigned() const
{
  return m_isUnsigned;
}

void MemWatchEntry::setLabel(const std::string& label)
{
  m_label = label;
}

void MemWatchEntry::setConsoleAddress(const u32 address)
{
  m_consoleAddress = address;
}

void MemWatchEntry::setTypeAndLength(const Common::MemType type, const size_t length)
{
  size_t oldSize = getSizeForType(m_type, m_length);
  m_type = type;
  m_length = length;
  size_t newSize = getSizeForType(m_type, m_length);
  if (oldSize != newSize)
  {
    delete[] m_memory;
    m_memory = new char[newSize];
    std::fill(m_memory, m_memory + newSize, 0);
  }
}

void MemWatchEntry::setBase(const Common::MemBase base)
{
  m_base = base;
}

void MemWatchEntry::setLock(const bool doLock)
{
  m_lock = doLock;
  if (m_lock)
  {
    if (readMemoryFromRAM() == Common::MemOperationReturnCode::OK)
    {
      m_freezeMemSize = getSizeForType(m_type, m_length);
      m_freezeMemory = new char[m_freezeMemSize];
      std::memcpy(m_freezeMemory, m_memory, m_freezeMemSize);
    }
  }
  else if (m_freezeMemory != nullptr)
  {
    m_freezeMemSize = 0;
    delete[] m_freezeMemory;
    m_freezeMemory = nullptr;
  }
}

void MemWatchEntry::setSignedUnsigned(const bool isUnsigned)
{
  m_isUnsigned = isUnsigned;
}

void MemWatchEntry::setBoundToPointer(const bool boundToPointer)
{
  m_boundToPointer = boundToPointer;
}

void MemWatchEntry::setPointerOffset(const int pointerOffset, int index)
{
  m_pointerOffsets[index] = pointerOffset;
}

void MemWatchEntry::removeOffset()
{
  m_pointerOffsets.pop_back();
}

void MemWatchEntry::addOffset(const int offset)
{
  m_pointerOffsets.push_back(offset);
}

void MemWatchEntry::setTargetProcess(DolphinComm::DolphinInstance* targetProcess)
{
  m_targetProcess = targetProcess;
}

bool MemWatchEntry::targetReadFromRAM(const u32 offset, char* buffer, const size_t size, const bool withBSwap)
{
  if (m_targetProcess)
    return m_targetProcess->isValid() && m_targetProcess->readFromRAM(offset, buffer, size, withBSwap);

  return DolphinComm::DolphinAccessor::readFromRAM(offset, buffer, size, withBSwap);
}

bool MemWatchEntry::targetWriteToRAM(const u32 offset, const char* buffer, const size_t size, const bool withBSwap)
{
  if (m_targetProcess)
    return m_targetProcess->isValid() && m_targetProcess->writeToRAM(offset, buffer, size, withBSwap);

  return DolphinComm::DolphinAccessor::writeToRAM(offset, buffer, size, withBSwap);
}

bool MemWatchEntry::targetIsARAMAccessible()
{
  if (m_targetProcess)
    return m_targetProcess->isValid() && m_targetProcess->isARAMAccessible();
  return DolphinComm::DolphinAccessor::isARAMAccessible();
}

bool MemWatchEntry::targetIsValidConsoleAddress(const u32 address)
{
  if (m_targetProcess)
    return m_targetProcess->isValid() && m_targetProcess->isValidConsoleAddress(address);

  return DolphinComm::DolphinAccessor::isValidConsoleAddress(address);
}

Common::MemOperationReturnCode MemWatchEntry::freeze()
{
  Common::MemOperationReturnCode writeCode = writeMemoryToRAM(m_freezeMemory, m_freezeMemSize);
  return writeCode;
}

u32 MemWatchEntry::getAddressForPointerLevel(const int level)
{
  if (!m_boundToPointer && level > m_pointerOffsets.size() && level > 0)
    return 0;

  u32 address = m_consoleAddress;
  char addressBuffer[sizeof(u32)] = {0};
  for (int i = 0; i < level; ++i)
  {
    if (targetReadFromRAM(
            Common::dolphinAddrToOffset(address, targetIsARAMAccessible()),
            addressBuffer, sizeof(u32), true))
    {
      std::memcpy(&address, addressBuffer, sizeof(u32));
      if (targetIsValidConsoleAddress(address))
        address += m_pointerOffsets.at(i);
      else
        return 0;
    }
    else
    {
      return 0;
    }
  }
  return address;
}

std::string MemWatchEntry::getAddressStringForPointerLevel(const int level)
{
  u32 address = getAddressForPointerLevel(level);
  if (address == 0)
  {
    return "???";
  }
  else
  {
    std::stringstream ss;
    ss << std::hex << std::uppercase << address;
    return ss.str();
  }
}

Common::MemOperationReturnCode MemWatchEntry::readMemoryFromRAM()
{
  u32 realConsoleAddress = m_consoleAddress;
  if (m_boundToPointer)
  {
    char realConsoleAddressBuffer[sizeof(u32)] = {0};
    for (int offset : m_pointerOffsets)
    {
      if (targetReadFromRAM(
              Common::dolphinAddrToOffset(realConsoleAddress,
                                          targetIsARAMAccessible()),
              realConsoleAddressBuffer, sizeof(u32), true))
      {
        std::memcpy(&realConsoleAddress, realConsoleAddressBuffer, sizeof(u32));
        if (targetIsValidConsoleAddress(realConsoleAddress))
        {
          realConsoleAddress += offset;
        }
        else
        {
          m_isValidPointer = false;
          return Common::MemOperationReturnCode::invalidPointer;
        }
      }
      else
      {
        return Common::MemOperationReturnCode::operationFailed;
      }
    }
    // Resolve sucessful
    m_isValidPointer = true;
  }

  if (!targetIsValidConsoleAddress(realConsoleAddress))
    return Common::MemOperationReturnCode::OK;

  if (targetReadFromRAM(
          Common::dolphinAddrToOffset(realConsoleAddress,
                                      targetIsARAMAccessible()),
          m_memory, getSizeForType(m_type, m_length), shouldBeBSwappedForType(m_type)))
    return Common::MemOperationReturnCode::OK;
  return Common::MemOperationReturnCode::operationFailed;
}

Common::MemOperationReturnCode MemWatchEntry::writeMemoryToRAM(const char* memory,
                                                               const size_t size)
{
  u32 realConsoleAddress = m_consoleAddress;
  if (m_boundToPointer)
  {
    char realConsoleAddressBuffer[sizeof(u32)] = {0};
    for (int offset : m_pointerOffsets)
    {
      if (targetReadFromRAM(
              Common::dolphinAddrToOffset(realConsoleAddress,
                                          targetIsARAMAccessible()),
              realConsoleAddressBuffer, sizeof(u32), true))
      {
        std::memcpy(&realConsoleAddress, realConsoleAddressBuffer, sizeof(u32));
        if (targetIsValidConsoleAddress(realConsoleAddress))
        {
          realConsoleAddress += offset;
        }
        else
        {
          m_isValidPointer = false;
          return Common::MemOperationReturnCode::invalidPointer;
        }
      }
      else
      {
        return Common::MemOperationReturnCode::operationFailed;
      }
    }
    // Resolve sucessful
    m_isValidPointer = true;
  }

  if (!targetIsValidConsoleAddress(realConsoleAddress))
    return Common::MemOperationReturnCode::OK;

  if (targetWriteToRAM(
          Common::dolphinAddrToOffset(realConsoleAddress,
                                      targetIsARAMAccessible()),
          memory, size, shouldBeBSwappedForType(m_type)))
    return Common::MemOperationReturnCode::OK;
  return Common::MemOperationReturnCode::operationFailed;
}

std::string MemWatchEntry::getStringFromMemory() const
{
  if ((m_boundToPointer && !m_isValidPointer) ||
      !DolphinComm::DolphinAccessor::isValidConsoleAddress(m_consoleAddress))
    return "???";
  return Common::formatMemoryToString(m_memory, m_type, m_length, m_base, m_isUnsigned);
}

Common::MemOperationReturnCode MemWatchEntry::writeMemoryFromString(const std::string& inputString)
{
  Common::MemOperationReturnCode writeReturn = Common::MemOperationReturnCode::OK;
  size_t sizeToWrite = 0;
  char* buffer =
      Common::formatStringToMemory(writeReturn, sizeToWrite, inputString, m_base, m_type, m_length);
  if (writeReturn != Common::MemOperationReturnCode::OK)
    return writeReturn;

  writeReturn = writeMemoryToRAM(buffer, sizeToWrite);
  if (writeReturn == Common::MemOperationReturnCode::OK)
  {
    if (m_lock)
      std::memcpy(m_freezeMemory, buffer, m_freezeMemSize);
    delete[] buffer;
    return writeReturn;
  }
  else
  {
    delete[] buffer;
    return writeReturn;
  }
}
