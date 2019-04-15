#pragma once

#include <sqlite.exp/core/BaseExceptions.hpp>

#include <sqlite3.h>
#include <memory>
#include <sstream>
#include <functional>

namespace sqliteexp {
namespace core {
  
class Connection
{
    std::shared_ptr<sqlite3> m_connection;
};
    
void ExecuteQuery(const Connection& connection, const std::string& query);
   
    
class SqliteQuery
{
    friend class SqliteExecuter;
public:
    SqliteQuery() = default;
    
    template<class Type>
    SqliteQuery& operator<< (const Type& object)
    {
        std::stringstream str;
        str << m_query << object;
        m_query = str.str();
        return *this;
    }
    
    friend std::ostream& operator<< (std::ostream &out, const SqliteQuery &query)
    {
        out << query.m_query;
        return out;
    }
    
private:
    std::string m_query;
};
    
class SqliteCpp;
    
class SqliteQueryExecuter
{
    friend SqliteCpp;
private:
    SqliteQueryExecuter(SqliteCpp& sqliteCpp, SqliteQuery query)
    :m_sqliteCpp(sqliteCpp),m_query(std::move(query)),m_runned(false)
    {}
    
public:
    SqliteQueryExecuter(SqliteQueryExecuter&) = delete;
    SqliteQueryExecuter(SqliteQueryExecuter&&) = default;
    
    ~SqliteQueryExecuter()
    {
        if(!m_runned)
            Run();
    }
    
    template<class Type>
    SqliteCpp& operator<< (const Type& object)
    {
        Run();
        m_sqliteCpp << object;
        return m_sqliteCpp;
    }
    SqliteCpp& operator<< (const std::function<void(void)>& func)
    {
        Run(func);
        return m_sqliteCpp;
    }
    void Run(const std::function<void(void)>& func = [](){})
    {
        m_runned = true;
    }
    
private:
    SqliteCpp& m_sqliteCpp;
    SqliteQuery m_query;
    bool m_runned;
};
    
class SqliteCpp
{
public:
    explicit SqliteCpp(const std::string& db)
    {}
    
    template<class Type>
    SqliteQueryExecuter operator<< (const Type& object)
    {
        return Execute(SqliteQuery() << object);
    }
    SqliteQueryExecuter Execute(const SqliteQuery& query)
    {
        return SqliteQueryExecuter(*this, query);
    }
};

}
}
