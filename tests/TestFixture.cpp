#include "TestFixture.h"

#include <filesystem>

#include "LogsJson.h"

static std::filesystem::path LOG_DIR = "logs";

TestFixture::TestFixture(const std::string& testname) : testname(testname)
{
}

TestFixture::~TestFixture()
{
    cleanUp();
}

void TestFixture::trackFile(const std::string& filename)
{
    test_files.push_back((LOG_DIR / filename).string());
}

void TestFixture::cleanUp()
{
    for (auto& files : test_files)
    {
        std::remove(files.c_str());
    }
    test_files.clear();
    test_files.shrink_to_fit();  // clear and minimize idiom.
}

void TestFixture::assert_true(bool condition, const std::string& message)
{
    if (!condition)
    {
        throw std::runtime_error("Assertion failed: " + message);
    }
}

void TestFixture::assert_equal(int actual, int expected, const std::string& message)
{
    if (expected != actual)
    {
        throw std::runtime_error("Assertion failed. Actual: " + std::to_string(actual) +
                                 " Expected: " + std::to_string(expected) + message);
    }
}

bool TestFixture::fileExists(const std::string& filename)
{
    // std::ifstream file(LOG_DIR / filename);
    // return file.good();
    return std::filesystem::exists(makeLogPath(filename));
}

int TestFixture::countLines(std::string& filename)
{
    int number_of_lines = 0;
    std::string line;
    std::ifstream file(LOG_DIR / extensionJSON(filename));

    while (std::getline(file, line))
    {
        if (!line.empty())
        {
            number_of_lines++;
        }
    }
    return number_of_lines;
}

void TestFixture::run(std::function<void()> testFunc)
{
    try
    {
        setUp();
        testFunc();
        tearDown();
        std::cout << testname << " passed " << std::endl;
    }
    catch (const std::exception& e)
    {
        cleanUp();
        std::cout << testname << " failed with: " << e.what() << std::endl;
        throw;
    }
}
