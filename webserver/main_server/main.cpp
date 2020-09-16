

using namespace std;
static const int MAXEVENTS = 5000;

int main()
{
   LOG<<"web_server start";
    handle_for_sigpipe();
    if (Epoll::epoll_init(MAXEVENTS, LISTENQ) < 0)
    {
        perror("epoll init failed");
        return 1;
    }
    if (ThreadPool::threadpool_create(THREADPOOL_THREAD_NUM, QUEUE_SIZE) < 0)
    {
        printf("Threadpool create failed\n");
        return 1;
    }
    int listen_fd = socket_bind_listen(PORT);
    if (listen_fd < 0)
    {
        perror("socket bind failed");
        return 1;
    }
    if (setSocketNonBlocking(listen_fd) < 0)
    {
        perror("set socket non block failed");
        return 1;
    }
    shared_ptr<RequestData> request(new RequestData());
    request->setFd(listen_fd);
    if (Epoll::epoll_add(listen_fd, request, EPOLLIN | EPOLLET) < 0)
    {
        perror("epoll add error");
        return 1;
    }

    while (true)
    {
        //sleep(10);
        Epoll::my_epoll_wait(listen_fd, MAXEVENTS, -1);

        //ThreadPool::threadpool_destroy();
        //break;
    }
    return 0;
}
