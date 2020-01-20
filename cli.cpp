#include "data_win.h"
#include <winsock.h>

#define PORT 1234
#define SERVER "192.168.18.8"

//-lws2_32

SOCKET SSocket;
string msg,file_name;
char command, trash;
ofstream outfile;
int MAX_NAME_SIZE = 20;

using namespace std;

//Handles the saving process when user decides to save his file on server
//Shows him both file stored on computer and server so he can check if he
//does want to keep some of those changes and not overwrite them
void save(){
    char c='c';
    cout << endl << "This is current version of " << file_name << ".txt, that you want to save: " << endl;
    string current_file_text="";
    string line;
    ifstream file(file_name+".txt");
    if(file.is_open()){
        while(getline(file, line)) current_file_text += line + "\n";
        cout << current_file_text << endl ;
    }

    cout << endl << "And this is current version of " << file_name << ".txt, that is stored on server: " << endl;
    
    send_message(SSocket, READ_FILE, file_name);
    string content = read_message(SSocket);
    content = without_first_and_last_char(content);
    cout << content;
    file.close();
    
    
    //Making sure that the user won't unconsciously overwrite somebody's work
    //It also gives user the chance to fix mistakes, etc.
    while(c!='s'&&c!='d'){
	    cout << endl <<"Make sure that there aren't any changes that you want to overwrite"<< endl;
	    cout << "If you are sure that current version of file is the one you want to save on the server then type type 's'" << endl;
	    cout << "If you don't want to make any changes to the server file then press 'd'"<<endl;
	    
	    cin >> c;
	    //if he wants to save changes on server
	    if(c=='s'){
		    current_file_text="";
		    ifstream newfile (file_name + ".txt");
		    if(newfile.is_open()){
		        while(getline(newfile, line)) current_file_text += line + "\n";
		    send_message(SSocket, UPDATE_FILE, file_name + ETX + current_file_text);
		    }
		    newfile.close();
		    cout << endl << "File " << file_name << ".txt has been modified" << endl;
		}
		//if he wants to discard changes
		if(c=='d') break;
	}
}

//Function that counts how many file names are sent by server
// excluding root directory and current directory
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

//Function that returns current directory
string ExePath() {
    char buffer[MAX_PATH];
    GetModuleFileName( NULL, buffer, MAX_PATH );
    string::size_type pos = string( buffer ).find_last_of( "\\/" );
    return string( buffer ).substr( 0, pos);
}

//Thread handler that is supposed to open a notepad with file chosen by user
DWORD WINAPI myThread(LPVOID lpParameter)
{
	string command_and_directory = "notepad.exe " + ExePath() + "//" +file_name + ".txt";
	system(command_and_directory.c_str());
	return 0;
}

// Basically a console interface to communicate with the user
void menu(){
    int n=0;
    int l=0;
    int count = 0;
    
    //getting the list of files stored on server
    send_message(SSocket, GET_FILE_NAMES, "");
    string content = read_message(SSocket);
    content = content.substr(1, content.size());
    
    l = number_of_files(content);
    string file_names[l];
    string name = "";
    
    
    //the list of files came in a string so we have to split it up
	// and delete root directury and current directory from a list
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
    
    
    while(n!=1 && n!=2){
        cout << "Welcome, do you want to create a new file or join to an existing one? Type in a number: " << endl << endl;
        cout << "1 - Create a new file" << endl;
        cout << "2 - Join to an existing file" << endl;
        cin >> n;
        //Create New File
        if(n==1) {
            cout << n;
            cout << endl << endl << "Type the name of your file" << endl;
            cin.ignore();
            getline(cin,file_name);
            //Create File on server
            send_message(SSocket, CREATE_FILE, file_name);
            //Create File on client
            ofstream outfile (file_name + ".txt");
            DWORD myThreadID;
			CreateThread(0, 0, myThread, 0, 0, &myThreadID);
        }
        //Join Existing File
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
            send_message(SSocket, READ_FILE, file_name);
            string content = read_message(SSocket);
            text = without_first_and_last_char(content);
            outfile << text;
            
            DWORD myThreadID;
			CreateThread(0, 0, myThread, 0, 0, &myThreadID);
        }
    }
    
    //When the user already does change the file he can save it on the server
    //User can also open other file to work with, or quit the program
    while(1){
        n=0;
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
}

//Function for handling ctrl+c event in console, so that the connection will be finished and server won't bug
bool consoleHandler(int signal) {

    if (signal == CTRL_C_EVENT) {
		outfile.close();
    	send_message(SSocket, CON_FIN, "");
        closesocket(SSocket);

	    /* terminate use of the winsock */
        WSACleanup();
        exit(0);
    }
    return true;
}

    
int main(int argc, char *argv[]) {
    // Setup
    
    WSADATA WData;
    WORD WRequiredVersion;
    struct sockaddr_in stServerAddr;
    struct hostent* lpstServerEnt;

	/* initialize winsock */
    WRequiredVersion = MAKEWORD(2, 0);
    if (WSAStartup(WRequiredVersion, &WData) != 0){
            fprintf(stderr, "WSAStartup failed!");
            exit(1);
    }
	
	/* look up server's IP address */
    lpstServerEnt = gethostbyname(SERVER);
    if (! lpstServerEnt){
        fprintf(stderr, "%s: Can't get the server's IP address.\n", argv[0]);
        exit(1);
    }
	
    SSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    
    memset(&stServerAddr, 0, sizeof(struct sockaddr));
    stServerAddr.sin_family = AF_INET;
    memcpy(&stServerAddr.sin_addr.s_addr, lpstServerEnt->h_addr, lpstServerEnt->h_length);
    stServerAddr.sin_port = htons(PORT);

	//ctrl+c event handler, makes sure that the connection will be finished and server won't bug
	SetConsoleCtrlHandler((PHANDLER_ROUTINE) consoleHandler, TRUE); 
	
    connect(SSocket, (struct sockaddr*) &stServerAddr,  sizeof(struct sockaddr));
    
    menu();   

    outfile.close();
    
    //Send a message that finishes connection
    send_message(SSocket, CON_FIN, "");
    closesocket(SSocket);

	/* terminate use of the winsock */
    WSACleanup();
    return 0;
}
