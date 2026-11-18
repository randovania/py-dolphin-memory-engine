#ifdef __linux__

#pragma once

#include <fstream>
#include <sstream>
#include <string>

#include "../IDolphinProcess.h"

namespace DolphinComm
{
class LinuxDolphinProcess : public IDolphinProcess
{
public:
  LinuxDolphinProcess() {}
  bool setPID(const int pid) override;
  std::vector<int> getProcessIDs(const std::vector<std::string>& names) override;
  bool obtainEmuRAMInformations() override;
  bool readFromRAM(const u32 offset, char* buffer, size_t size, const bool withBSwap) override;
  bool writeToRAM(const u32 offset, const char* buffer, const size_t size,
                  const bool withBSwap) override;
};
}  // namespace DolphinComm
#endif
