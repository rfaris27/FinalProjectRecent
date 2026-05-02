#include <gtest/gtest.h>

#include "../src/model/CustomerOptions.h"
#include "MockDal.h"

class CustomerOptionsTest : public ::testing::Test {
   protected:
    MockCustomerOptionsDal mockDal;
    CustomerOptions *service;

    void SetUp() override {
        mockDal.addAccount(100, "user1", "12345", "John", 5000.0, "Active", false);
        service = new CustomerOptions(&mockDal);
    }

    void TearDown() override { delete service; }
};

TEST_F(CustomerOptionsTest, WithdrawValid) {
    Account a(100, "user1", "12345", "John", 5000.0, "Active", false);
    TransactionResult result = service->withdraw(a, 1000.0);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.message, "Cash Successfully Withdrawn");
    EXPECT_DOUBLE_EQ(result.newBalance, 4000.0);
}

TEST_F(CustomerOptionsTest, WithdrawEntireBalance) {
    Account a(100, "user1", "12345", "John", 5000.0, "Active", false);
    TransactionResult result = service->withdraw(a, 5000.0);

    EXPECT_TRUE(result.success);
    EXPECT_DOUBLE_EQ(result.newBalance, 0.0);
}

TEST_F(CustomerOptionsTest, WithdrawInsufficientFunds) {
    Account a(100, "user1", "12345", "John", 5000.0, "Active", false);
    TransactionResult result = service->withdraw(a, 10000.0);

    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.message, "Insufficient funds.");
    EXPECT_DOUBLE_EQ(result.newBalance, 5000.0);
}

TEST_F(CustomerOptionsTest, WithdrawNegativeAmount) {
    Account a(100, "user1", "12345", "John", 5000.0, "Active", false);
    TransactionResult result = service->withdraw(a, -500.0);

    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.message, "Invalid amount.");
}

TEST_F(CustomerOptionsTest, WithdrawZero) {
    Account a(100, "user1", "12345", "John", 5000.0, "Active", false);
    TransactionResult result = service->withdraw(a, 0.0);

    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.message, "Invalid amount.");
}

TEST_F(CustomerOptionsTest, DepositValid) {
    Account a(100, "user1", "12345", "John", 5000.0, "Active", false);
    TransactionResult result = service->deposit(a, 2000.0);

    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.message, "Cash Deposited Successfully.");
    EXPECT_DOUBLE_EQ(result.newBalance, 7000.0);
}

TEST_F(CustomerOptionsTest, DepositNegativeAmount) {
    Account a(100, "user1", "12345", "John", 5000.0, "Active", false);
    TransactionResult result = service->deposit(a, -1000.0);

    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.message, "Invalid amount.");
}

TEST_F(CustomerOptionsTest, DepositZero) {
    Account a(100, "user1", "12345", "John", 5000.0, "Active", false);
    TransactionResult result = service->deposit(a, 0.0);

    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.message, "Invalid amount.");
}

TEST_F(CustomerOptionsTest, DepositLargeAmount) {
    Account a(100, "user1", "12345", "John", 5000.0, "Active", false);
    TransactionResult result = service->deposit(a, 1000000.0);

    EXPECT_TRUE(result.success);
    EXPECT_DOUBLE_EQ(result.newBalance, 1005000.0);
}

TEST_F(CustomerOptionsTest, GetBalance) {
    Account a(100, "user1", "12345", "John", 5000.0, "Active", false);
    EXPECT_DOUBLE_EQ(service->getBalance(a), 5000.0);
}

TEST_F(CustomerOptionsTest, GetBalanceZero) {
    Account a(101, "user2", "22222", "Jane", 0.0, "Active", false);
    EXPECT_DOUBLE_EQ(service->getBalance(a), 0.0);
}

TEST_F(CustomerOptionsTest, WithdrawFromNonexistentAccount) {
    Account a(999, "ghost", "99999", "Ghost", 5000.0, "Active", false);
    TransactionResult result = service->withdraw(a, 100.0);

    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.message, "Database update failed.");
}
