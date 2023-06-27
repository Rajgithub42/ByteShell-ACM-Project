#include<string.h>
#include<stdio.h>
#include<iostream>
#include<unistd.h>
#include <vector>
#include <sys/wait.h>
#include <map>

using namespace std;


vector<string> builtins = {"cd", "pwd", "exit"};


void read_line() // read a line
{
    int bufsz= 1024;
    int pos=0;
    char *buffer= malloc(sizeof(char)*bufsz);
    char c;

    if(!buffer)
    {
        fprintf(stderr, "ACMShell: allocation error\n");
        exit(EXIT_FAILURE);
    }
    while(true)
    {
        c = getchar();
        if(c=='\n')
        {
            buffer[pos]='\0';
            return buffer;
        }
        else 
        {
            buffer[pos]=c;
            pos++;
        }
        if(pos>=bufsz)
        {
            bufsz+=64;
            buffer= realloc(buffer, bufsz);
        }
        if(!buffer)
        {
            fprintf(stderr,"ACMShell: allocation error\n");
            exit(EXIT_FAILURE);
        }
    }
}


int cmds_line_args() //use of strtok in cmd_line_arg function
{
    char *cmd=NULL, *cmd_cpy = NULL, *token=NULL;
    char *delim= "ACMShell_TOK_DELIM";
    size_t n= 0;
    int argc=0 , i=0;
    char **argv= NULL;
    printf("$ ");
    if(getline(&cmd, &n, stdin)==-1)
    return -1;

    cmd_cpy= strdup(cmd);

    token= strtok(cmd, delim);
    

    while(token)
    {
        token =strtok(NULL, delim);
        argc++;
    }
    printf("%d\n", argc);

    argv = malloc(sizeof(char *)*argc);

    token= strtok(cmd_cpy, delim);

    while(token)
    {
        argv[i]=token;
        token= strtok(NULL, delim);
        i++;
    }

    argc[i]=NULL;
    
    i=0;
    while(argv[i])
    {
        printf("%s\n", argv[i++]);
    }
    free(cmd), free(cmd_cpy), free(argv);
    return 0;
}

void changeDirectory(const vector<string>& args)
{
    if (args.size() < 2) {
        cerr << "Usage: cd <directory>" << std::endl;
        return;
    }

    const char* directory = args[1].c_str();
    if (chdir(directory) == 0) {
        char cwd[256];
        if (getcwd(cwd, sizeof(cwd)) != nullptr) {
            std::cout << "Successfully changed directory to: " << cwd << endl;
        }
    } 
    else 
    {
        cerr << "Error changing directory" << endl;
    }
}

// Function to print the current working directory
void printWorkingDirectory()
{
    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) != nullptr)
    {
        cout << "Current working directory: " << cwd << endl;
    }
    else
    {
        cerr << "Error getting current working directory" << endl;
    }
}

// Function to exit the shell
void exitShell()
{
    cout << "Exiting ACMShell..." << endl;
    exit(0);
}


// Node structure for linked list
struct Node 
{
    string command;
    Node* next;

    Node(const string& cmd) : command(cmd), next(nullptr) {}
};

// Class for history functionality
class History
{
private:
    Node* head;  // Head of the linked list

public:
    History() : head(nullptr) {}

    // Add a command to the history
    void addCommand(const string& cmd) {
        Node* newNode = new Node(cmd);

        if (head == nullptr) {
            head = newNode;
        } else {
            Node* curr = head;
            while (curr->next != nullptr) {
                curr = curr->next;
            }
            curr->next = newNode;
        }
    }

    // Display the command history
    void displayHistory() {
        Node* curr = head;
        int count = 1;

        cout << "ACMShell Command History:" << endl;
        while (curr != nullptr) {
            cout << count << ". " << curr->command << endl;
            curr = curr->next;
            count++;
        }
    }
}


void ByteShell_launch(vector<string>& args) {
    // Convert vector of strings to array of C-style strings
    vector<char*> argv;
    for (const auto& arg : args) {
        argv.push_back(const_cast<char*>(arg.c_str()));
    }
    argv.push_back(nullptr);  // Add a null pointer at the end

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(argv[0], argv.data()) == -1) {
            cerr << "Error executing command" << endl;
            exit(1);
        }
    } else if (pid > 0) {
        // Parent process
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            int exitStatus = WEXITSTATUS(status);
            if (exitStatus != 0) {
                std::cerr << "Command exited with status " << exitStatus << std::endl;
            }
        } else if (WIFSIGNALED(status)) {
            int signal = WTERMSIG(status);
            cerr << "Command terminated with signal " << signal << endl;
        }
    } else {
        // Fork error
        cerr << "Error forking process" << endl;
    }
}

int ByteShell_execute(vector<string>& arg)
{
    if(args[0]==NULL)
    {
        return 1; // An empty command was entered
    }
    for(int i=0; i < (int) builtins.size(); i++)
    {
        if(strcmp(args[0], builtins[i])==0)
        return (*builtin_func[i])(args);
    }
    return ByteShell_launch(args);
}



int mapfile(int argc, char* argv[])
{
    // Check if the filename is provided as an argument
    if (argc != 2)
    {
        cout << "Usage: mapfile <filename>" << std::endl;
        return 1;
    }

    // Open the file for reading
    ifstream file(argv[1]);
    if (!file)
    {
        cout << "Failed to open the file." << endl;
        return 1;
    }

    // Create a map to store the key-value pairs
    map<string, string> keyValueMap;

    // Read the file line by line and populate the map
    string line;
    while (getline(file, line))
    {
        // Find the position of the equal sign '='
        size_t pos = line.find('=');
        if (pos != string::npos) {
            // Extract the key and value from the line
            string key = line.substr(0, pos);
            string value = line.substr(pos + 1);

            // Insert the key-value pair into the map
            keyValueMap[key] = value;
        }
    }

    // Close the file
    file.close();

    // Print the key-value pairs
    for (const auto& pair : keyValueMap)
    {
        cout << pair.first << " = " << pair.second << endl;
    }

    return 0;
}


void ACMHelp()
 {
    cout << "Shell Help" << endl;
    cout << "Command\t\tDescription" << endl;

    map<string, string> commandMap;
    commandMap["ls"] = "List files and directories";
    commandMap["cd"] = "Change the current directory";
    commandMap["pwd"] = "Print the current working directory";
    commandMap["help"] = "Display help information";

    for (const auto& command : commandMap) {
        cout << command.first << "\t\t" << command.second << endl;
    }
}

void ACMlogout() {
    cout << "Logging out..." << endl;
    exit(0);
}


int main()
{
    History commandHistory;
    vector<string> args = {"ls", "-l"};
    
    while (true) {
        string command;
        cout << "$ ";
        getline(std::cin, command);

        if (command == "help") {
            ACMHelp();
        }

        if (command == "logout") {
            ACMlogout();
        }

        // Tokenize the command
        vector<string> tokens;
        string delimiter = " ";
        size_t pos = 0;
        while ((pos = command.find(delimiter)) != std::string::npos) {
            string token = command.substr(0, pos);
            tokens.push_back(token);
            command.erase(0, pos + delimiter.length());
        }
        tokens.push_back(command);

        // Check if the command is a built-in function
        if (!tokens.empty()) {
            if (tokens[0] == "cd") {
                changeDirectory(tokens);
            } else if (tokens[0] == "pwd") {
                printWorkingDirectory();
            } else if (tokens[0] == "exit") {
                exitShell();
            } else {
                cerr << "Command not found: " << tokens[0] << endl;
            }
        }

         // Add the command to history
        commandHistory.addCommand(command);

        // Check if the command is "history"
        if (command == "Shw=ow ACMShell history") {
            commandHistory.displayHistory();
    }
    }
    launch(args);
    return 0;
}



