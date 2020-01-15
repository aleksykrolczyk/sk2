#include "data.h"

using namespace std;

void send_message(int fd, char comm, string content){
    int symbols_sent = 0;
    int content_size = content.size() + 1;
    
    write(fd, &comm, sizeof(char));
    while(symbols_sent != content_size){

        symbols_sent += write(fd, &content, content_size);
        cout << symbols_sent << " / " << content_size;
    }
    write(fd, &END_SYMBOL, sizeof(END_SYMBOL));
    cout << "... DONE" << endl;
}

string read_message(int cli_fd){
    string temp_msg = "";
    char symbol = 0;
    while(symbol != END_SYMBOL){
        read(cli_fd, &symbol, sizeof(char));
        if (symbol > 20 || symbol < 16) temp_msg += symbol;
    }
    return temp_msg.substr(0, temp_msg.size()-1);
}

string msg;
char command;

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
    // send_message(fd, CREATE_FILE, "wiadomosc");
    send_message(fd, RECEIVE_FILE, "wiadomosc");
    string content = read_message(fd);
    cout << content;

    close(fd);
    return 0;
}