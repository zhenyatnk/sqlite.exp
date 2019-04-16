#pragma once

#include <stdexcept>
#include <vector>
#include <sstream>
#include <iomanip>

#include <sqlite3.h>

namespace sqliteexp {
namespace core {
namespace exceptions_base {

class sqliteexper_error_base
    :public std::runtime_error
{
public:
    sqliteexper_error_base(const std::string &aMessage, const int &aErrorCode)
        :runtime_error(aMessage), m_ErrorCode(aErrorCode), m_message(aMessage)
    {
        std::stringstream str;
        str << m_message << " Error: 0x" << std::setfill('0') << std::setw(8) << std::hex << GetErrorCode();
        m_message = str.str();
    }
    
    sqliteexper_error_base(const std::string &aMessage)
    :runtime_error(aMessage), m_ErrorCode(0), m_message(aMessage)
    {}

    template<class Type>
    sqliteexper_error_base& operator << (const Type& object)
    {
        std::stringstream str;
        str << m_message << object;
        m_message = str.str();
        return *this;
    }
    const char* what() const _NOEXCEPT override
    {
        return m_message.c_str();
    }
    
    int GetErrorCode() const
    {
        return m_ErrorCode;
    }

private:
    int m_ErrorCode;
    std::string m_message;
};

inline bool SQLITE_SUCCEESS(uint32_t result, const std::vector<uint32_t>& success = {})
{
    if(result == SQLITE_OK)
        return true;
    if(success.empty())
        return false;
    return std::find(success.begin(), success.end(), result) != success.end();
}
    
}
}
}

template<class Type>
std::vector<Type> make_errors(const std::initializer_list<Type>& container)
{
    return container;
}

#define SQLITE_CHECK_EX(expr, container)    \
for(uint32_t ___result = expr;!sqliteexp::core::exceptions_base::SQLITE_SUCCEESS(___result, container);) \
throw sqliteexp::core::exceptions_base::sqliteexper_error_base(#expr, ___result)

#define SQLITE_CHECK_BOOL(expr)    \
if(!(expr)) \
throw sqliteexp::core::exceptions_base::sqliteexper_error_base(#expr)

#define SQLITE_CHECK(expr)   SQLITE_CHECK_EX(expr, {})
