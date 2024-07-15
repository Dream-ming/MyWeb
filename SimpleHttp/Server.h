#pragma once  // ��ֹͷ�ļ����ظ�����

// ��ʼ���������׽���
int initListenFd(unsigned short port);

// ����epoll
int epollRun(int lfd);

// �Ϳͻ��˽�������
//int acceptClient(int lfd, int epfd);
void* acceptClient(void* arg);

// ����http������Ϣ
//int recvHttpRequest(int cfd, int epfd);
void* recvHttpRequest(void* arg);

// ����������
int parseRequestLine(const char* line, int cfd);

// �����ļ�
int sendFile(const char* fileName, int cfd);

// ������Ӧͷ(״̬��+��Ӧͷ)
int sendHeadMsg(int cfd, int status, const char* descr, const char* type, int length);

// �����ļ��õ�HTTP��content type
const char* getFileType(const char* name);

// ����Ŀ¼
int sendDir(const char* dirName, int cfd);

// ��16�����ַ�ת����������
int hexToDec(char c);

// ����������ţ�ת��Ϊutf-8
void decodeMsg(char* to, char* from);