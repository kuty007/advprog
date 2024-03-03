#ifndef MYSHELL_HPP
#define MYSHELL_HPP

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>
#include <csignal>
#include <cstdlib>
#include <sys/ioctl.h>
#include <iomanip>
#include <map>
#include <algorithm> 
#include <iterator>
#include <fcntl.h>
#include <sys/stat.h> 
#include <termios.h>


class myShell {
private:
    static std::string prompt; // the propmt before each command
    int lastStatus; // it stores the status num of last command
    std::string lastCommand; // it stores the last command
    static volatile sig_atomic_t childPid; // it stores the child process ID
    static void sigintHandler(int); // for handling ctrl+C
    std::map<std::string, std::string> vars; // dict that stores each $ var in key-value pattern 

public:
    myShell(); // ctor
    ~myShell() = default; // dtor

    static void setPrompt(const std::string&); // it changes the name of the prompt
    void run(); // it runs the shell
    static volatile sig_atomic_t& getChildPid(); // Static member function to get childPid
    void executeCommand(const std::string&, bool, bool);
    void executePipes(std::vector<std::string>, bool);
    int ifCaseHandeling(std::string);
};

#endif // MYSHELL_HPP