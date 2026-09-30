#ifdef _WIN32

#pragma once

#include <windows.h>

#include "../IDolphinProcess.h"

namespace DolphinComm
{
class WindowsDolphinProcess : public IDolphinProcess
{
public:
  WindowsDolphinProcess();
  ~WindowsDolphinProcess() override;
  bool setPID(const int pid) override;
  std::vector<int> getProcessIDs(const std::vector<std::string>& names) override;
  bool obtainEmuRAMInformations() override;
  bool readFromRAM(const u32 offset, char* buffer, const size_t size,
                   const bool withBSwap) override;
  bool writeToRAM(const u32 offset, const char* buffer, const size_t size,
                  const bool withBSwap) override;

private:
  HANDLE m_hDolphin = NULL;
};
}  // namespace DolphinComm
#endif
