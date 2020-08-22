#include "DummyDolphinProcess.h"
#include "../../Common/CommonUtils.h"

namespace DolphinComm
{
bool DummyDolphinProcess::obtainEmuRAMInformations()
{
  return false;
}

bool DummyDolphinProcess::findPID()
{
  return false;
}

bool DummyDolphinProcess::readFromRAM(const u32 offset, char* buffer, const size_t size,
                                      const bool withBSwap)
{
  return false;
}

bool DummyDolphinProcess::writeToRAM(const u32 offset, const char* buffer, const size_t size,
                                     const bool withBSwap)
{
  return false;
}
} // namespace DolphinComm
