#include <sqliteexper/core/BaseExceptions.hpp>

#include <gtest/gtest.h>
#include <sqlite3.h>
//--------------------------------------------------
using namespace sqliteexper::core;

class Sqlite3ConfigTest
    :public ::testing::Test
{
public:
    void SetUp()
    {}
};

TEST_F(Sqlite3ConfigTest, Compile_ThreadSafe_Serialized)
{
    ASSERT_EQ(1, sqlite3_threadsafe());
}
