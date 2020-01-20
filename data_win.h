#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <winsock.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>
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
const char UPDATE_FILE = 'g';
const char BLOCK_FILE = 'h';

const char SAVE_POSSIBLE = 'y';
const char SAVE_NOT_POSSIBLE = 'n';

using namespace std;

string without_first_and_last_char(string content){
    string text="";
    content = content.substr(1, content.length());
    for(unsigned int z=0; z<content.length(); z++)
        if(content[z]!=4&&content[z]!=0) text+=content[z];
    return text;
}

string without_last_char(string content){
    string text="";
    for(unsigned int z=0; z<content.length(); z++)
        if(content[z]!=4&&content[z]!=0) text+=content[z];
    return text;
}

string read_message(SOCKET SSocket){
    string temp_msg = "";
    char symbol = 0;
    while(symbol != END_SYMBOL){
        recv(SSocket, &symbol, sizeof(char),0);
        temp_msg += symbol;
    }
    return temp_msg;
}

void send_message(SOCKET SSocket, char comm, string content){
    int symbols_sent = 0;
    int size = content.length() + 1;
    
    char * msg = new char[size];
    
    strcpy(msg, content.c_str());
    
    send(SSocket, &comm, sizeof(char),0);
    while(symbols_sent != size){
        symbols_sent += send(SSocket, msg, size,0);
        cout << symbols_sent << " / " << size;
    }
    
    send(SSocket, &END_SYMBOL, sizeof(END_SYMBOL),0);

    delete[] msg;
    cout << "... DONE" << endl;
}
