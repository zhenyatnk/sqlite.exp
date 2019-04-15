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
    << "CREATE TABLE report;" << [](){std::cout << "1111";}
    << "INSERT INTO report value;" << [](){std::cout << "2222";};
    SqliteCpp("db.db").Execute(SqliteQuery() << "CREATE TABLE report;" << "INSERT INTO report value;" << "SELECT * FROM " << 1 << 2 << "sff;")<<[](){std::cout << "33333";};
    SqliteCpp("db.db") << SqliteQuery() << "CREATE TABLE report;" << "INSERT INTO report value;" << "SELECT * FROM " << 1 << 2 << "sff;" << [](){ std::cout << "4444";};
    
    SqliteCpp("db.db") << "CREATE TABLE report;" << "INSERT INTO report value;";
    SqliteCpp("db.db") << "CREATE TABLE report;" << "INSERT INTO report value;"<< [](){std::cout << "66666";};
    SqliteCpp("db.db") << "CREATE TABLE report;"<< [](){std::cout << "777";} << "INSERT INTO report value;";
}
