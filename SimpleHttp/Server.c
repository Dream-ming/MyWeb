#include "Server.h"
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <assert.h>
#include <sys/sendfile.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>

struct FdInfo
{
    int fd;
    int epfd;
    pthread_t tid;
};

int initListenFd(unsigned short port)
{
    // 1. ����������fd
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
    {
        perror("socket");
        return -1;
    }
    // 2. ���ö˿ڸ���
    int opt = 1;
    int ret = setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (ret == -1)
    {
        perror("setsockopt");
        return -1;
    }
    // 3. ��
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    ret = bind(lfd, (struct sockaddr*)&addr, sizeof(addr));
    if (ret == -1)
    {
        perror("bind");
        return -1;
    }
    // 4. ���ü���
    ret = listen(lfd, 128);
    if (ret == -1)
    {
        perror("listen");
        return -1;
    }
    // ����fd
    return lfd;
}

int epollRun(int lfd)
{
    // 1. ����epollʵ��
    int epfd = epoll_create(1);
    if (epfd == -1)
    {
        perror("epoll_create");
        return -1;
    }
    // 2. ��lfd����epoll���ϼ���
    struct epoll_event ev;
    ev.data.fd = lfd;
    ev.events = EPOLLIN;
    int ret = epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &ev);
    if (ret == -1)
    {
        perror("epoll_ctl");
        return -1;
    }
    // 3. ���
    struct epoll_event evs[1024];
    int size = sizeof(evs) / sizeof(evs[0]);
    while (1)
    {
        int num = epoll_wait(epfd, evs, size, -1);
        for (int i = 0; i < num; ++i)
        {
            struct FdInfo* info = (struct FdInfo*)malloc(sizeof(struct FdInfo));
            int fd = evs[i].data.fd;
            info->epfd = epfd;
            info->fd = fd;
            if (fd == lfd)
            {
                // ����������
                // acceptClient(lfd, epfd);
                pthread_create(&info->tid, NULL, acceptClient, info);
            }
            else {
                // ���ܶԷ����͵�����
                //recvHttpRequest(fd, epfd);
                pthread_create(&info->tid, NULL, recvHttpRequest, info);
            }
        }
    }
    return 0;
}

void* acceptClient(void* arg)
{
    struct FdInfo* info = (struct FdInfo*)arg;
    // 1. ��������
    int cfd = accept(info->fd, NULL, NULL);
    if (cfd == -1)
    {
        perror("accpet");
        return NULL;
    }
    // 2. ���÷�����
    int flag = fcntl(cfd, F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(cfd, F_SETFL, flag);
    // 3. cfd��ӵ�epoll��
    struct epoll_event ev;
    ev.data.fd = cfd;
    ev.events = EPOLLIN | EPOLLET;
    int ret = epoll_ctl(info->epfd, EPOLL_CTL_ADD, cfd, &ev);
    if (ret == -1)
    {
        perror("epoll_ctl");
        return NULL;
    }
    printf("acceptClient threadId: %ld\n", info->tid);
    free(info);
    return NULL;
}

void* recvHttpRequest(void* arg)
{
    struct FdInfo* info = (struct FdInfo*)arg;
    printf("recvHttpRequest...\n");
    int len = 0, totle = 0;
    char tmp[1024] = { 0 };
    char buf[4096] = { 0 };
    while ((len = recv(info->fd, tmp, sizeof(tmp), 0)) > 0)
    {
        if (totle + len < sizeof(buf))
        {
            memcpy(buf + totle, tmp, len);
        }
        totle += len;
    }
    // �ж������Ƿ񱻽������
    if (len == -1 && errno == EAGAIN)
    {
        // ���������У�����ֻ����get����
        char* pt = strstr(buf, "\r\n");
        int reqLen = pt - buf;
        buf[reqLen] = '\0';
        parseRequestLine(buf, info->fd);
    }
    else if (len == 0)
    {
        // �ͻ��˶Ͽ�������
        epoll_ctl(info->epfd, EPOLL_CTL_DEL, info->fd, NULL);
        close(info->fd);
    }
    else
    {
        perror("recv");
    }
    printf("recvMsg threadId: %ld\n", info->tid);
    free(info);
    return NULL;
}
/*
get /xxx/1.jpg http/1.1   // get  Ŀ¼·��  ʹ��Э��
*/
int parseRequestLine(const char* line, int cfd)
{
    // ���������� 
    char method[12];
    char path[1024];
    sscanf(line, "%[^ ] %[^ ]", method, path);
    if (strcasecmp(method, "get") != 0)
    {
        // ������post
        return -1;
    }
    // ����ͻ�������ľ�̬��Դ
    decodeMsg(path, path);
    printf("method: %s, path: %s\n", method, path);
    char* file = NULL;
    if (strcmp(path, "/") == 0)
    {
        file = "./";
    }
    else
    {
        file = path + 1;
    }
    // ��ȡ�ļ����ԣ�ʹ��stat����
    struct stat st;
    int ret = stat(file, &st);
    if (ret == -1)
    {
        // �ļ������� -- �ظ�404
        sendHeadMsg(cfd, 404, "Not Found", getFileType(".html"), -1); // ��֪���ļ��Ĵ�С����д-1
        sendFile("404.html", cfd);
        return 0;
    }
    // �ж��ļ�����
    if(S_ISDIR(st.st_mode))
    {
        // �����Ŀ¼�е����ݷ����ͻ���
        sendHeadMsg(cfd, 200, "OK", getFileType(".html"), -1);
        sendDir(file, cfd);
    }
    else
    {
        // ���ļ������ݷ��͸��ͻ���
        sendHeadMsg(cfd, 200, "OK", getFileType(file), st.st_size); // st�����ļ���С��ֱ����
        sendFile(file, cfd);
    }
    return 0;
}

int sendFile(const char* fileName, int cfd)
{
    // 1. ���ļ�
    int fd = open(fileName, O_RDONLY);
    // ���ԣ�����ļ���ʧ�ܾ���ֹ����
    assert(fd > 0); 
#if 0
    // ����TCP����ʽ����Э�飬����һ��һ�㷢����
    // ��дwhile�����ݣ������û������ں����Ŀ�������������
    while (1)
    {
        char buf[1024];
        int len = read(fd, buf, sizeof(buf));
        if (len > 0)
        {
            send(cfd, buf, len, 0);
            // ���Ͷ˳����ķ������ݿ��ܻᵼ�½��ն˳�������
            // ����usleep(1)���У�����Ҫһ��usleep���ý��ն��ȶ�����
            usleep(10); 
        }
        else if (len == 0)
        {
            break;
        }
        else
        {
            perror("read");
        }
    }
#else
    // ����ֱ����ϵͳ�� sendfile ����
    off_t offset = 0;
    int size = lseek(fd, 0, SEEK_END); // �õ��ļ����ֽ������˴�lseek���ָ���ƶ����ļ�β��
    lseek(fd, 0, SEEK_SET); // ���ļ�ָ���ƻ�ͷ������sendfile�ܴ��ļ�ͷ����ʼ����
    while (offset < size)
    {
        int ret = sendfile(cfd, fd, &offset, size - offset);
        printf("ret value: %d\n", ret);
        if (ret == -1)
        {
            perror("sendfile");
        }
    }

#endif
    close(fd);
    return 0;
}

int sendHeadMsg(int cfd, int status, const char* descr, const char* type, int length)
{
    // ״̬�� = http�汾 + ״̬�� + ״̬����
    char buf[4096] = { 0 };
    sprintf(buf, "http/1.1 %d %s\r\n", status, descr);
    // ��Ӧͷ
    sprintf(buf + strlen(buf), "content-type: %s\r\n", type);
    sprintf(buf + strlen(buf), "content-length: %d\r\n\r\n", length);

    send(cfd, buf, strlen(buf), 0);
    return 0;
}

const char* getFileType(const char* name)
{
    // a.jpg a.mp4 a.html
    // ����������� '.' �ַ����粻���ڷ���NULL
    const char* dot = strrchr(name, '.');
    if (dot == NULL)
        return "text/plain; charset=utf-8"; // ���ı�
    if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
        return "text/html; charset=utf-8";
    if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
        return "image/jpeg";
    if (strcmp(dot, ".gif") == 0)
        return "image/gif";
    if (strcmp(dot, ".png") == 0)
        return "image/png";
    if (strcmp(dot, ".css") == 0)
        return "text/css";
    if (strcmp(dot, ".au") == 0)
        return "audio/basic";
    if (strcmp(dot, ".wav") == 0)
        return "audio/wav";
    if (strcmp(dot, ".avi") == 0)
        return "video/x-msvideo";
    if (strcmp(dot, ".mov") == 0 || strcmp(dot, ".qt") == 0)
        return "video/quicktime";
    if (strcmp(dot, ".mpeg") == 0 || strcmp(dot, ".mpe") == 0)
        return "video/mpeg";
    if (strcmp(dot, ".vrml") == 0 || strcmp(dot, ".wrl") == 0)
        return "model/vrml";
    if (strcmp(dot, ".midi") == 0 || strcmp(dot, ".mid") == 0)
        return "audio/midi";
    if (strcmp(dot, ".mp3") == 0)
        return "audio/mpeg";
    if (strcmp(dot, ".ogg") == 0)
        return "application/ogg";
    if (strcmp(dot, ".pac") == 0)
        return "application/x-ns-proxy-autoconfig";
    
    return "text/plain; charset=utf-8";
}

/*
<html>
    <head>
        <title>test</title>
    </head>
    <body>
        <table>
            <tr>
                <td></td>
                <td></td>
            </tr>
            <tr>
                <td></td>
                <td></td>
            </tr>
        </table>
    </body>
</html>
*/
int sendDir(const char* dirName, int cfd)
{
    char buf[4096] = { 0 };
    sprintf(buf, "<html><head><title>%s</title></head><body><table>", dirName);
    // ����ָ�룬ָ��һ��ָ�����飬ȡ��ַ�õ�����ָ��
    struct dirent** namelist;
    int num = scandir(dirName, &namelist, NULL, alphasort);
    for (int i = 0; i < num; ++i)
    {
        // ȡ���ļ���
        char* name = namelist[i]->d_name;
        struct stat st;
        char subPath[1024] = { 0 };
        printf("%s %s\n", dirName, name);
        sprintf(subPath, "%s/%s", dirName, name);
        stat(subPath, &st);
        if (S_ISDIR(st.st_mode))
        {
            sprintf(buf + strlen(buf), 
                "<tr><td><a href=\"%s/\">%s</a></td><td>%ld</td></tr>", 
                name, name, st.st_size);
        }
        else
        {
            sprintf(buf + strlen(buf),
                "<tr><td><a href=\"%s\">%s</a></td><td>%ld</td></tr>",
                name, name, st.st_size);
        }
        send(cfd, buf, strlen(buf), 0);
        memset(buf, 0, sizeof(buf));
        free(namelist[i]);
    }
    sprintf(buf, "</table></body></html>");
    send(cfd, buf, strlen(buf), 0);
    free(namelist);
    return 0;
}

// ���ַ�ת��Ϊ������
int hexToDec(char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    if (c >= 'a' && c <= 'f')
    {
        return c - 'a' + 10;
    }
    if (c >= 'A' && c <= 'F')
    {
        return c - 'A' + 10;
    }
    return 0;
}

// ���������ַ���ת��Ϊutf-8����������*to���������*from
void decodeMsg(char* to, char* from)
{
    // /%E7%8C%AB%E7%8C%AB.jpeg
    for (; *from != '\0'; ++to, ++from)
    {
        // isxdigit -> �ж��ַ��ǲ���16���Ƹ�ʽ��0-f
        if (from[0] == '%' && isxdigit(from[1]) && isxdigit(from[2]))
        {
            // ��16���Ƶ��� -> ʮ����
            *to = hexToDec(from[1]) * 16 + hexToDec(from[2]);
            from += 2;
        }
        else
        {
            *to = *from;
        }
    }
    *to = '\0';
}