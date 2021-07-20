#include <getopt.h>
#include <stdio.h>
#include <sys/types.h>    
#include <sys/stat.h>
#include <dirent.h>
#include "myShell.h"
#include <cstring>
#include <sys/wait.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <fstream>
#include <cstdio>

using namespace std;

int main(int argc, char* argv[]){
    myShell ms;
    ms.execute();
}

void myShell::execute(){
    while(true){
        cout << greeting;
        string command;
        getline(cin, command);
        add_to_history(command);
        parse_and_execute(command);
    }
}
/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*
* WaitFor(pid)                                                  *
* Waits for foreground processes...based on Rochkind's waitfor  *
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
void myShell::WaitFor(int pid){
  int gotpid, status;

  while( (gotpid = wait(&status)) != pid)
    cout << "Process [" << gotpid << "] Finished..." << endl;
}

int myShell::parse_and_execute(string c){
    char * cstr = new char [c.length()+1];
    std::strcpy (cstr, c.c_str());

    // cstr now contains a c-string copy of str
    string input = ""; //use this for input for piping or redirection
    string output = ""; //output from either ls or pwd
    char * p = strtok (cstr," ");
    std::streambuf *coutbuf = std::cout.rdbuf();; //save old buf
    bool redirect = false;
    bool input_redirect = false;
    while (p != NULL)
    {
        if(strcmp(p, "ls")==0){
            output = find_dir_entry(input);
            char * prev_tolk = p;
            p = strtok(NULL," ");
            if (p == NULL) {break;}
            if (strcmp(p, "<")==0){
                p = strtok(NULL," ");
                ifstream infile;
                infile.open("./"+(string)p);
                string line;
                getline(infile, line);
                input += line + "\n";
                cout << input << endl;
                }
            p = prev_tolk;
            }
        if(strcmp(p, "pwd")==0){output = pwd();}
        if(strcmp(p, "|")==0){input = output;}
        if(strcmp(p, "history")==0){output = get_history();}
        if(strcmp(p, ">")==0){
            redirect = true;
            break;
            }
        //std::cout << p << endl;
        p = strtok(NULL," ");
    }
    //std::cout << c << endl;

    //delete[] c;
        
    if(c.compare("exit") == 0){
        exit(0);
    }
    if (redirect){
        p = strtok(NULL," ");
        std::ofstream out(p);
        std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
        std::cout << output << "\n";
    } else {
        std::cout << output << "\n";
    }
    
    reset_cout(coutbuf);
    
    return 0;
}

string myShell::find_dir_entry (string output){
    char *filename;
    char *o = new char[output.length() + 1];
    strcpy(o, output.c_str());
    struct stat dot_sb;
    if(strcmp(o, "")==0){
        filename = ".";
    }else {
        filename = o;
    }
    stat (filename, &dot_sb);
    DIR *dirp;
    struct stat fileStat;
    struct dirent *dp;

    dirp = opendir (filename);
    //printf("cwd inode is %ld\n", dot_sb.st_ino);
    string rv;
    //printf("Current directory files\n");
    while ((dp = readdir(dirp)) != NULL){
        //stat(dp->d_name, &fileStat);
        //printf("%ld, %lu: %s\n", fileStat.st_ino, dp->d_ino, dp->d_name);
        rv += to_string(dp->d_ino) + ": " + string(dp->d_name) + "\n";
    }

    closedir(dirp);
    return rv;
}

string myShell::pwd(){
    
    char path[256] = {'\0'};
    do{
        DIR *pdir = opendir(".."); //parent
        struct dirent *entp;

        DIR *cdir = opendir("."); //current
        struct dirent *entc;
        __ino_t cid;

        
        while ((entc = readdir (cdir)) != NULL) {
            //printf("%lu %s\n", entc->d_ino, entc->d_name);
            if(strcmp((const char *)entc->d_name, (const char *)".") == 0){
                cid = entc->d_ino;
            }
        }
        char * add_to_path = get_dir_name(entp, pdir, cid);        
        if(strlen(add_to_path) != 0){
            char *tmp = strdup(path);
            strcpy(path, add_to_path);
            strcat(path, tmp);
            free(tmp);
            char *tmp2 = strdup(path);
            strcpy(path, "/");
            strcat(path, tmp2);
            //free(tmp2);
            chdir("..");
        }else{        
            char *tmp3 = strdup(path);
            strcpy(path, "~");
            strcat(path, tmp3);
            //free(tmp3);
            goto home;
        }
        
    }while(true);
    home:
    //printf("%s\n", path);
    chdir(path);
    return string(path);
}

char * myShell::get_dir_name(struct dirent *entp, DIR *pdir, __ino_t cid){
    while ((entp = readdir (pdir)) != NULL) {
        //printf("%lu %s\n", entp->d_ino, entp->d_name);
        if(entp->d_ino == cid){
            char *path = entp->d_name;
            return path;
        }
    }
    return "";
}

void myShell::add_to_history(string command){
    ofstream outfile;
    outfile.open("./history.txt", std::ios_base::app);
    ifstream infile;
    infile.open("./history.txt");
    int number_of_lines = 0;
    string line;
    while (getline(infile, line))
        ++number_of_lines;
    outfile << to_string(number_of_lines)+": "+command << endl;
    outfile.close();
    infile.close();
}

string myShell::get_history(){
    string rv;
    ifstream infile;
    infile.open("./history.txt");
    string line;
    while (getline(infile, line)){
        rv += line + "\n";
    }
    return rv;
}

void myShell::reset_cout(std::streambuf *coutbuf)
{
    std::cout.rdbuf(coutbuf); //reset to standard output again
}
