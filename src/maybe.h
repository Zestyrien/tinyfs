#pragma once

template <typename T, typename E> class Maybe {
private:
  T m_result;
  E m_error;
  bool m_hasError;

public:
  Maybe(T const &result) : m_hasError(false), m_result{result} {}
  Maybe(E const &error) : m_hasError(true), m_error{error} {}

  bool error() const { return m_hasError; }
  T getResult() const { return m_result; }
  E getError() const { return m_error; }
};
