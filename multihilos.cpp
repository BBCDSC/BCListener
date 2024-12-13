#define PORT 19999
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <vector>
using namespace std;

void server(struct sockaddr_in* server_addr, int& socketDesc)
{
    memset(server_addr, (char)0, sizeof(sockaddr_in));
    
    socketDesc = socket(AF_INET, SOCK_STREAM, 0);

    if (socketDesc < 0)
    {
        cerr << "Cannot open socket" << endl;
        return;
    }
    server_addr->sin_family = AF_INET;
    server_addr->sin_addr.s_addr = INADDR_ANY;
    server_addr->sin_port = htons(PORT);
    int n;

    if ((n = bind(socketDesc, (struct sockaddr*)server_addr, sizeof(sockaddr_in))) < 0)
    {
        cerr << "Cannot Bind" << endl;
        return;
    }

    if ((n = listen(socketDesc, SOMAXCONN) < 0))
    {
        cerr << "Cannot Listen!" << endl;
        return;
    }
}
void func(struct sockaddr_in* client_addr, int& new_sd)
{
    char memory[1024] = { 0 };
    int n;

    while ((n = recv(new_sd, memory, 1024, 0)))
    {
        cout << "Current Thread ID " << this_thread::get_id() << endl;
        //cout << buffer << endl;
    }
    
}
    
int main(int argc, char* argv[])
{
    int socketDesc;
    struct sockaddr_in server_addr;
    server(&server_addr, socketDesc);

    vector<std::thread> poolofthread;
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(sockaddr_in);
    int new_sd;

            
    while ((new_sd = accept(socketDesc, (sockaddr*)&client_addr, (socklen_t*)&length)) > 0)
    {
        cout << "Client Accepted" << endl;
        thread t(func, &client_addr, ref(new_sd));
        poolofthread.push_back(move(t));
    }
    return 0;

}
