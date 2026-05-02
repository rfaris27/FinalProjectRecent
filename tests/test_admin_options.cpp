#include <gtest/gtest.h>

#include "../src/model/AdminOptions.h"
#include "MockDal.h"

class AdminOptionsTest : public ::testing::Test {
   protected:
    MockAdminOptionsDal mockDal;
    AdminOptions *service;

    void SetUp() override {
        mockDal.addAccount(100, "admin1", "12345", "Admin", 0.0, "Active", true);
        mockDal.addAccount(101, "John456", "11111", "John Doe", 5000.0, "Active", false);
        mockDal.addAccount(102, "Jane789", "22222", "Jane Doe", 3000.0, "Active", false);
        service = new AdminOptions(&mockDal);
    }

    void TearDown() override { delete service; }
};

TEST_F(AdminOptionsTest, CreateAccountValid) {
    AdminResult result = service->createAccount("NewUser", "33333", "New Person", 1000.0, "Active");
    EXPECT_TRUE(result.success);
    EXPECT_GE(result.accountNumber, 100);
    EXPECT_NE(result.message.find("Account Successfully Created"), std::string::npos);
}

TEST_F(AdminOptionsTest, CreateAccountInvalidPinTooShort) {
    AdminResult result = service->createAccount("User", "123", "Person", 1000.0, "Active");
    EXPECT_FALSE(result.success);
    EXPECT_NE(result.message.find("Pin Code must be an integer of length 5"), std::string::npos);
}

TEST_F(AdminOptionsTest, CreateAccountInvalidPinLetters) {
    AdminResult result = service->createAccount("User", "abcde", "Person", 1000.0, "Active");
    EXPECT_FALSE(result.success);
    EXPECT_NE(result.message.find("Pin Code must be an integer of length 5"), std::string::npos);
}

TEST_F(AdminOptionsTest, CreateAccountEmptyLogin) {
    AdminResult result = service->createAccount("", "33333", "Person", 1000.0, "Active");
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.message, "Login cannot be empty.");
}

TEST_F(AdminOptionsTest, CreateAccountEmptyName) {
    AdminResult result = service->createAccount("User", "33333", "", 1000.0, "Active");
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.message, "Holder name cannot be empty.");
}

TEST_F(AdminOptionsTest, DeleteAccountValid) {
    AdminResult result = service->deleteAccount(101);
    EXPECT_TRUE(result.success);
    EXPECT_EQ(result.message, "Account Deleted Successfully");
}

TEST_F(AdminOptionsTest, DeleteAccountNotFound) {
    AdminResult result = service->deleteAccount(999);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.message, "Account not found.");
}

TEST_F(AdminOptionsTest, DeleteAdminAccountBlocked) {
    AdminResult result = service->deleteAccount(100);
    EXPECT_FALSE(result.success);
    EXPECT_EQ(result.message, "Account not found.");
}

TEST_F(AdminOptionsTest, GetHolderNameValid) {
    std::string name = service->getHolderName(101);
    EXPECT_EQ(name, "John Doe");
}

TEST_F(AdminOptionsTest, GetHolderNameNotFound) {
    std::string name = service->getHolderName(999);
    EXPECT_EQ(name, "");
}

TEST_F(AdminOptionsTest, GetHolderNameAdmin) {
    std::string name = service->getHolderName(100);
    EXPECT_EQ(name, "");
}

TEST_F(AdminOptionsTest, UpdateFieldHolderName) {
    AdminResult result = service->updateAccountField(101, "HolderName", "Johnny");
    EXPECT_TRUE(result.success);

    auto account = service->searchAccount(101);
    ASSERT_NE(account, nullptr);
    EXPECT_EQ(account->getHolderName(), "Johnny");
}

TEST_F(AdminOptionsTest, UpdateFieldBalance) {
    AdminResult result = service->updateAccountField(101, "Balance", "9999.99");
    EXPECT_TRUE(result.success);

    auto account = service->searchAccount(101);
    ASSERT_NE(account, nullptr);
    EXPECT_DOUBLE_EQ(account->getBalance(), 9999.99);
}

TEST_F(AdminOptionsTest, UpdateFieldInvalidPin) {
    AdminResult result = service->updateAccountField(101, "Pin", "abc");
    EXPECT_FALSE(result.success);
    EXPECT_NE(result.message.find("Pin Code must be an integer of length 5"), std::string::npos);
}

TEST_F(AdminOptionsTest, UpdateFieldValidPin) {
    AdminResult result = service->updateAccountField(101, "Pin", "99999");
    EXPECT_TRUE(result.success);
}

TEST_F(AdminOptionsTest, UpdateFieldNotFound) {
    AdminResult result = service->updateAccountField(999, "HolderName", "Ghost");
    EXPECT_FALSE(result.success);
}

TEST_F(AdminOptionsTest, SearchAccountValid) {
    auto account = service->searchAccount(101);
    ASSERT_NE(account, nullptr);
    EXPECT_EQ(account->getHolderName(), "John Doe");
    EXPECT_DOUBLE_EQ(account->getBalance(), 5000.0);
}

TEST_F(AdminOptionsTest, SearchAccountNotFound) {
    auto account = service->searchAccount(999);
    EXPECT_EQ(account, nullptr);
}

TEST_F(AdminOptionsTest, SearchAdminAccountReturnsNull) {
    auto account = service->searchAccount(100);
    EXPECT_EQ(account, nullptr);
}

TEST_F(AdminOptionsTest, CreateAccountInvalidPinTooLong) {
    AdminResult result = service->createAccount("User", "1234567", "Person", 1000.0, "Active");
    EXPECT_FALSE(result.success);
}

TEST_F(AdminOptionsTest, UpdateStatus) {
    AdminResult result = service->updateAccountField(101, "Status", "Inactive");
    EXPECT_TRUE(result.success);

    auto account = service->searchAccount(101);
    ASSERT_NE(account, nullptr);
    EXPECT_EQ(account->getStatus(), "Inactive");
}

TEST_F(AdminOptionsTest, UpdateLogin) {
    AdminResult result = service->updateAccountField(101, "Login", "NewLogin");
    EXPECT_TRUE(result.success);

    auto account = service->searchAccount(101);
    ASSERT_NE(account, nullptr);
    EXPECT_EQ(account->getLogin(), "NewLogin");
}
