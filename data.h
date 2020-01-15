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

const char END_SYMBOL = 4;

const char CON_INIT = 'a';
const char CREATE_FILE = 'b';
const char CON_FIN = 'c';
const char RECEIVE_FILE = 'd';
const char SEND_LINE = 'e';


