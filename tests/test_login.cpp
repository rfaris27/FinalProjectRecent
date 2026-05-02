#include <gtest/gtest.h>

#include "../src/model/Login.h"
#include "MockDal.h"

class LoginTest : public ::testing::Test {
   protected:
    MockLoginDal mockDal;
    Login *loginService;

    void SetUp() override {
        mockDal.addAccount(100, "Adnan123", "12345", "Adnan", 160000.0, "Active", true);
        mockDal.addAccount(101, "John456", "11111", "John Doe", 5000.0, "Active", false);
        loginService = new Login(&mockDal);
    }

    void TearDown() override { delete loginService; }
};

TEST_F(LoginTest, ValidAdminLogin) {
    auto account = loginService->authenticate("Adnan123", "12345");
    ASSERT_NE(account, nullptr);
    EXPECT_EQ(account->getHolderName(), "Adnan");
    EXPECT_TRUE(account->isAdmin());
}

TEST_F(LoginTest, ValidCustomerLogin) {
    auto account = loginService->authenticate("John456", "11111");
    ASSERT_NE(account, nullptr);
    EXPECT_EQ(account->getHolderName(), "John Doe");
    EXPECT_FALSE(account->isAdmin());
}

TEST_F(LoginTest, InvalidLogin) {
    auto account = loginService->authenticate("wrong", "12345");
    EXPECT_EQ(account, nullptr);
}

TEST_F(LoginTest, InvalidPin) {
    auto account = loginService->authenticate("Adnan123", "00000");
    EXPECT_EQ(account, nullptr);
}

TEST_F(LoginTest, EmptyLogin) {
    auto account = loginService->authenticate("", "12345");
    EXPECT_EQ(account, nullptr);
}

TEST_F(LoginTest, EmptyPin) {
    auto account = loginService->authenticate("Adnan123", "");
    EXPECT_EQ(account, nullptr);
}

TEST_F(LoginTest, BothEmpty) {
    auto account = loginService->authenticate("", "");
    EXPECT_EQ(account, nullptr);
}
