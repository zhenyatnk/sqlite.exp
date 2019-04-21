#pragma once

#include <sqlite.exp/core/BaseExceptions.hpp>

#include <sqlite3.h>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <iostream>

namespace sqliteexp {
namespace core {

    //Safe and unsafe implement interface
class SqliteRowGetter
{
public:
    SqliteRowGetter(const std::shared_ptr<sqlite3_stmt>& row)
        :m_row(row)
    {}
    
    template <class Type> Type Get(int index) const;
    
    template <> int Get<int>(int index) const
    {
        //sqlite3_column_count TODO
        SQLITE_CHECK_BOOL(SQLITE_INTEGER == sqlite3_column_type(m_row.get(), index)) << " index = " << index << " is type = " << sqlite3_column_type(m_row.get(), index);
        return sqlite3_column_int(m_row.get(), index);
    }
    template <> int64_t Get<int64_t>(int index) const
    {
        SQLITE_CHECK_BOOL(SQLITE_INTEGER == sqlite3_column_type(m_row.get(), index)) << " index = " << index << " is type = " << sqlite3_column_type(m_row.get(), index);
        return sqlite3_column_int64(m_row.get(), index);
    }
    template <> double Get<double>(int index) const
    {
        SQLITE_CHECK_BOOL(SQLITE_FLOAT == sqlite3_column_type(m_row.get(), index)) << " index = " << index << " is type = " << sqlite3_column_type(m_row.get(), index);
        return sqlite3_column_double(m_row.get(), index);
    }
    template <> std::string Get<std::string>(int index) const
    {
        SQLITE_CHECK_BOOL(SQLITE_TEXT == sqlite3_column_type(m_row.get(), index)) << " index = " << index << " is type = " << sqlite3_column_type(m_row.get(), index);
        return reinterpret_cast<const char*>(sqlite3_column_text(m_row.get(), index));
    }
    template <> std::wstring Get<std::wstring>(int index) const
    {
        SQLITE_CHECK_BOOL(SQLITE_TEXT == sqlite3_column_type(m_row.get(), index)) << " index = " << index << " is type = " << sqlite3_column_type(m_row.get(), index);
        return reinterpret_cast<const wchar_t*>(sqlite3_column_text16(m_row.get(), index));
    }
    
private:
    std::shared_ptr<sqlite3_stmt> m_row;
};
////////////////////////////////////////////////////////////////////////////////
class SqliteQuery
{
public:
    SqliteQuery() = default;
    
    template<class Type>
    SqliteQuery& operator<< (const Type& object)
    {
        std::stringstream str;
        str << m_text << object;
        m_text = str.str();
        return *this;
    }
    friend std::ostream& operator<< (std::ostream &out, const SqliteQuery &query)
    {
        out << query.m_text;
        return out;
    }
    const char* GetText()
    {
        return m_text.c_str();
    }
    
private:
    std::string m_text = {};
};
////////////////////////////////////////////////////////////////////////////////
class SqliteCpp;
    
class SqliteQueryExecuter
{
    friend SqliteCpp;
    
private:
    SqliteQueryExecuter(SqliteCpp& sqliteCpp, SqliteQuery query)
    :m_sqliteCpp(sqliteCpp),m_query(std::move(query)),m_runned(false)
    {}
    
public:
    using Callback = std::function<bool(const SqliteRowGetter&)>;
    static bool EmptyCallback(const SqliteRowGetter&){ return true;};
    
public:
    SqliteQueryExecuter(SqliteQueryExecuter&) = delete;
    SqliteQueryExecuter(SqliteQueryExecuter&& rght);
    
    ~SqliteQueryExecuter()
    {
        try
        {
            if(!m_runned)
                Run();
        }
        catch(std::exception& e)
        {
            std::cout << e.what() << std::endl; // TODO
        }
    }

    template<class Type, class = typename std::enable_if<!std::is_convertible<Type, Callback>::value>::type>
    SqliteQueryExecuter operator<< (const Type& object)
    {
        Run();
        return m_sqliteCpp << object;
    }
    
    SqliteCpp& operator<< (const Callback& func)
    {
        Run(func);
        return m_sqliteCpp;
    }
    void Run(const Callback& func = EmptyCallback);
    
private:
    SqliteCpp& m_sqliteCpp;
    SqliteQuery m_query = {};
    bool m_runned = false;
};
////////////////////////////////////////////////////////////////////////////////
class SqliteCpp
{
    friend class SqliteQueryExecuter;
    
public:
    explicit SqliteCpp(const std::string& db)
    {
        sqlite3 *sqlite;
        SQLITE_CHECK(sqlite3_open(db.c_str(), &sqlite));
        m_connection = std::shared_ptr<sqlite3>(sqlite, [](sqlite3 *object){ if(!!object) sqlite3_close(object); });
    }
    explicit SqliteCpp(const std::shared_ptr<sqlite3>& connection)
    :m_connection(connection)
    {}
    
    template<class Type>
    SqliteQueryExecuter operator<< (const Type& object)
    {
        return Execute(SqliteQuery() << object);
    }
    SqliteQueryExecuter operator<< (const SqliteQuery& object)
    {
        return Execute(object);
    }
    SqliteQueryExecuter Execute(const SqliteQuery& query)
    {
        return SqliteQueryExecuter(*this, query);
    }
private:
    std::shared_ptr<sqlite3> m_connection = nullptr;
};
////////////////////////////////////////////////////////////////////////////////
SqliteQueryExecuter::SqliteQueryExecuter(SqliteQueryExecuter&& rght)
    :m_sqliteCpp(rght.m_sqliteCpp),m_query(std::move(rght.m_query)),m_runned(false)
{
    rght.m_runned = true;
}
    
void SqliteQueryExecuter::Run(const Callback& func)
{
    m_runned = true;
    auto connection = m_sqliteCpp.m_connection;
    sqlite3_stmt *res;
    SQLITE_CHECK(sqlite3_prepare_v2(connection.get(), m_query.GetText(), -1, &res, nullptr))
        <<" Query: ["<< m_query.GetText() << "] Message: ["<<sqlite3_errmsg(connection.get()) << "]" ;
    std::shared_ptr<sqlite3_stmt> resShared(res, [](sqlite3_stmt* object){ if(!!object) sqlite3_finalize(object);});
    auto result = SQLITE_OK;
    while(SQLITE_ROW == (result = sqlite3_step(res)))
        if(!func(SqliteRowGetter(resShared)))
            break;
    
    SQLITE_CHECK_EX(result, make_errors<uint32_t>({SQLITE_ROW, SQLITE_DONE}))
        <<" Query: ["<< m_query.GetText() << "] Message: ["<<sqlite3_errmsg(connection.get()) << "]" ;
}
////////////////////////////////////////////////////////////////////////////////
}
}
