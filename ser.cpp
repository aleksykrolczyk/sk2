#include "data.h"

using namespace std;

string read_message(int cli_fd){
    string temp_msg = "";
    char symbol = 0;
    while(symbol != END_SYMBOL){
        read(cli_fd, &symbol, sizeof(char));
        if (symbol > 20 || symbol < 16) temp_msg += symbol;
    }
    return temp_msg.substr(0, temp_msg.size()-1);
}

void send_message(int fd, char comm, string content){
    int symbols_sent = 0;
    int content_size = content.size() + 1;
    
    write(fd, &comm, sizeof(char));
    while(symbols_sent != content_size){

        symbols_sent += write(fd, &content, content_size);
        cout << symbols_sent << " / " << content_size;
    }

}

void stop_sending(int fd){
    write(fd, &END_SYMBOL, sizeof(END_SYMBOL));
    cout << "... DONE" << endl;
}

int main() {

    int fd = socket(PF_INET, SOCK_STREAM, 0);
    int option  = 10;

    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    struct sockaddr_in saddr;
    struct sockaddr_in caddr;

    saddr.sin_family = PF_INET;
    saddr.sin_port = htons(1234);
    saddr.sin_addr.s_addr = INADDR_ANY;

    bind(fd, (struct sockaddr*) &saddr, sizeof(saddr));
    listen(fd, 15);

    socklen_t s;
    int cfd;

    char buf[256];
    memset(buf, 0, 256);

    fd_set readFd, writeFd, globalFd;

    FD_ZERO(&readFd);
    FD_ZERO(&writeFd);
    FD_ZERO(&globalFd);

    struct timeval timeout;
    int fd_max = fd;
    int  fda = 0;
    int i = 0;

    char command;
    string message, bash_command, line;

    while(1) {
        FD_SET(fd, &readFd);
        writeFd = globalFd;
        timeout.tv_sec = 25;
        timeout.tv_usec = 0;

        cout << "Preselect...\n";
        int res = select(fd_max + 1, &readFd, &writeFd, (fd_set*) 0, &timeout);
        cout << "Postselect...\n";

        if(res == 0) {
            cout << "Timeout...\n";
            continue;
        }

        fda = res;

        // Accepting new connections
        if(FD_ISSET(fd, &readFd)) {
            fda -= 1;
            s = sizeof(caddr);
            cfd = accept(fd, (struct sockaddr*) &caddr, &s);
            cout << "Accepted...\n";
            FD_SET(cfd, &globalFd);
            if(cfd > fd) fd_max = cfd;
        }

        for(i = fd + 1; i <= fd_max && fda > 0; i++) {
            if(FD_ISSET(i, &writeFd)) {
                fda -= 1;
                
                // Actual program
                read(cfd, &command, sizeof(char));
                message = read_message(cfd);
                cout << "** " << command << " - " << message << " **\n\n";

                switch( command ){
                    case CON_INIT:
                        cout << "CON_INIT\n";
                        cout << cfd << " napisal: " << message << endl;
                        break;

                    case CREATE_FILE:
                        cout << "CREATE_FILE\n";
                        bash_command = "touch " + message + ".txt";
                        system(bash_command.c_str() );
                        break;

                    case RECEIVE_FILE:{
                        cout << "RECEIVE_FILE\n";
                        ifstream file(message + ".txt");
                        if(file.is_open()){
                            while(getline(file, line)){
                                cout << "Line: " << line << "\n";
                                send_message(cfd, SEND_LINE, line + '\n');
                            }
                            stop_sending(cfd);
                        }
                    }

                    default:
                        cout << "Unkown command: " << command << endl;
                        break;
                }
                

                ///
                close(i);
                FD_CLR(i, &globalFd);
                if(i == fd_max)
                    while(fd_max > fd && ! FD_ISSET(fd_max, &globalFd)) {
                        fd_max -= 1;
                    }
            }
        }

    }
    close(fd);

    return 0;
}
