//Bryant R. Hays
//01/19/2022
//Lab01 CS 470
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>
#include<stdlib.h>

using namespace std;

#define MAX_COMM_SIZE 100


//Static variables for the prompt.
static string Default_Prompt = "\ncwushell> ";
static string prompt = "";

//Function Declarations

void systExit(char **pString);

void execCpuInfo(char **pString);

void execMemInfo(char **pString);

void execPrompt(char **pString);

void executeCommand(char* command);

void displayManual();

void displayCpuInfoHelp();

void displayMemInfoHelp();

void executeCombinedMemInfo(char *string, char **parameters);

void executeCombinedCPUInfo(char *string, char **parameters);

void execMemTotal();

void execUsedMemory();

void execL2Size();

void execClockSpeed();

void execCPU();

void execNumCores();

void execLinuxCommand(char **pString);


//Function to tokenize the commands by spaces. The tokenized commands are then NULL terminated.
void tokenizeComm(char *command, char **parameters){

    char *token;

    token = strtok(command, " ");

    while (token != NULL){

        *parameters++ = token;
        token = strtok(NULL, " ");
    }

    *parameters = NULL;

}

//Function to execute the given command.
void execComm(char **parameters, char *command){

    //If there are no parameters, return and print the prompt again.
    if (parameters[0] == NULL){
        return;
    }

    //Exit the shell.
    if (strcmp(parameters[0], "exit") == 0){

        systExit(parameters);
    }

    //Display the manual.
    else if (strcmp(parameters[0], "manual") == 0){
        displayManual();
    }

    //Display processor information.
    else if(strcmp(parameters[0], "cpuinfo") == 0){
        execCpuInfo(parameters);
    }

    //Display memory information.
    else if(strcmp(parameters[0], "meminfo") == 0){
        execMemInfo(parameters);
    }

    //Change the prompt.
    else if(strcmp(parameters[0], "prompt") == 0){
        execPrompt(parameters);

        //See if the command is a built-in Linux command and execute.
    } else{
        execLinuxCommand(parameters);
    }

}

//Function to execute built-in Linux command by forking the process into multiple concurrent processes.
void execLinuxCommand(char **params) {

    pid_t pid;
    int status = 0;

    //Anything after this will be forked into two separate running processes.
    pid = fork();
    if (pid == 0) { //Wew are in the child process.
        execvp(params[0], params); //Executes the given linux command where params[0] is the file name and params are the arguments.

        cout << "\n" << params[0] << ": command not found"; //This line is only reached if the forking process fails (command does not exist).

    } else { //We are in the parent process.
            wait(&status);
        }

     cout << "\n";
    }


//Function to change the prompt to the given [new_prompt]. Typing prompt will then restore the current prompt to the default prompt.
void execPrompt(char **parameters) {

    //Display help info.
    if(parameters[1] == "-h" || parameters[1] == "--help"){
        printf("Usage: prompt [new_prompt]\n");
        printf("will change the current shell prompt to the new_prompt. Typing prompt will restore the default shell prompt.\n");

    }

    //Just typing prompt will restore the current prompt to the default prompt.
    else if(parameters[1] == NULL){

        prompt = Default_Prompt;

    }

    //If the prompt is too long, return.
    else if (strlen(parameters[1]) > 20){

        cout << "\nPrompt length is too long!";
        return;
    }

    //Change the current prompt to the given prompt name.
    else {
        prompt = parameters[1];
    }


}

//Executes memory info commands that are separated by spaces.
void execMemInfo(char **parameters) {
    bool alreadyUsed[4];
    int i = 1;

   while (parameters[i] != NULL){

        //Display the corresponding help information.
       if (parameters[i] == 0
          || strcmp(parameters[i], "-h") == 0
          || strcmp(parameters[i], "--help") == 0) {

           if (alreadyUsed[0] == 1){
               i++;
               continue;
           }

           alreadyUsed[0] = 1;

           displayMemInfoHelp();
           i++;
       }

       //Handle combined shell commands.
       else if (strlen(parameters[i]) >= 3){

           executeCombinedMemInfo(parameters[i], parameters);
            i++;
       }

       //Execute -t command which will display the total available memory in terms of bytes.
       else if (strcmp(parameters[i], "-t") == 0){
           if (alreadyUsed[1] == 1){
               i++;
               continue;
           }

           alreadyUsed[1] = 1;


           execMemTotal();
           i++;
       }

       //Execute -u command which will display the total used memory in terms of bytes.
       else if (strcmp(parameters[i], "-u") == 0){
           if (alreadyUsed[2] == 1){
               i++;
               continue;
           }

           alreadyUsed[2] = 1;

           execUsedMemory();
           i++;
       }

       //Execute -c command which will display the size of L2 cache memory in terms of bytes.
       else if (strcmp(parameters[i], "-c") == 0){
           if (alreadyUsed[3] == 1){
               i++;
               continue;
           }

           alreadyUsed[3] = 1;


           execL2Size();
           i++;
       }
       //Invalid switch name. Displays the corresponding help page.
       else{
           cout << "\n" << parameters[0] << ": " << "invalid option -- \'" << parameters[i] << "\'\n";
           displayMemInfoHelp();
           return;
       }

   }

}


//Linux shell command to display the L2 cache size.
void execL2Size() {

    printf("CPU L2 cache size: ");
    fflush(stdout);
    executeCommand("cat /sys/devices/system/cpu/cpu0/cache/index2/size");

}

//Linux shell command to display the total used memory.
void execUsedMemory() {
    cout << ("Used memory: ");
    fflush(stdout);
    executeCommand("vmstat -s | grep 'used memory'");
}

//Linux shell command to display the total available memory.
void execMemTotal() {
    executeCommand("cat /proc/meminfo |grep MemTotal");

}

//Executes memory commands that are combined. Switch functions remain the same.
void executeCombinedMemInfo(char *comm, char **parameters) {

    bool alreadyUsed[4];

    for (int i = 1; i < strlen(comm); i++){

        if (comm[0] != '-'){

            cout << "\n" << comm << ": " << "invalid option -- \'" << comm << "\'\n";
            displayMemInfoHelp();
            return;
        }

        else if (comm[i] == 't'){
            if (alreadyUsed[0] == 1){
                continue;
            }

            execMemTotal();
            alreadyUsed[0] = 1;
        }

        else if (comm[i] == 'u'){
            if (alreadyUsed[1] == 1){
                continue;
            }

            execUsedMemory();
            alreadyUsed[1] = 1;

        }

        else if (comm[i] == 'c'){
            if (alreadyUsed[2] == 1){
                continue;
            }

            execL2Size();
            alreadyUsed[2] = 1;

        }

        else if (comm[i] == 'h'){
            if (alreadyUsed[3] == 1){
                continue;
            }
            displayMemInfoHelp();
            alreadyUsed[3] = 1;
        }

        else {
            cout << "\n" << parameters[0] << ": " << "invalid option -- \'" << comm << "\'\n";
            displayMemInfoHelp();
            return;
        }
    }

}


//Executes CPU info commands that are separated by spaces.
void execCpuInfo(char **parameters) {

    //If the and is already used, do not execute it again.
    bool alreadyUsed[4];

    int i = 1;

    //Execute each parameter until NULL.
    while (parameters[i] != NULL){

        //Display help page.
        if (parameters[i] == 0
            || strcmp(parameters[i], "-h") == 0
            || strcmp(parameters[i], "--help") == 0) {

            if (alreadyUsed[0] == 1){
                i++;
                continue;
            }
            displayCpuInfoHelp();
            alreadyUsed[0] = 1;
            i++;
        }

        //Handles combined shell commands.
        else if (strlen(parameters[i]) >= 3){
            executeCombinedCPUInfo(parameters[i], parameters);
            i++;
        }

        //Execute the -c command, which displays the clock speed in MHz.
        else if(strcmp(parameters[i], "-c") == 0){
            if (alreadyUsed[1] == 1){
                i++;
                continue;
            }
            execClockSpeed();
            alreadyUsed[1] = 1;
            i++;

        }

        //Execute the -t command, which displays the CPUs manufacturer and model.
        else if(strcmp(parameters[i], "-t") == 0){
            if (alreadyUsed[2] == 1){
                i++;
                continue;
            }
            execCPU();
            alreadyUsed[2] = 1;
            i++;

        }

        //Execute the -n command, which displays the number of CPU cores.
        else if(strcmp(parameters[i], "-n") == 0){
            if (alreadyUsed[3] == 1){
                i++;
                continue;
            }


            execNumCores();
            alreadyUsed[3] = 1;
            i++;

        }

        //Invalid switch name. Displays the corresponding help page.
        else {
            cout << "\n" << parameters[0] << ": " << "invalid option -- \'" << parameters[i] << "\'\n";
            displayCpuInfoHelp();
            return;
        }


    }

}

//Linux shell command to print the number of CPU cores.
void execNumCores() {
    printf("Number of CPU cores: ");
    fflush(stdout);
    executeCommand("cat /proc/cpuinfo | grep processor | wc -l");

}

//Linux shell command to print the host machine's processor manufacturer and model name.
void execCPU() {
    executeCommand("cat /proc/cpuinfo | grep 'vendor' | uniq");
    executeCommand("cat /proc/cpuinfo | grep 'model name' | uniq");

}

//Linux shell command to display the clock speed in MHz
void execClockSpeed() {
    executeCommand("lscpu | grep MHz");
}

//Executes CPU information commands that are combined. Switch functions remain the same.
void executeCombinedCPUInfo(char *comm, char **parameters) {

    bool alreadyUsed[4];

    for (int i = 1; i < strlen(comm); i++){
        if (comm[0] != '-'){
            cout << "\n" << comm[i] << ": " << "invalid option -- \'" << comm[i] << "\'\n";
            displayMemInfoHelp();
            return;
        }

        else if (comm[i] == 'c'){
            if (alreadyUsed[0] == 1){
                continue;
            }
            execClockSpeed();
            alreadyUsed[0] = 1;
        }

        else if (comm[i] == 't'){
            if (alreadyUsed[1] == 1){
                continue;
            }
            execCPU();
            alreadyUsed[1] = 1;

        }

        else if (comm[i] == 'n'){

            if (alreadyUsed[2] == 1){
                continue;
            }

            execNumCores();
            alreadyUsed[2] = 1;

        }

        else if (comm[i] == 'h'){
            if (alreadyUsed[3] == 1){
                continue;
            }

            displayCpuInfoHelp();
            alreadyUsed[3] = 1;
        }

        else {
            cout << "\n" << parameters[0] << ": " << "invalid option -- \'" << comm << "\'\n";
            displayCpuInfoHelp();
            return;
        }
    }

}


//Help pages.
void displayCpuInfoHelp() {
    printf("\nUsage: cpuinfo -h | -c | -t | -n\n");
    printf("Will print on the screen different cpu related information based on the switch.\n");
    printf("\n");
    printf("-h, --help \tprints this message.\n");
    printf("-c \tprints the CPU's clock speed.\n");
    printf("-t \tprints the CPU manufacturer.\n");
    printf("-n \tprints the number of cores.\n");

}

void displayMemInfoHelp() {
    printf("\nUsage: meminfo -h | -t | -u | -c\n");
    printf("Will print on the screen different memory related information based on the switch\n");
    printf("\n");
    printf("-h, --help \tprints this message.\n");
    printf("-t \tprints the total RAM memory available in the system in bytes.\n");
    printf("-u \tprints the used RAM memory in bytes.\n");
    printf("-n \tprints the size of the L2 cache and core in bytes.\n");

}

//Manual
void displayManual() {

    cout << ("\nThe cwushell has the following built-in commands: ");
    cout << ("\n");
    cout << ("\nmanual ");
    cout << ("\nprints this file as a reference for all the available commands. \n");
    cout << ("\nexit [code]");
    cout << ("\nExits the cwushell with an optional exit code. \n");
    cout << ("\nprompt [new_prompt]");
    cout << ("\nwill change the current shell prompt to the new_prompt. Typing prompt will restore the default shell prompt.\n");
    cout << "\nmeminfo -switch";
    displayMemInfoHelp();
    cout << "\ncpuinfo -switch";
    displayCpuInfoHelp();
    cout << ("\ncwushell can also run normal Linux shell commands, which are externally executed using 'sh -c'.");

}

//Function to fork and execute the custom commands.
void executeCommand(char* command) {
    // Prepare args array for sh
    char* cmd_args[4];

    cmd_args[0] = "sh";
    cmd_args[1] = "-c";
    cmd_args[2] = command;
    cmd_args[3] = NULL; // Args are Null terminated

    int status;
    int return_code = EXIT_FAILURE;

    // Spawn child process
    pid_t pid = fork();

    if (pid < 0)
    {
        printf("Error: Unable to fork process.\n");
    }

    //At this point, we are in the child process.
    else if (pid==0)
    {
        execvp("sh", cmd_args);
        exit(127); // Command not found return code. This line is not reached on success.
    }

    // Wait for child process to exit
    if (waitpid(pid, &status, 0) == -1)
    {
        printf("Error: Waiting for child process failed.\n");
    }

    // If child process exited normally, get exit code
    if (WIFEXITED(status)) {
        return_code = WEXITSTATUS(status);
    }

}

//Handle exit command.
void systExit (char **parameters) {
    int retValue = 0;
    if (parameters[1] == NULL){ //If no exit code is given, exit normally.
        exit(0);
    }else if(isdigit(*parameters[1]) == false) { //If code is non-integer value, display warning and exit.
        printf("\nError: exit code must be an integer value!");
        return;
    } else {
        retValue = atoi(parameters[1]); //Exit the prompt with the given exit code.
        exit(retValue);

    }

}


//Driver code.
int main() {

    prompt = Default_Prompt;

    //Array for given command.
    char command[MAX_COMM_SIZE];

    //Array for parsed command.
    char *parameters[MAX_COMM_SIZE];


    do {
        cout << prompt;
        cin.getline(command, MAX_COMM_SIZE); //Populate the command array from keyboard input.
        tokenizeComm(command, parameters); //Tokenize the commands into the parameters array.
        execComm(parameters, command); //Execute the commands.

    } while (strcmp(command, "exit") != 0); //Execute commands until user explicitly types 'exit' in the prompt.

    return 0;
}

