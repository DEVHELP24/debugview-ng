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
std::string checkModuleErrors() {
    std::string result = "Checking for errors related to kernel modules:\n";
    
    result += "Checking dmesg for module errors or warnings:\n";
    result += exec("dmesg | grep -iE '(modprobe|insmod|rmmod|module.*(failed|not found|error|denied|unknown symbol|unresolved symbol|init failed))'");
    result += "\n";

    if (system("command -v journalctl > /dev/null") == 0) {
        result += "Checking journalctl for module errors or warnings:\n";
        result += exec("journalctl -k | grep -iE '(modprobe|insmod|rmmod|module.*(failed|not found|error|denied|unknown symbol|unresolved symbol|init failed))'");
        result += "\n";
    } else {
        result += "journalctl is not available.\n";
    }

    if (std::ifstream("/var/log/kern.log")) {
        result += "Checking /var/log/kern.log for module errors or warnings:\n";
        result += exec("grep -iE '(modprobe|insmod|rmmod|module.*(failed|not found|error|denied|unknown symbol|unresolved symbol|init failed))' /var/log/kern.log");
        result += "\n";
    } else {
        result += "/var/log/kern.log is not available.\n";
    }

    if (std::ifstream("/var/log/syslog")) {
        result += "Checking /var/log/syslog for module errors or warnings:\n";
        result += exec("grep -iE '(modprobe|insmod|rmmod|module.*(failed|not found|error|denied|unknown symbol|unresolved symbol|init failed))' /var/log/syslog");
        result += "\n";
    } else {
        result += "/var/log/syslog is not available.\n";
    }

    return result;
}

// Function to save results to output directory
void saveResults(const std::string& outputDir, const std::string& outputFile, const std::string& content) {
    std::ofstream outFile(outputDir + "/" + outputFile);
    if (outFile) {
        outFile << content;
        outFile.close();
        chmod((outputDir + "/" + outputFile).c_str(), 0666);
    } else {
        std::cerr << "Failed to save results to " << outputDir + "/" + outputFile << std::endl;
    }
}

// Function to generate output filename based on selected options
std::string generateOutputFilename(bool listModules, bool checkErrors, bool checkSystemdErrors, bool checkXorgDebug) {
    std::string filename = "ng_global_debug_results_" + std::to_string(std::time(nullptr));
    if (listModules) filename += "_modules";
    if (checkErrors) filename += "_errors";
    if (checkSystemdErrors) filename += "_systemd_errors";
    if (checkXorgDebug) filename += "_xorg_debug";
    return filename + ".txt";
}

// Main function
int main(int argc, char* argv[]) {
    bool listModules = false, checkErrors = false, checkSystemdErrors = false, checkXorgDebug = false;
    std::string outputDirectory;

    // Parse command-line options
    int opt;
    while ((opt = getopt(argc, argv, "hmesxo:")) != -1) {
 
