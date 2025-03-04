#ifndef BUFFER_H
#define BUFFER_H
#include <memory>
#include <vector>
#include <string>
#include <cstring>
#include "common.h"

static const int kPrePendIndex = 8;//prependindex����
static const int kInitalSize = 1024;//��ʼ����С���ٿռ䳤��

class Buffer{
    public:
        DISALLOW_COPY_AND_MOVE(Buffer);
        Buffer();
        ~Buffer();

        //buffer����ʼλ��
        char* begin();
        //const �����begin������ʹ��const�������begin����ʱ
        //���õ��ĵ�����ֻ�ܶ����ݽ��ж������������ܽ����޸�
        const char* begin() const;

        char* beginread();
        const char* beginread() const;

        char* beginwrite();
        const char* beginwrite() const;

        //��������
        void AppendPrepend(const  char* message, int len);
        void Append(const char* message);
        void Append(const char* message, int len);
        void Append(const std::string& message);

        //��ȡ�ɶ���С��
        int readablebytes() const;
        int writeablebytes() const;
        int prependablebytes() const;

        //�鿴����,��������`read_index_`��λ��
        char* Peek();
        const char* Peek() const;
        std::string PeekAsString(int len);
        std::string PeekAllAsString();

        //ȡ���ݣ�ȡ�������read_index_λ�ã��൱�ڲ����ظ�ȡ
        //����
        void Retrieve(int len);
        std::string RetrieveAsString(int len);

        //ȫ��
        void RetrieveAll();
        std::string RetrieveAllAsString();

        //ĳ������֮ǰ
        void RetrieveUtil(const char* end);
        std::string RetrieveUtilAsString(const char* end);

        //�鿴�ռ�
        void EnsureWriteableBytes(int len);

    private:
        std::vector<char> buffer_;
        int read_index_;
        int write_index_;
};
#endif // BUFFER_H
