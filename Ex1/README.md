# MyShell

@ Haim Goldfisher & Asaf Yekutiel

MyShell is a simple shell implementation written in C++, providing basic command-line interface functionality. It supports various features commonly found in Unix-like shells, such as command execution, input/output redirection, background execution, and basic control flow constructs like if statements.

## Features

- **Command Execution:** Execute single commands or command pipelines with support for input/output redirection.
- **Built-in Commands:** Includes built-in commands such as `cd`, `echo`, and `read`.
- **Variable Handling:** Change the prompt and define user-defined variables.
- **Signal Handling:** Gracefully handle Ctrl+C interruptions.
- **Control Flow:** Implement conditional execution using `if` statements.
- **Background Execution:** Run commands in the background using the `&` operator.
- **Error Handling:** Provides basic error handling for syntax errors in `if` statements.


## Getting Started

### Prerequisites

- C++ compiler with C++2a support (e.g., Clang++-14).

### Compilation

Compile the code using a C++ compiler. For example, using g++:

```bash
- make 
```

## Running the Shell
Run the compiled executable:
```bash
- ./myshell
```

## Commands Guide

- **`>`**: Redirects standard output (stdout) to the specified file. For example, `command > output.txt` will redirect the standard output to the file `output.txt`.

- **`2>`**: Redirects standard error (stderr) to the specified file. For example, `command 2> error.txt` will redirect any error output to the file `error.txt`.

- **`>>`**: Appends output to a file and creates one if it doesn't exist. For instance, `echo "text" >> output.txt` will append the text to `output.txt`.

- **`prompt`**: Changes the name of the prompt, providing a customizable interface. Usage: `prompt = "new_prompt"`.

- **`echo`**: Prints the following argument in the terminal. For example, `echo Hello, World!` will display "Hello, World!" in the terminal.

- **`$?`**: Returns the exit status of the previous command. For example `echo $?` It allows you to check whether the last command was successful or not.

- **`!!`**: Reruns the previous command. Typing `!!` in the prompt will execute the same command as before.

- **`cd`**: Navigates through the file system. Use `cd /path/to/directory` to change the current working directory.

- **`quit`**: Exits the shell. Typing `quit` will terminate the prompt.

- **`^c`**: Stops a process midway. Pressing Ctrl+C in the terminal interrupts the currently running process.

- **`pipe`**: Pipes commands together to pass the output of one command as input to another. For example, `command1 | command2` chains the output of `command1` to `command2`.

- **`$var`**: Saves and utilizes variables in the shell. For instance, `$name = "John"` assigns the value "John" to the variable `$name`.

- **`read`**: Reads and stores user input into saved variables. Use `read variable_name` to prompt the user for input and store it in `variable_name`.

- **`echo $var`**: Prints the following variable in the terminal. For example, `echo $name!` will display "John" in the terminal.

- **`if/else`**: Creates conditional flows in the terminal. Use the `if` statement to conditionally execute commands based on a specified condition. For example:

  ```bash
  if [ condition ]
  then
      [ command to execute if condition is true ]
  else
      [ command to execute if condition is false ]
  fi
  ```




