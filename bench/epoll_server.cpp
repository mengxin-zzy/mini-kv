#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <iostream>

static void set_nonblocking(int fd){
    int flags = ::fcntl(fd, F_GETFL, 0);
    ::fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}


int main(){
    int listen_fd = ::socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    ::setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(9000);

    ::bind(listen_fd, (sockaddr*)&addr, sizeof(addr));
    ::listen(listen_fd, 128);
    set_nonblocking(listen_fd);
    std::cout << "epoll server listening to 9000" << std::endl;

    int epfd = ::epoll_create1(0);

    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;
    ::epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);

    epoll_event events[64];
    while(true){
        int n = ::epoll_wait(epfd, events, 64, -1);

        for(int i = 0; i < n; i++){
            int fd = events[i].data.fd;

            if(fd == listen_fd){
                int conn = ::accept(listen_fd, nullptr, nullptr);
                set_nonblocking(conn);

                epoll_event cev{};
                cev.events = EPOLLIN;
                cev.data.fd =  conn;
                ::epoll_ctl(epfd, EPOLL_CTL_ADD, conn, &cev);

                std::cout << "new connevtion: fd = " << conn << std::endl;
            }
            else{
                char buf[1024];
                ssize_t r = ::read(fd, buf, sizeof(buf));
                if(r > 0){
                    ::write(fd, buf, r);
                }
                else if (r == 0){
                    std::cout << "close:fd = " << fd << std::endl;
                    ::epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
                    ::close(fd);
                }
                else{
                    if(errno != EAGAIN && errno != EWOULDBLOCK){
                        ::epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
                        ::close(fd);
                    }
                }
            }
        }
    }

}