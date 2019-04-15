#pragma once

#include <stdexcept>

namespace sqliteexp {
namespace core {
namespace exceptions_base {

class sqliteexper_error_base
    :public std::runtime_error
{
public:
    sqliteexper_error_base(const std::string &aMessage, const int &aErrorCode)
        :runtime_error(aMessage), m_ErrorCode(aErrorCode)
    {}

    int GetErrorCode() const
    {
        return m_ErrorCode;
    }

private:
    int m_ErrorCode;
};

}
}
}
