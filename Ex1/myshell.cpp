#include "myshell.hpp"

volatile sig_atomic_t myShell::childPid = 0;
std::string myShell::prompt = "hello:";

myShell::myShell() : lastCommand(""), lastStatus(0) 
{
    signal(SIGINT, sigintHandler); // init signal handler for Ctrl+C
}

void myShell::setPrompt(const std::string& newPrompt) 
{
    prompt = newPrompt;
}

void myShell::sigintHandler(int /*signum*/) 
{
    if (childPid != 0) { // if there is a child process
        std::cout << "\r"; // clear the line, to avoid C^ print
        kill(childPid, SIGINT); // terminate child's proccess
        waitpid(childPid, nullptr, 0); // wait for child to finish
        childPid = 0;
    } else { // at parent process
        std::cout << "\rYou typed Control-C!\n";
        std::cout << prompt << ": ";
        std::cout.flush();
    }
}

void myShell::executeCommand(const std::string& input, bool isBackground, bool disablePrint) 
{
    if (input.empty()) 
    {
        return;
    }
    bool redirect = false;
    bool append_redirect = false;
    std::string outfile;
    int fd;
    if (input.find('>') != std::string::npos || input.find(">>") != std::string::npos ||
     input.find("2>") != std::string::npos) 
    {
        std::vector<std::string> tokens;
        std::istringstream iss(input);
        std::copy(std::istream_iterator<std::string>(iss),
                    std::istream_iterator<std::string>(),
                    std::back_inserter(tokens));
        size_t i = tokens.size();
        if (i >= 2 && (tokens[i - 2] == ">" || tokens[i - 2] == ">>")) 
        {
            redirect = true;
            if (tokens[i - 2] == ">>") 
            {
                append_redirect = true;
            }
            outfile = tokens[i - 1];
            tokens[i - 2] = "";
            tokens[i - 1] = "";
        } 
        else if (i >= 2 && tokens[i - 2] == "2>") 
        {
            redirect = true;
            append_redirect = false;
            outfile = tokens[i - 1];
            tokens[i - 2] = "";
            tokens[i - 1] = "";
            fd = creat(outfile.c_str(), 0660);
            close(STDERR_FILENO);
            dup(fd);
            close(fd);
        } 
        else 
        {
            redirect = append_redirect = false;
        }
    }
    pid_t pid = fork(); // fork to create a child process
    if (pid == -1) 
    {
        std::cerr << "Error forking process." << std::endl;
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) // child process
    {
        if (redirect) 
        {
            if (append_redirect) 
            {
                fd = open(outfile.c_str(), O_WRONLY | O_CREAT | O_APPEND | O_RDONLY, 0644); // append mode
            } 
            else 
            {
                fd = open(outfile.c_str(), O_CREAT | O_TRUNC | O_WRONLY | O_RDONLY, 0644); // write mode
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }
        if (disablePrint) // for "if" cases which exec used only for exit status
        {
            int devNull = open("/dev/null", O_WRONLY);
            dup2(devNull, STDOUT_FILENO);
            close(devNull);
        }
        this->lastStatus = execlp("sh", "sh", "-c", input.c_str(), nullptr); // execute the command in the existing shell
        std::cerr << "Error executing command." << std::endl;
        exit(EXIT_FAILURE);
    } 
    else // parent process
    {
        childPid = pid;
        int status;
        if (!isBackground) {
            waitpid(pid, &status, 0); // wait for the child process to finish if not running in the background
            this->lastCommand = input; // store the last command
            this->lastStatus = WEXITSTATUS(status); // store the exit status of the last command
            // std::cout << this->lastStatus << std::endl;
            childPid = 0; // reset childPid after child process has finished
        }
    }
}

void myShell::executePipes(std::vector<std::string> commands, bool disablePrint)
{
    // Create a pipe for each command except the last one
    std::vector<int> pipes((commands.size() - 1) * 2);
    for (size_t i = 0; i < commands.size() - 1; ++i) 
    {
        if (pipe(pipes.data() + i * 2) < 0) 
        {
            std::cerr << "Error creating pipe." << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    // Execute each command
    for (size_t i = 0; i < commands.size(); ++i) 
    {
        // std::cout << "Executing command: '" << commands[i] << "'" << std::endl;
        pid_t pid = fork();
        if (pid == -1) 
        {
            std::cerr << "Error forking process." << std::endl;
            exit(EXIT_FAILURE);
        } 
        else if (pid == 0) // child process
        {
            // If not the first command, redirect input from the previous pipe
            if (i != 0) 
            {
                if (dup2(pipes[(i - 1) * 2], 0) < 0) 
                {
                    std::cerr << "Error redirecting input." << std::endl;
                    exit(EXIT_FAILURE);
                }
            }

            // If not the last command, redirect output to the next pipe
            if (i != commands.size() - 1) 
            {
                if (dup2(pipes[i * 2 + 1], 1) < 0) 
                {
                    std::cerr << "Error redirecting output." << std::endl;
                    exit(EXIT_FAILURE);
                }
            }

            // Close all pipes in the child process
            for (int& pipe : pipes) 
            {
                close(pipe);
            }
            int status;
            // Execute the command
            executeCommand(commands[i], false, disablePrint);
            
            // std::cout << "Command '" << commands[i] << "' execution successful." << std::endl;
            exit(this->lastStatus); // exit with the exit code of last command in pipeline
        }
    }

    // close all pipes in the parent process
    for (int& pipe : pipes) 
    {
        close(pipe);
    }
    // wait for all child processes to finish
    for (size_t i = 0; i < commands.size(); ++i) 
    {
        wait(nullptr);
    }
}

int myShell::ifCaseHandeling(std::string ifCommand)
{
    std::string thenSymbol, thenCommand, elseSymbol, elseCommand, fiSymbol;
    std::cout << "> ";
    std::getline(std::cin, thenSymbol);
    if (thenSymbol != "then")
    {
        std::cerr << "Syntax error near unexpected token `" << thenSymbol << "' " << std::endl;
        lastStatus = 1;
        return lastStatus;
    }
    std::cout << "> ";
    std::getline(std::cin, thenCommand);
    std::cout << "> ";
    std::getline(std::cin, elseSymbol);
    if (elseSymbol != "else")
    {
        std::cerr << "Syntax error near unexpected token `" << elseSymbol << "' " << std::endl;
        lastStatus = 1;
        return lastStatus;
    }
    std::cout << "> ";
    std::getline(std::cin, elseCommand);
    std::cout << "> ";
    std::getline(std::cin, fiSymbol);
    if (fiSymbol != "fi")
    {
        std::cerr << "Syntax error near unexpected token `" << fiSymbol << "' " << std::endl;
        lastStatus = 1;
        return lastStatus;
    }
    std::vector<std::string> commands;
    std::string command;
    std::istringstream commandStream(ifCommand);
    while (std::getline(commandStream, command, '|')) 
    {
        commands.push_back(command);
    }
    if (commands.size() > 1)
    {
        executePipes(commands, true);
    }
    else
    {
        executeCommand(ifCommand, false, true);
    }
    if (lastStatus == 0)
    {
        lastCommand = thenCommand;
    }
    else
    {
        lastCommand = elseCommand;
    }
    return 0;
}

void myShell::run() 
{
    while (true) {
        std::cout << prompt << " "; // prompt printer
        std::cout.flush();
        std::string input; // input reader
        std::getline(std::cin, input);
        if (input.substr(0, 2) == "if")
        {
            if (input.size() < 3)
            {
                input = ' ' + input;
            }
            if (ifCaseHandeling(input.substr(3)) != 0)
            {
                continue;
            }
            input = "!!";
        }
        if (input == "quit") // exit if the user types "quit"
        {
            break;
        }
        if (input == "!!") // return last command if the user types "!!"
        {
            input = lastCommand;
        }
        std::vector<std::string> tokens; // vector of tokens/strings/commands
        std::stringstream ss(input);
        std::string token;
        while (ss >> token) // tokenize the input into words
        {
            tokens.push_back(token);
        }
        if (tokens.size() >= 2 && tokens[0] == "cd") // "cd" command case
        {
            if (chdir(tokens[1].c_str()) == -1) // chdir changes the working directory
            {
                std::cerr << "Error changing directory." << std::endl;
            }
            continue; // return without forking and executing
        }
        if (tokens.size() >= 3 && tokens[0] == "prompt" && tokens[1] == "=") // check if the user wants to change the prompt
        {
            setPrompt(tokens[2]); // update the new prompt value
            continue; // continue to next iter without forking and executing
        }
        if (tokens.size() >= 3 && tokens[0][0] == '$' && tokens[1] == "=") // check if the user wants to add a new variable
        {
            std::string value = tokens[2];
            for (size_t i = 3; i < tokens.size(); ++i) {
                value += " " + tokens[i];
            }
            vars[tokens[0]] = value; // add name & value to vars dict
            continue; // continue to next iter without forking and executing
        }
        if (tokens.size() >= 2 && tokens[0] == "echo") // handle the "echo" command 
        {
            if (tokens.size() == 2 && tokens[1] == "$?")  // check for "echo $?" case to print the last command's status
            {
                std::cout << this->lastStatus << std::endl;
                continue; // continue to next iter without forking and executing
            }
            if (tokens.size() == 2 && tokens[1][0] == '$') // check if the user wants to echo a variable
            {
                auto it = vars.find(tokens[1]); // find the key in the vars dict
                if (it != vars.end()) // if the key exists in the dict
                {
                    std::cout << it->second << std::endl; // print key's value
                    continue; // continue to next iter without forking and executing
                }
            }
        }
        if (tokens.size() == 2 && tokens[0] == "read")
        {
            std::string readArg;
            std::cout << "> ";
            std::getline(std::cin, readArg);
            // std::cout << tokens[1] << " : " << readArg << std::endl;
            this->vars['$'+tokens[1]] = readArg; // add name & value to vars dict
            continue; // continue to next iter without forking and executing
        }
        std::vector<std::string> commands;
        std::string command;
        std::istringstream commandStream(input);
        while (std::getline(commandStream, command, '|')) 
        {
            commands.push_back(command);
        }
        
        if (commands.size() > 1)
        {
            executePipes(commands, false);
        }
        else
        {
            executeCommand(input, (input.back() == '&') ? true : false, false);
        }
    }
}

int main() {
    signal(SIGINT, SIG_IGN); // ignore SIGINT in the main function
    myShell myshell;
    myshell.run();
    return 0;
}