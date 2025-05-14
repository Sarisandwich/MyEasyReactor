#include"Buffer.h"

Buffer::Buffer(u_int16_t sep):sep_(sep)
{}

Buffer::~Buffer()
{}

void Buffer::append(const char* data, size_t size)
{
    buf_.append(data, size);
}

void Buffer::appendwithsep(const char* data, size_t size)
{
    if(sep_==0)
    {
        buf_.append(data, size);    //处理报文内容。
    }
    else if(sep_==1)
    {
        buf_.append((char*)&size, 4);   //处理报文头部。
        buf_.append(data, size);    //处理报文内容。
    }
    
}

size_t Buffer::size()
{
    return buf_.size();
}

const char*Buffer::data()
{
    return buf_.data();
}

void Buffer::clear()
{
    buf_.clear();
}

void Buffer::erase(size_t pos, size_t n)
{
    buf_.erase(pos, n);
}

bool Buffer::pickmessage(std::string &s)
{
    if(buf_.size()==0) return false;

    if(sep_==0)
    {
        s=buf_;
        buf_.clear();
    }
    else if(sep_==1)
    {
        int len;
        memcpy(&len, buf_.data(), 4);   //从buf_中获取报文头部
        //如果buf_中的数据量小于报文头部，说明buf_中的报文内容不完整
        if(buf_.size()<len+4) return false;

        s=buf_.substr(4, len);      //从buf_中获取一个报文
        buf_.erase(0, len+4);       //从buf_中删除刚刚已获取的报文
    }

    return true;
}