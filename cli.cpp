#include "data.h"
using namespace std;

string msg;
char command, trash;

int main(int argc, char *argv[]) {
    // Setup
    struct sockaddr_in addr;
    struct hostent *host;

    int fd = socket(PF_INET, SOCK_STREAM, 0);

    host = gethostbyname("localhost");
    
    addr.sin_family = PF_INET;
    addr.sin_port = htons(1234);
    memcpy(&addr.sin_addr.s_addr, host->h_addr, host->h_length);

    connect(fd, (struct sockaddr*) &addr, sizeof(addr));

    // Actual program
    // send_message(fd, CREATE_FILE, "new_file");
    // send_message(fd, CON_INIT, "test_file");
    // send_message(fd, GET_FILE_NAMES, "");

    send_message(fd, READ_FILE, "wiadomosc");
    read(fd, &trash, sizeof(char)); // reads command (clears this one useless character at the beginning)
    string content = read_message(fd); // use this only if server is to respond somehow
    cout << content;

    close(fd);
    return 0;
}