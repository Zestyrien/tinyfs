#pragma once
#include <string_view>

namespace tfs {

enum class Error { NotEnoughSpace };

std::wstring_view GetErrorInfo(Error const &error);

} // namespace tfs
