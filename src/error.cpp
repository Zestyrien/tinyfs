#include "error.h"

auto const notEnoughSpace = L"Operation failed: not enough space on the disk.";
auto const diskInterfaceNotProvided =
    L"Internal error: disk interface not provided.";
auto const outOfRange = L"Operation failed: out of range.";
auto const partitionTooSmall = L"Operation failed: the partition is too small, "
                               L"minimum size for the partition is 15 blocks.";
auto const invalidDirectory = L"Error attempt to read an invalid directory.";
auto const elementAlreadyExists =
    L"Error the file or directory already present.";
auto const elementNotFound = L"Error element not found.";
auto const unknown = L"Unknown error";

std::wstring_view tfs::GetErrorInfo(tfs::Error const &error) {
  switch (error) {
  case (tfs::Error::NotEnoughSpace):
    return notEnoughSpace;
  case (tfs::Error::DiskInterfaceNotProvided):
    return diskInterfaceNotProvided;
  case (tfs::Error::OutOfRange):
    return outOfRange;
  case (tfs::Error::PartitionTooSmall):
    return partitionTooSmall;
  case (tfs::Error::InvalidDirectory):
    return invalidDirectory;
  case (tfs::Error::ElementAlreadyExists):
    return elementAlreadyExists;
  case (tfs::Error::ElementNotFound):
    return elementNotFound;
  default:
    break;
  }

  return unknown;
}
