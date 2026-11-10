#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdio>
#include <iostream>

int main(){
    int listen_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0){ perror("socket"); return 1;}

    int opt = 1;
    ::setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);
    if(::bind(listen_fd, (sockaddr*)&addr, sizeof(addr)) < 0){
        perror("bind");
        return 1;
    }

    if(::listen(listen_fd, 128) < 0){
        perror("listen");
        return 1;
    }

    std::cout << "listening on 9000..." << std::endl;

    while(true){
        int conn = ::accept(listen_fd, nullptr, nullptr);

        if(conn < 0){
            perror("accept");
            continue;
        }

        char buf[1024];

        ssize_t n = ::read(conn, buf, sizeof(buf));
        if(n > 0) ::write(conn, buf, n);

        ::close(conn);
    }
}