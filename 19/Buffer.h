#pragma once
#include<string>
#include<iostream>

class Buffer
{
private:
    std::string buf_;   //用于存放数据
public:
    Buffer();
    ~Buffer();

    void append(const char* data, size_t size); //把数据追加到buf_中
    size_t size();                              //返回数据大小
    const char* data();                         //返回buf_的首地址
    void clear();                               //清空buf_
};