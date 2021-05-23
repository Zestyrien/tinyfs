#include <iostream>
#include <string>

#include "error.h"
#include "maybe.h"

int main() {

  Maybe<std::wstring, tfs::Error> o(L"ciao");
  Maybe<std::wstring, tfs::Error> e(tfs::Error::NotEnoughSpace);

  if (!o.error()) {
    std::wcout << o.getResult() << std::endl;
  }

  if (e.error()) {
    std::wcout << tfs::GetErrorInfo(e.getError()) << std::endl;
  }

  std::wcout << L"hello" << std::endl;
  return 0;
}
