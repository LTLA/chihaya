#include <gtest/gtest.h>
#include "chihaya/chihaya.hpp"
#include "utils.h"

class BinaryLogicTest : public ::testing::TestWithParam<int> {
public:
    BinaryLogicTest() : path("Test_binary_logic.h5") {}
protected:
    std::string path;

    static H5::Group binary_logic_opener(H5::Group& handle, const std::string& name, const std::string& method, int version) {
        auto ghandle = operation_opener(handle, name, "binary logic");
        add_version_string(ghandle, version);
        add_string_scalar(ghandle, "method", method);
        return ghandle;
    }
};

TEST_P(BinaryLogicTest, Simple) {
    auto version = GetParam();

    {
        H5::H5File fhandle(path, H5F_ACC_TRUNC);
        auto ghandle = binary_logic_opener(fhandle, "hello", "&&", version);
        mock_array_opener(ghandle, "left", { 13, 19 }, version, "INTEGER");
        mock_array_opener(ghandle, "right", { 13, 19 }, version, "INTEGER");
    }

    auto output = chihaya::validate(path, "hello"); 
    EXPECT_EQ(output.type, chihaya::BOOLEAN);
}

TEST_P(BinaryLogicTest, Mixed) {
    auto version = GetParam();

    {
        H5::H5File fhandle(path, H5F_ACC_TRUNC);
        auto ghandle = binary_logic_opener(fhandle, "hello", "||", version);
        mock_array_opener(ghandle, "left", { 13, 19 }, version, "INTEGER");
        mock_array_opener(ghandle, "right", { 13, 19 }, version, "FLOAT");
    }

    auto output = chihaya::validate(path, "hello"); 
    EXPECT_EQ(output.type, chihaya::BOOLEAN);
}

TEST_P(BinaryLogicTest, Errors) {
    auto version = GetParam();

    {
        H5::H5File fhandle(path, H5F_ACC_TRUNC);
        binary_logic_opener(fhandle, "hello", "||", version);
    }
    expect_error([&]() -> void { chihaya::validate(path, "hello"); }, "'left'");

    {
        H5::H5File fhandle(path, H5F_ACC_TRUNC);
        auto ghandle = binary_logic_opener(fhandle, "hello", "||", version);
        mock_array_opener(ghandle, "left", { 13, 19 }, version, "STRING");
    }
    expect_error([&]() -> void { chihaya::validate(path, "hello"); }, "'left' should be integer, float or boolean");

    {
        H5::H5File fhandle(path, H5F_ACC_TRUNC);
        auto ghandle = binary_logic_opener(fhandle, "hello", "||", version);
        mock_array_opener(ghandle, "left", { 13, 19 }, version, "INTEGER");
    }
    expect_error([&]() -> void { chihaya::validate(path, "hello"); }, "'right'");

    {
        H5::H5File fhandle(path, H5F_ACC_TRUNC);
        auto ghandle = binary_logic_opener(fhandle, "hello", "&&", version);
        mock_array_opener(ghandle, "left", { 10, 5 }, version, "INTEGER");
        mock_array_opener(ghandle, "right", { 13, 19 }, version, "INTEGER");
    }
    expect_error([&]() -> void { chihaya::validate(path, "hello"); }, "'left' and 'right' should have the same");
}

TEST_P(BinaryLogicTest, MethodErrors) {
    auto version = GetParam();

    {
        H5::H5File fhandle(path, H5F_ACC_TRUNC);
        auto ghandle = binary_logic_opener(fhandle, "hello", "&&", version);
        mock_array_opener(ghandle, "left", { 13, 19 }, version, "INTEGER");
        mock_array_opener(ghandle, "right", { 13, 19 }, version, "INTEGER");
        ghandle.unlink("method");
    }
    expect_error([&]() -> void { chihaya::validate(path, "hello"); }, "'method'");

    {
        H5::H5File fhandle(path, H5F_ACC_TRUNC);
        auto ghandle = binary_logic_opener(fhandle, "hello", "foo", version);
        mock_array_opener(ghandle, "left", { 13, 19 }, version, "INTEGER");
        mock_array_opener(ghandle, "right", { 13, 19 }, version, "INTEGER");
    }
    expect_error([&]() -> void { chihaya::validate(path, "hello"); }, "unrecognized 'method'");
}

INSTANTIATE_TEST_SUITE_P(
    BinaryLogic,
    BinaryLogicTest,
    ::testing::Values(0, 1000000, 1100000)
);

