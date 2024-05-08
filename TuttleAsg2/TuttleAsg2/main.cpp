//
//  main.cpp
//  TuttleAsg2
//
//  Created by Anastasia Tuttle  on 1/30/24.
//

#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

using namespace std;

//struct to hold commad-line arguments
struct arguments
{
    int argc;
    vector<char*> argv;
};

//Function to parse a command-line string into an arguments
arguments parse_command(const string& command)
{
    istringstream iss(command);
    vector<char*> args;
    
    //tokenize the command string and store in args
    string token;
    while (iss >> token)
    {
        args.push_back(strdup(token.c_str()));
    }
    
    //add a nullptr at the end for execvp
    args.push_back(nullptr);
    
    //create and populate the arguments structure
    return {static_cast<int>(args.size() - 1), move(args)};
}

//function to execute a command
void execute_command(const arguments& args, const string& input_file, const string& output_file)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        cerr<<"Error forking process."<<endl;
        return;
    }
    else if (pid == 0) //child process
    {
        //redirect input if specified
        if (!input_file.empty())
        {
            int input_fd = open(input_file.c_str(), O_RDONLY);
            if (input_fd == -1)
            {
                cerr <<"Error opening input file: "<<input_file<<endl;
                exit(EXIT_FAILURE);
            }
            
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }
        
        //redirect output if specified
        if (!output_file.empty())
        {
            int output_fd = open(output_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (output_fd == -1)
            {
                cerr<<"Error opening output file: "<<output_file <<endl;
                exit (EXIT_FAILURE);
            }
            
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }
        
        //execute the command
        execvp(args.argv[0], args.argv.data());
        
        //if execvp fails, print an error message
        cerr<<"Error executing command: "<<strerror(errno) <<endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
    }
};

//function to change the current working directory
void change_directory(const string& path)
{
    if (chdir(path.c_str()) != 0)
    {
        cerr<<"Error changing directory to: "<<path<<endl;
    }
}

//function to print the current working directory
void print_working_directory()
{
    char* cwd = getcwd(nullptr, 0);
    cout<<"Current working directory: "<<cwd<<endl;
    free(cwd);
}

int main ()
{
    while (true)
    {
        try {
            //get user input
            string user_input;
            cout<<"$ ";
            getline(cin, user_input);
            
            //check for exit command
            if (user_input == "exit")
            {
                break;
            }
            
            //check for cd and pwd commands
            if (user_input.substr(0, 2) == "cwd")
            {
                change_directory(user_input.substr(3));
                continue; //skip the rest of the loop
            }
            else if (user_input == "pwd")
            {
                print_working_directory();
                continue; //skip the rest of the loop
            }
            
            //sppit the input into command and arguments
            arguments args = parse_command (user_input);
            
            //find input and output redirection symbols
            string input_redirect;
            string output_redirect;
            
            for (int i = 0; i < args.argc; ++i)
            {
                if (strcmp (args.argv[i], "<") == 0 && i + 1 < args.argc)
                {
                    input_redirect = args.argv[i + 1];
                    args.argv[i] = nullptr;
                    args.argv[i + 1] = nullptr;
                    i += 1;
                }
                else if (strcmp(args.argv[i], ">") == 0 && i + 1 < args.argc)
                {
                    output_redirect = args.argv[i + 1];
                    args.argv[i] = nullptr;
                    args.argv[i + 1] = nullptr;
                    i += 1;
                }
            }
            
            //execute the command
            execute_command(args, input_redirect, output_redirect);
            
            //clean up allocated memory for args
            for(char* arg : args.argv)
            {
                free(arg);
            }
        }
        catch (...)
        {
            cerr<<"An error occurred."<<endl;
        }
    }
    return 0;
};
