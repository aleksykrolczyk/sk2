#include "data_lin.h"
using namespace std;

#define PORT 1234

int main() {

    // Setup
    int fd = socket(PF_INET, SOCK_STREAM, 0);
    int option  = 10;

    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    struct sockaddr_in saddr;
    struct sockaddr_in caddr;

    saddr.sin_family = PF_INET;
    saddr.sin_port = htons(PORT);
    saddr.sin_addr.s_addr = INADDR_ANY;

    bind(fd, (struct sockaddr*) &saddr, sizeof(saddr));
    listen(fd, 15);

    socklen_t size;
    int new_sock;

    fd_set active_fd_set, read_fd_set;
    FD_ZERO(&active_fd_set);
    FD_SET(fd, &active_fd_set);

    int i = 0;
    char command;
    string message, bash_command, line, temp_str;

    while(1) {
        read_fd_set = active_fd_set;

        cout << "\nPreselect...\n";
        select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL);
        cout << "Postselect...\n";

        for(i = 0; i < FD_SETSIZE; i++) {
            if(FD_ISSET(i, &read_fd_set)) {
                
                // Accepting new connections by server
                if(i == fd) {
                    size = sizeof(caddr);
                    new_sock = accept(fd, (struct sockaddr*) &caddr, &size);
                    cout << "Accepted...\n";
                    FD_SET(new_sock, &active_fd_set);
                }
                else{
                
                    // Serving client(s)
                    read(i, &command, sizeof(char));
                    message = read_message(i);
                    temp_str = "";
                    printf("\nCommand - %c\nMessage - %s\n", command, message.c_str());

                    if (command == CREATE_FILE){ // Creates new file in 'files/' directory
                        message = without_last_char(message);
                        bash_command = "touch files/" + message;
                        system(bash_command.c_str());
                    }

                    else if (command == READ_FILE){ // returns file content to the client
                        message = without_last_char(message);
                        ifstream file("files/" + message);
                        if(file.is_open()){
                            while(getline(file, line)){
                                temp_str += line + "\n";
                            }
                            send_message(i, SEND_TEXT, temp_str);
                        }
                        file.close();
                    }
                    
                    
                    else if (command == UPDATE_FILE){ // updates file's content
                        string file_name = "";
                        string content = "";
                        int is_file_name = 1;

                        for(char& c : without_last_char(message)){
                            if (c == ETX){
                                is_file_name = 0;
                                continue;
                            }
                            if (is_file_name) file_name += c;
                            if (!is_file_name) content += c;
                        }

                        printf("File_name: %s\nContent: %s\n", file_name.c_str(), content.c_str());

                        ofstream file ("files/" + file_name);
                        message = without_last_char(message);
                        cout << message;
                        file << content;
                        file.close();
                    }

                    else if (command == GET_FILE_NAMES){ // return available file names to the client
                            struct dirent *entry;
                            DIR *dir = opendir("files");
                            string name;
                            while((entry = readdir(dir)) != NULL){
                                name = entry->d_name;
                                temp_str += name + ETX;
                            }
                            send_message(i, SEND_TEXT, temp_str);
                        }
                    
                    else if (command == CON_FIN){ // ends the connection
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
