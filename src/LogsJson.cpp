#include "LogsJson.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdint.h>

namespace fs = std::filesystem;

static const fs::path LOG_DIR = "logs";

/*
Consider Adding Atomic Write.
*/

void ensureLogDirExists() {
  std::error_code ec;
  fs::create_directories(LOG_DIR, ec);
}

fs::path makeLogPath(const std::string &filename) {
  ensureLogDirExists();
  return LOG_DIR / extensionJSON(filename);
}

// ensure correct fileformat (on windows)
std::string extensionJSON(const std::string &filename) {
  if (filename.size() < 5 || filename.substr(filename.size() - 5) != ".json") {
    return filename + ".json";
  }
  return filename;
}

// Append new JSON object to a JSON array in file
void appendToJSON(const std::string &filename, const json &entry) {
  fs::path fullname = makeLogPath(filename);
  json json_arr;

  std::ifstream inFile(fullname); // opens stream for reading file
  /*
  inFile.good() -> checks file stream ok (file exist)
  inFile.peek() != eof() checks if the file is not empty -> only read json if
  its not empty inFile >> json_arr -> parse all the existing content into json
  object.
  */
  if (inFile.good() && inFile.peek() != std::ifstream::traits_type::eof()) {
    try {
      inFile >> json_arr;
    } catch (...) {
      json_arr = json::array(); // reset if corrupt
    }
  } else { // if json doesnt exist or empty -> make new empty array
    json_arr = json::array();
  }
  inFile.close(); // closes file

  if (!json_arr.is_array()) {
    json_arr = json::array(); // make sure its array
  }

  json_arr.push_back(entry); // append the new json object array

  std::ofstream out(fullname,
                    std::ios::trunc); // open file to write (overwrite)
  if (!out.is_open()) {
    std::cerr << "Could not open: " << fullname << "for writing\n";
    return;
  }
  out << json_arr.dump(4); // pretty print (4 times indented)
  out.close();             // close the file.
}

void createJSON(const std::string &filename) {
  fs::path fullname = makeLogPath(filename);
  json json_arr = json::array(); // Create empty array.

  std::ofstream out(fullname, std::ios::trunc);
  if (!out.is_open()) {
    std::cerr << "Could not open: " << fullname << " for writing\n";
    return;
  }

  out << json_arr.dump(4); // write to empty file.
  out.close();             // close the file.
}