#pragma once

#include <string>
#include <iostream>
#include "gtest/gtest.h"

class BaseTestCase : public testing::Test
{
public:
    BaseTestCase(): m_strFeature("BaseTestCase"), m_strTestName("") {}
    virtual ~ BaseTestCase() = default;

    virtual void SetUp() {}
    virtual void TearDown()  {}
    virtual void RecordTestCase(const std::string& given,
        const std::string& when,
        const std::string& then,
        const std::string& scenario="",
        const std::string& checkpoints="") {

        std::string strClassName(::testing::UnitTest::GetInstance()->current_test_info()->test_case_name());
        std::string strFuncName(::testing::UnitTest::GetInstance()->current_test_info()->name());
        std::string strTestName = strClassName + "." + strFuncName;

        RecordProperty("feature", m_strFeature);
        RecordProperty("scenario", scenario.empty()?strTestName:scenario);
        RecordProperty("given", given);
        RecordProperty("when", when);
        RecordProperty("then", then);
        RecordProperty("checkpoints", checkpoints);
    }
protected:
    std::string m_strFeature;
    std::string m_strTestName;
};