#include "data.h"
using namespace std;

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
    string message, bash_command, line, temp_str;

    while(1) {
        FD_SET(fd, &readFd);
        writeFd = globalFd;
        timeout.tv_sec = 300;
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
                temp_str = "";
                cout << "** " << command << " - " << message << " **\n\n";

                if (command == CON_INIT){ // debugging basically, to be removed at th
                        cout << "CON_INIT\n";
                        cout << message << endl;
                }

                else if (command == CREATE_FILE){
                    cout << "CREATE_FILE\n";
                    bash_command = "touch files/" + message;
                    system(bash_command.c_str() );
                }

                else if (command == READ_FILE){
                    cout << "RECEIVE_FILE\n";
                    ifstream file("files/" + message);
                    if(file.is_open()){
                        while(getline(file, line)){
                            temp_str += line + "\n";
                        }
                        send_message(cfd, SEND_TEXT, temp_str);
                    }
                }

                else if (command == GET_FILE_NAMES){
                    struct dirent *entry;
                    DIR *dir = opendir("files");
                    int i = 0;
                    while((entry = readdir(dir)) != NULL){
                        if(i > 1) temp_str += string(entry->d_name) + ETX;
                        i += 1;
                    }
                    cout << temp_str << endl;
                    send_message(cfd, SEND_TEXT, temp_str);
                }

                else{
                    printf("Unknown command %c (msg: %s)", command, message.c_str());
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
