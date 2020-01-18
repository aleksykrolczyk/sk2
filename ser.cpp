#include "data.h"
using namespace std;

string current_file;

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

    socklen_t size;
    int new_sock;

    char buf[256];
    memset(buf, 0, 256);

    fd_set active_fd_set, read_fd_set;
    FD_ZERO(&active_fd_set);
    FD_SET(fd, &active_fd_set);

    int i = 0;

    char command;
    string message, bash_command, line, temp_str;

    while(1) {
        read_fd_set = active_fd_set;

        cout << "Preselect...\n";
        int res = select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL);
        cout << res <<"Postselect...\n";

        

        for(i = 0; i < FD_SETSIZE; i++) {
            if(FD_ISSET(i, &read_fd_set)) {
                
                // Accepting new connections
                if(i == fd) {
                    size = sizeof(caddr);
                    new_sock = accept(fd, (struct sockaddr*) &caddr, &size);
                    cout << "Accepted...\n";
                    FD_SET(new_sock, &active_fd_set);
                }
                else{
                
                // Actual program
                read(i, &command, sizeof(char));
                message = read_message(i);
                temp_str = "";
                cout << "** " << command << " - " << message << " **\n\n";

                if (command == CON_INIT){ // debugging basically, to be removed at th
                        cout << "CON_INIT\n";
                        cout << message << endl;
                }

                else if (command == CREATE_FILE){
                    cout << "CREATE_FILE\n";
                    message = without_last_char(message);
                    current_file = message;
                    cout << "Current File: " << current_file <<endl;
                    bash_command = "touch files/" + message;
                    system(bash_command.c_str());
                }

                else if (command == READ_FILE){
                    cout << "READ_FILE\n";
                    message = without_last_char(message);
                    current_file = message;
                    cout << "Current File: " << current_file <<endl;
                    ifstream file("files/" + message);
                    if(file.is_open()){
                        while(getline(file, line)){
                            temp_str += line + "\n";
                        }
                        send_message(i, SEND_TEXT, temp_str);
                    }
                    file.close();
                }
                
                
                else if (command == UPDATE_FILE){
                    cout << "UPDATE_FILE\n";
                    cout << "Current File: " << current_file <<endl;
                    ofstream file ("files/"+current_file);
                    message = without_first_and_last_char(message);
                    cout << message;
                    file << message;
                    file.close();
                }

                else if (command == GET_FILE_NAMES){
                        struct dirent *entry;
                        DIR *dir = opendir("files");
                        int length;
                        string name;
                        while((entry = readdir(dir)) != NULL){
                            name = entry->d_name;
                            temp_str += name + ETX;
                        }
                        cout << temp_str << endl;
                        send_message(i, SEND_TEXT, temp_str);
                    }
                
                else if (command == CON_FIN){
                        close(i);
                        FD_CLR(i, &active_fd_set);
                    }

                else printf("Unknown command %c (msg: %s)", command, message.c_str());
                

            }
        }

    }
    }
    close(fd);

    return 0;
}