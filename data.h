#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>

#include <string.h>
#include <fstream>
#include <dirent.h>
#include <sstream>

const char END_SYMBOL = 4;
const char ETX = 3;

const char CON_INIT = 'a';
const char CREATE_FILE = 'b';
const char CON_FIN = 'c';
const char READ_FILE = 'd';
const char SEND_TEXT = 'e';
const char GET_FILE_NAMES = 'f';


// Not a good practice but whatever

using namespace std;

string without_last_char(string input){
    // not used right know, might be handy someday
    return input.substr(0, input.size()-1);
}

string read_message(int cli_fd){
    string temp_msg = "";
    char symbol = 0;
    while(symbol != END_SYMBOL){
        read(cli_fd, &symbol, sizeof(char));
        temp_msg += symbol;
    }
    return temp_msg;
}

void send_message(int fd, char comm, string content){
    int symbols_sent = 0;
    int size = content.length() + 1;
    char * msg = new char[size];
    strcpy(msg, content.c_str());

    write(fd, &comm, sizeof(char));
    while(symbols_sent != size){
        symbols_sent += write(fd, msg, size);
        cout << symbols_sent << " / " << size;

    }
    write(fd, &END_SYMBOL, sizeof(END_SYMBOL));

    delete[] msg;
    cout << "... DONE" << endl;
}