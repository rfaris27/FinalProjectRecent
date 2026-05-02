#include <gtest/gtest.h>

#include "../src/model/Account.h"

TEST(AccountTest, ConstructorSetsAllFields) {
    Account a(100, "user1", "12345", "John Doe", 5000.0, "Active", false);

    EXPECT_EQ(a.getAccountNumber(), 100);
    EXPECT_EQ(a.getLogin(), "user1");
    EXPECT_EQ(a.getPin(), "12345");
    EXPECT_EQ(a.getHolderName(), "John Doe");
    EXPECT_DOUBLE_EQ(a.getBalance(), 5000.0);
    EXPECT_EQ(a.getStatus(), "Active");
    EXPECT_FALSE(a.isAdmin());
}

TEST(AccountTest, AdminAccountFlag) {
    Account admin(101, "admin1", "99999", "Admin User", 0.0, "Active", true);
    EXPECT_TRUE(admin.isAdmin());
}

TEST(AccountTest, ZeroBalance) {
    Account a(102, "user2", "11111", "Jane", 0.0, "Active", false);
    EXPECT_DOUBLE_EQ(a.getBalance(), 0.0);
}

TEST(AccountTest, NegativeBalance) {
    Account a(103, "user3", "22222", "Bob", -100.0, "Inactive", false);
    EXPECT_DOUBLE_EQ(a.getBalance(), -100.0);
    EXPECT_EQ(a.getStatus(), "Inactive");
}

TEST(AccountTest, EmptyStrings) {
    Account a(104, "", "", "", 0.0, "", false);
    EXPECT_EQ(a.getLogin(), "");
    EXPECT_EQ(a.getHolderName(), "");
}

TEST(AccountTest, LargeBalance) {
    Account a(105, "rich", "55555", "Rich Person", 99999999.99, "Active", false);
    EXPECT_DOUBLE_EQ(a.getBalance(), 99999999.99);
}
