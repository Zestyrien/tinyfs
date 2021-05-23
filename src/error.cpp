#include "error.h"

auto const notEnoughSpace = L"Operation failed: not enough space on the disk.";
auto const unknown = L"Unknown error";

std::wstring_view tfs::GetErrorInfo(tfs::Error const &error) {
  switch (error) {
  case (tfs::Error::NotEnoughSpace):
    return notEnoughSpace;
  default:
    break;
  }

  return unknown;
}
