#include "data.h"
using namespace std;

string msg;
char command, trash;

int main(int argc, char *argv[]) {
    // Setup
    struct sockaddr_in addr;
    struct hostent *host;

    int fd = socket(PF_INET, SOCK_STREAM, 0);
    cout << "-- " << fd <<  " --\n";


    host = gethostbyname("localhost");
    
    addr.sin_family = PF_INET;
    addr.sin_port = htons(1234);
    memcpy(&addr.sin_addr.s_addr, host->h_addr, host->h_length);

    connect(fd, (struct sockaddr*) &addr, sizeof(addr));

    // Actual program


    // send_message(fd, CON_INIT, "proc1");
    // sleep(1);
    // send_message(fd, CON_INIT, "proc1");

    string content;
    send_message(fd, GET_FILE_NAMES, "aaaa");
    read(fd, &trash, sizeof(char)); // reads command (clears this one useless character at the beginning)
    content = read_message(fd); // use this only if server is to respond somehow
    cout << content;
    sleep(5);
    send_message(fd, GET_FILE_NAMES, "aaaaa");
    read(fd, &trash, sizeof(char)); // reads command (clears this one useless character at the beginning)
    content = read_message(fd); // use this only if server is to respond somehow
    cout << content;

    send_message(fd, CON_FIN, "");
    close(fd);
    return 0;
}