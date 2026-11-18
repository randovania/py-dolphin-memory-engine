#ifdef __APPLE__

#pragma once

#include <mach/mach.h>
#include "../IDolphinProcess.h"

namespace DolphinComm
{
class MacDolphinProcess : public IDolphinProcess
{
public:
  MacDolphinProcess() {}
  bool setPID(const int pid) override;
  std::vector<int> getProcessIDs(const std::vector<std::string>& names) override;
  bool obtainEmuRAMInformations() override;
  bool readFromRAM(const u32 offset, char* buffer, size_t size, const bool withBSwap) override;
  bool writeToRAM(const u32 offset, const char* buffer, const size_t size,
                  const bool withBSwap) override;

private:
  task_t m_task;
  task_t m_currentTask;
};
}  // namespace DolphinComm
#endif
