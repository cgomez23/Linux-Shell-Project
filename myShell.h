#include <iostream>
#include <string>       
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>

using namespace std;

class myShell {
private:

public: 
    // History
    //string history[];

    string greeting = "CLI > ";
    string command;

    // The run loop
    void execute();

    // Helper Functions
    int parse_and_execute(string c);

    void WaitFor(int pid);
    string find_dir_entry (string output);
    string pwd();
    void add_to_history(string command);
    char * get_dir_name(struct dirent *entp, DIR *pdir, __ino_t cid);
    string get_history();
    void reset_cout(std::streambuf *coutbuf);
};