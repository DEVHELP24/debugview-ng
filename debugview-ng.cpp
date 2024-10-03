#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <ctime>
#include <sys/types.h>
#include <pwd.h>

#define COMMAND_LENGTH 256

// SPDX-License-Identifier: MIT
// Author: [NAZY-OS]
// Description: [Shows various system errors and warnings]

// Function to display usage information
void usage() {
    std::cerr << "Usage: debugview-ng [-h] [-m] [-e] [-s] [-x] [-o output_directory]" << std::endl;
    std::cerr << "  -h: Display this help message" << std::endl;
    std::cerr << "  -m: List loaded kernel modules" << std::endl;
    std::cerr << "  -e: Check for errors related to kernel modules" << std::endl;
    std::cerr << "  -s: Find errors related to systemd services during startup" << std::endl;
    std::cerr << "  -x: Find errors and warnings in Xorg logs" << std::endl;
    std::cerr << "  -o output_directory: Specify the output directory to save results" << std::endl;
    exit(1);
}

// Function to execute a shell command and return the output as a string
std::string exec(const char* cmd) {
    char buffer[COMMAND_LENGTH];
    std::string result;

    FILE* pipe = popen(cmd, "r");
    if (!pipe) return "popen() failed!";
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }
    pclose(pipe);
    return result;
}

// Function to get loaded kernel modules
std::string getKernelModules() {
    return exec("lsmod | awk '{print $1}'");
}

// Function to check for kernel module errors
std::string che
