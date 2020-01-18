#include "data.h"
#include <iostream>
#include <fstream>

int fd;
string msg,file_name;
char command, trash;
ofstream outfile;
int MAX_NAME_SIZE = 20;

using namespace std;

void save(){
    string empty;
    cout << endl << "This is current version of " << file_name << ".txt, that you want to save: " << endl;
    string current_file_text="";
    string line;
    ifstream file(file_name+".txt");
    if(file.is_open()){
        while(getline(file, line)) current_file_text += line + "\n";
        cout << current_file_text << endl ;
    }

    cout << endl << "And this is current version of " << file_name << ".txt, that is stored on server: " << endl;
    
    send_message(fd, READ_FILE, file_name);
    string content = read_message(fd);
    content = without_first_and_last_char(content);
    cout << content;
    
    cout << endl <<"Make sure that there aren't any changes that you want to overwrite, if you are sure that current version of file is the one you want to save on the server then type anything press enter"<< endl;
    
    cin >> empty;
    
    current_file_text="";
    //ifstream file (file_name + ".txt");
    if(file.is_open()){
        while(getline(file, line)) current_file_text += line + "\n";
    send_message(fd, UPDATE_FILE, current_file_text);
    }
    file.close();
    cout << endl << "File " << file_name << ".txt has been modified" << endl;
}

int number_of_files(string content){
    int i=0;
    string name = "";
    for(char& c : content){
        if(c == ETX){
            if (name.length() == 1 && name[0] == '.'){
                name="";
                continue;
            }
            if (name.length() == 2 && name == ".."){
                name="";
                continue;
            }
            name="";
            i++;
        }
        else name+=c;
    }
    return i;   
}

void menu(){
    int n=0;
    int l=0;
    int count = 0;
    
    //getting the list of files
    send_message(fd, GET_FILE_NAMES, "");
    string content = read_message(fd);

    content = content.substr(1, content.size());
    
    l = number_of_files(content);
    string file_names[l];
    string name = "";
    
    cout<<endl<<l<<endl;
    
    for(char& c : content){
        if(c != ETX) name = name + c;
        else {
            if (name.length() == 1 && name[0] == '.'){
                name="";
                continue;
            }
            if (name.length() == 2 && name == ".."){
                name="";
                continue;
            }
            file_names[count] = name;
            count++;
            name="";
        }
    }
    while(n!=1&&n!=2){
        cout << "Welcome, do you want to create a new file or join to an existing one? Type in a number: " << endl << endl;
        cout << "1 - Create a new file" << endl;
        cout << "2 - Join to an existing file" << endl;
        cin >> n;
        //New File
        if(n==1) {
            cout << n;
            cout << endl << endl << "Type the name of your file" << endl;
            cin.ignore();
            getline(cin,file_name);
            //Create File on server
            send_message(fd, CREATE_FILE, file_name);
            //Create File on client
            ofstream outfile (file_name + ".txt");
            cout << "Open " << file_name << ".txt and start modifying it"<<endl;
            sleep(10);
        }
        //Join File
        else if(n==2){
            int num;
            cout << endl << "Here is the list of existing files, stored on our server. Please insert a corresponing number "<< endl << endl;
            for (int i=0;i<l;i++){
                cout << i+1 << " - " << file_names[i] << endl;
            }
            cin >> num;
            
            string text;
            file_name = file_names[num-1];
            ofstream outfile (file_name + ".txt");
            send_message(fd, READ_FILE, file_name);
            string content = read_message(fd);
            text = without_first_and_last_char(content);
            outfile << text;
            
            cout << "Open " << file_name << ".txt and start modifying it"<<endl;
            sleep(10);
            
        }
    }
}

void new_sigint(int xd){
    send_message(fd, CON_FIN, "");
    outfile.close();
    exit(0);
}

    
int main(int argc, char *argv[]) {
    // Setup
    struct sockaddr_in addr;
    struct hostent *host;

    fd = socket(PF_INET, SOCK_STREAM, 0);
    signal(SIGINT,new_sigint);
    
    host = gethostbyname("localhost");
    
    addr.sin_family = PF_INET;
    addr.sin_port = htons(1234);
    memcpy(&addr.sin_addr.s_addr, host->h_addr, host->h_length);

    connect(fd, (struct sockaddr*) &addr, sizeof(addr));



            /*
            string temp_str="";
            string line;
            ifstream file("wiadomosc.txt");
            if(file.is_open()){
                while(getline(file, line)) temp_str += line + "\n";
            send_message(fd, UPDATE_FILE, temp_str);
            */
    menu();   
            
            
    while(1){
        int n=0;
        int i;
        cout << endl <<"Do you want to send the changes to the server? Remember to first save the file on your computer."<< endl;
        cout << "1 - Yes, I want to save my progress" <<endl;
        cout << "2 - No, I want to work with another file" <<endl;
        cout << "3 - No, I want to quit without saving" <<endl;
        cin >> n;
        
        if(n==1){
            save();
            cout << endl <<"What do you want to do next?" <<endl;
            cout << "1 - Continue working with current file" <<endl;
            cout << "2 - Choose another file to work with" <<endl;
            cout << "3 - Quit the program" <<endl;
            cin >> i;
            if (i==1){ cout << "Open " << file_name << ".txt and start modifying it"<<endl;
                sleep(10);
            }
            if (i==2) menu();
            if (i==3) break;
        }
        if(n==2) menu();
        if(n==3) break;
    }
    outfile.close();
    send_message(fd, CON_FIN, "");
    close(fd);
    return 0;
}
