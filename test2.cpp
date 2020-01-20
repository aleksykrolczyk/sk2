#include "data_lin.h"
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

    char response;
    string content = string("AABBCC");
    string file_name = string("test_file");
    send_message(fd, BLOCK_FILE, file_name);
    read(fd, &response, sizeof(char));
    printf("Reponse: -- %c -- \n", response);

    // sleep(1);
    send_message(fd, CON_FIN, "");
    close(fd);
    return 0;
}