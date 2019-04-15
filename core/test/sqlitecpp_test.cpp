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
    SqliteCpp("db.db")
    << "CREATE TABLE report;" << [](){}
    << "INSERT INTO report value;" << [](){};
    SqliteCpp("db.db").Execute(SqliteQuery() << "CREATE TABLE report;" << "INSERT INTO report value;" << "SELECT * FROM " << 1 << 2 << "sff;")<<[](){};
    SqliteCpp("db.db") << SqliteQuery() << "CREATE TABLE report;" << "INSERT INTO report value;" << "SELECT * FROM " << 1 << 2 << "sff;" << [](){};
    
    SqliteCpp("db.db") << "CREATE TABLE report;" << "INSERT INTO report value;";
}
