//
//  main.cpp
//  TuttleExamQ1
//
//  Created by Anastasia Tuttle  on 2/21/24.
//

#include <iostream>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

char ** parseCommandLine(const char* command, size_t& argc);

void executeCommand(const char* command)
{
    size_t argc;
    char** argv = parseCommandLine(command, argc);
    
    pid_t pid = fork();
    
    if (pid == -1)
    {
        cout<<"Fork failed"<<endl;
        //handle error if fork fails
    }
    else if (pid == 0)
    {
        //child process
        execvp(argv[0], argv);
        
        cout<<"Exec failed"<<endl;
        exit(EXIT_FAILURE);
    }
    else
    {
        int status;
        waitpid(pid, &status, 0);
        
        //check exit status and handle it
        if (WIFEXITED(status))
        {
            cout<<"Child process exited with status: "<<WEXITSTATUS(status)<<endl;
        }
        else
        {
            cout<<"Child process terminated abnormally"<<endl;
        }
    }
    
    //clean up allocated memory
    for(size_t i = 0; i < argc; ++i)
    {
        delete[] argv[i];
    }
    delete[] argv;
}

int main()
{
    //example used in the question
    executeCommand("ls -l");
    
    return 0;
}

char** parseCommandLine(const char* command, size_t& argc)
{
    //simple parsing
    const char* delimiters = " \t\n";
    char* commandCopy = strdup(command); //duplicate the command to modify it
    char* token = strtok(commandCopy, delimiters);
    
    //cound the number of arguments
    argc = 0;
    while (token != nullptr)
    {
        ++argc;
        token = strtok(nullptr, delimiters);
    }
    
    //allocate memory for argv
    char** argv = new char*[argc + 1]; //+1 for the nullptr at the end
    
    //populate argv with command tokens
    argc = 0;
    token = strtok(commandCopy, delimiters);
    while (token != nullptr)
    {
        argv[argc] = strdup(token);
        ++argc;
        token = strtok(nullptr, delimiters);
    }
    
    //set the last element of argv to nullprt
    argv[argc] = nullptr;
    
    //free the duplicated CommandCopy
    free(commandCopy);
    
    return argv;
}
