#include <sqlite.exp/core/BaseExceptions.hpp>
#include <sqlite.exp/core/sqlitecpp.hpp>

#include <gtest/gtest.h>
//--------------------------------------------------
using namespace sqliteexp::core;

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

TEST_F(SqliteCppTest, Create)
{
    int count = 0;
    std::string str;
    SqliteCpp sql(":memory:");
    sql
    << "CREATE TABLE report (first int, second text);"
    << "INSERT INTO report (first, second) VALUES (1, 'text');"
    << "SELECT * FROM report" << [&count, &str](const SqliteRowGetter& getter){++count; str = getter.Get<std::string>(1); return true;};
    ASSERT_EQ(1, count);
    ASSERT_STREQ("text", str.c_str());
}
