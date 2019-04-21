#include <sqlite.exp/core/BaseExceptions.hpp>
#include <sqlite.exp/core/sqlitecpp.hpp>

#include <gtest/gtest.h>
//--------------------------------------------------
using namespace sqliteexp::core;

namespace{
    std::shared_ptr<sqlite3> TestSqlConnection(const std::string& name)
    {
        sqlite3 *sqlite;
        SQLITE_CHECK(sqlite3_open(name.c_str(), &sqlite));
        return std::shared_ptr<sqlite3>(sqlite, [](sqlite3 *object){ if(!!object) sqlite3_close(object); });
    }
    void TestSqlExecuter(const std::shared_ptr<sqlite3>& connection, const std::string& query, const std::function<void(sqlite3_stmt*)>& func = {})
    {
        sqlite3_stmt *res;
        SQLITE_CHECK(sqlite3_prepare_v2(connection.get(), query.c_str(), -1, &res, nullptr))
            <<" Query: ["<< query << "] Message: ["<<sqlite3_errmsg(connection.get()) << "]" ;
        
        std::shared_ptr<sqlite3_stmt> resShared(res, [](sqlite3_stmt* object){ if(!!object) sqlite3_finalize(object);});
        auto result = SQLITE_OK;
        while(SQLITE_ROW == (result = sqlite3_step(res)))
            func(res);
        
        SQLITE_CHECK_EX(result, make_errors<uint32_t>({SQLITE_ROW, SQLITE_DONE}))
        <<" Query: ["<< query << "] Message: ["<<sqlite3_errmsg(connection.get()) << "]" ;
    }
}

class SqliteCppTest
    :public ::testing::Test
{
public:
    void SetUp()
    {}
};

TEST_F(SqliteCppTest, Interface)
{
  /*
    SqliteCpp(L"db.db")
    << "CREATE TABLE report;" << [](){std::cout << "1111"; return true;}
    << "INSERT INTO report value;" << [](){std::cout << "2222"; return true;};
    SqliteCpp(L"db.db").Execute(SqliteQuery() << "CREATE TABLE report;" << "INSERT INTO report value;" << "SELECT * FROM " << 1 << 2 << "sff;")<<[](){std::cout << "33333";};
    SqliteCpp(L"db.db") << SqliteQuery() << "CREATE TABLE report;" << "INSERT INTO report value;" << "SELECT * FROM " << 1 << 2 << "sff;" << [](){ std::cout << "4444";};
    
    SqliteCpp(L"db.db") << "CREATE TABLE report;" << "INSERT INTO report value;";
    SqliteCpp(L"db.db") << "CREATE TABLE report;" << "INSERT INTO report value;"<< [](){std::cout << "66666";};
    SqliteCpp(L"db.db") << "CREATE TABLE report;"<< [](){std::cout << "777";} << "INSERT INTO report value;";
   */
}

TEST_F(SqliteCppTest, Connection)
{
    ASSERT_NO_THROW(SqliteCpp(":memory:"));
}
TEST_F(SqliteCppTest, EmptyQuery)
{
    ASSERT_THROW(SqliteCpp(":memory:") << "" << SqliteQueryExecuter::EmptyCallback, exceptions_base::sqliteexper_error_base);
}
TEST_F(SqliteCppTest, Query_Correct)
{
    ASSERT_NO_THROW(SqliteCpp(":memory:") << "CREATE TABLE report (first int, second text);"<< SqliteQueryExecuter::EmptyCallback);
}
TEST_F(SqliteCppTest, Query_NoCorrect)
{
    ASSERT_THROW(SqliteCpp(":memory:") << "CREATE TABLE report" << SqliteQueryExecuter::EmptyCallback, exceptions_base::sqliteexper_error_base);
}
TEST_F(SqliteCppTest, Query_NoCorrect_Dtor)
{
    ASSERT_NO_THROW(SqliteCpp(":memory:") << "CREATE TABLE report");
}
TEST_F(SqliteCppTest, Query_Select_Count)
{
    int count = 0;
    auto connection = TestSqlConnection(":memory:");
    TestSqlExecuter(connection, "CREATE TABLE report (first int, second text);");
    TestSqlExecuter(connection, "INSERT INTO report (first, second) VALUES (1, 'first');");
    TestSqlExecuter(connection, "INSERT INTO report (first, second) VALUES (1, 'second');");
    
    SqliteCpp (connection) << "SELECT * FROM report" << [&count](const SqliteRowGetter& getter){++count; return true;};
    ASSERT_EQ(2, count);
}
TEST_F(SqliteCppTest, Query_Select_Data)
{
    std::string str;
    auto connection = TestSqlConnection(":memory:");
    TestSqlExecuter(connection, "CREATE TABLE report (first int, second text);");
    TestSqlExecuter(connection, "INSERT INTO report (first, second) VALUES (1, 'first');");
    TestSqlExecuter(connection, "INSERT INTO report (first, second) VALUES (1, 'second');");
    
    SqliteCpp (connection) << "SELECT * FROM report" << [&str](const SqliteRowGetter& getter){str+= getter.Get<std::string>(1); return true;};
    ASSERT_STREQ("firstsecond", str.c_str());
}
TEST_F(SqliteCppTest, Query_Select_Stop_Data)
{
    std::string str;
    auto connection = TestSqlConnection(":memory:");
    TestSqlExecuter(connection, "CREATE TABLE report (first int, second text);");
    TestSqlExecuter(connection, "INSERT INTO report (first, second) VALUES (1, 'first');");
    TestSqlExecuter(connection, "INSERT INTO report (first, second) VALUES (1, 'second');");
    
    SqliteCpp (connection) << "SELECT * FROM report" << [&str](const SqliteRowGetter& getter){str+= getter.Get<std::string>(1); return false;};
    ASSERT_STREQ("first", str.c_str());
}
TEST_F(SqliteCppTest, Query_Select_Stop_Count)
{
    int count = 0;
    auto connection = TestSqlConnection(":memory:");
    TestSqlExecuter(connection, "CREATE TABLE report (first int, second text);");
    TestSqlExecuter(connection, "INSERT INTO report (first, second) VALUES (1, 'first');");
    TestSqlExecuter(connection, "INSERT INTO report (first, second) VALUES (1, 'second');");

    SqliteCpp (connection) << "SELECT * FROM report" << [&count](const SqliteRowGetter& getter){++count; return false;};
    ASSERT_EQ(1, count);
}

TEST_F(SqliteCppTest, Query_Select_NotCallback)
{
    auto connection = TestSqlConnection(":memory:");
    TestSqlExecuter(connection, "CREATE TABLE report (first int, second text);");
    TestSqlExecuter(connection, "INSERT INTO report (first, second) VALUES (1, 'first');");
    TestSqlExecuter(connection, "INSERT INTO report (first, second) VALUES (1, 'second');");
    
    ASSERT_NO_THROW(SqliteCpp (connection) << "SELECT * FROM report");
}
TEST_F(SqliteCppTest, Query_Select_MixedCallback)
{
    int count = 0;
    auto connection = TestSqlConnection(":memory:");
    TestSqlExecuter(connection, "CREATE TABLE report (first int, second text);");
    TestSqlExecuter(connection, "INSERT INTO report (first, second) VALUES (1, 'first');");
    TestSqlExecuter(connection, "INSERT INTO report (first, second) VALUES (1, 'second');");
    
    SqliteCpp (connection) << "SELECT * FROM report" << "SELECT * FROM report"<< [&count](const SqliteRowGetter& getter){++count; return false;};
    ASSERT_EQ(1, count);
}
