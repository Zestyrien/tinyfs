#pragma once
#include <string_view>

namespace tfs {

enum class Error {
  NotEnoughSpace,
  DiskInterfaceNotProvided,
  OutOfRange,
  PartitionTooSmall,
  InvalidDirectory,
  ElementAlreadyExists,
  ElementNotFound
};

std::wstring_view GetErrorInfo(Error const &error);

} // namespace tfs
