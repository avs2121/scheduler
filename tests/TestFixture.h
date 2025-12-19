#pragma once
#include <fstream>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

class TestFixture {
public:
  TestFixture(const std::string &testname);

  virtual ~TestFixture();
  virtual void setUp() {};
  virtual void tearDown() {};

  // File management
  void trackFile(std::string &filename);
  void cleanUp();

  // Assertions
  void assert_true(bool condition, const std::string &message);
  void assert_equal(int expected, int actual, const std::string &message);

  // Utilities
  bool fileExists(std::string &filename);
  int countLines(std::string &filename);

  // test execution
  void run(std::function<void()> testFunc);

protected:
  std::string testname;
  std::vector<std::string> test_files;
};