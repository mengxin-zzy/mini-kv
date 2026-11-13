#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
#include "db.h"

static void set_nonblocking(int fd){
    int flags = ::fcntl(fd, F_GETFL, 0);
    ::fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

static std::unordered_map<int, std::string> g_buffers;

static void handle_line(KV& db, int fd, const std::string& line){
    std::istringstream iss(line);
    std::string cmd, key, val;
    iss >> cmd;

    if(cmd == "SET"){
        iss >> key >> val;
        if(key.empty() || val.empty()){
            ::write(fd, "-ERR bad args\n", 14);
            return;
        }
        db.Put(key, val);
        ::write(fd, "+OK\n", 4);
    }
    else if(cmd == "GET"){
        iss >> key;
        auto v = db.Get(key);
        if(!v){
            ::write(fd, "$-1\n", 4);
            return;
        }
        std::string resp = "$" + std::to_string(v->size()) + "\n" + *v + "\n";
        ::write(fd, resp.data(), resp.size());
    }
    else if(cmd == "DEL"){
        iss >> key;
        std::string resp = db.Delete(key) ? "+1\n" : "+0\n";
        ::write(fd, resp.data(), resp.size());
    }
    else{
        std::string resp = "-ERR unknow command'" + cmd + "'\n";
        ::write(fd, resp.data(), resp.size());
    }
}

static void process_buffer(KV& db, int fd){
    auto& buf = g_buffers[fd];
    size_t pos;
    while((pos = buf.find('\n')) != std::string::npos){
        std::string line = buf.substr(0, pos);
        buf.erase(0, pos + 1);
        if (!line.empty() && line.back() == '\r')line.pop_back();
        if (!line.empty())handle_line(db, fd, line);
    }
}


int main(){
    KV db;
    db.Load();

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

    int epfd = ::epoll_create1(0);
    epoll_event ev{};
    ev.events = EPOLLIN;
    ev.data.fd = listen_fd;
    ::epoll_ctl(epfd, EPOLL_CTL_ADD, listen_fd, &ev);
    std::cout << "kv_server lsitening on 9000\n";

    epoll_event events[64];
    while(true){
        int n = ::epoll_wait(epfd, events, 64, -1);
        for(int i = 0; i < n; i++){
            int fd = events[i].data.fd;

            if(fd == listen_fd){
                int conn = ::accept(listen_fd, nullptr, nullptr);
                if(conn < 0) continue;
                set_nonblocking(conn);
                epoll_event cev{};
                cev.events = EPOLLIN;
                cev.data.fd = conn;
                ::epoll_ctl(epfd, EPOLL_CTL_ADD, conn, &cev);
                g_buffers[conn] = "";

                std::cout << "connected fd =" << conn << std::endl;
            }
            else{
                char buf[4096];
                ssize_t r = ::read(fd, buf, sizeof(buf));
                if(r > 0){
                    g_buffers[fd].append(buf, r);
                    process_buffer(db, fd);
                }
                else if(r == 0){
                    std::cout << " closed fd = " << fd << std::endl;
                    ::epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
                    ::close(fd);
                    g_buffers.erase(fd);
                }
                else{
                    if(errno != EAGAIN && errno != EWOULDBLOCK){
                        ::epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
                        ::close(fd);
                        g_buffers.erase(fd);
                    }
                }
            }
        }
    }
}