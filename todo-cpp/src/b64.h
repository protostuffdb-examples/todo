#pragma once

#include <string>
#include <cstring>

namespace b64 {

// original based from https://stackoverflow.com/a/37109258

static const char* B64chars =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const int B64index [256] = { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62, 63, 62, 62, 63, 52, 53, 54, 55,
   56, 57, 58, 59, 60, 61,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,
    7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,
    0,  0,  0, 63,  0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
   41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
};

int encodeTo(char *out, const char *in, int len)
{
    int i;
    char *p = out;
    
    for (i = 0; i < len - 2; i += 3)
    {
        *p++ = B64chars[(in[i] >> 2) & 0x3F];
        *p++ = B64chars[((in[i] & 0x3) << 4) |
                        ((int) (in[i + 1] & 0xF0) >> 4)];
        *p++ = B64chars[((in[i + 1] & 0xF) << 2) |
                        ((int) (in[i + 2] & 0xC0) >> 6)];
        *p++ = B64chars[in[i + 2] & 0x3F];
    }
    if (i < len)
    {
        *p++ = B64chars[(in[i] >> 2) & 0x3F];
        if (i == (len - 1))
        {
            *p++ = B64chars[((in[i] & 0x3) << 4)];
            *p++ = '=';
        }
        else
        {
            *p++ = B64chars[((in[i] & 0x3) << 4) |
                            ((int) (in[i + 1] & 0xF0) >> 4)];
            *p++ = B64chars[((in[i + 1] & 0xF) << 2)];
        }
        *p++ = '=';
    }
    
    *p++ = '\0';
    return p - out;
}

std::string encode(const void* data, const size_t len)
{
    unsigned char* p = (unsigned char*)data;
    size_t d = len % 3;
    std::string str64(4 * (int(d > 0) + len / 3), '=');

    for (size_t i = 0, j = 0; i < len - d; i += 3)
    {
        int n = int(p[i]) << 16 | int(p[i + 1]) << 8 | p[i + 2];
        str64[j++] = B64chars[n >> 18];
        str64[j++] = B64chars[n >> 12 & 0x3F];
        str64[j++] = B64chars[n >> 6 & 0x3F];
        str64[j++] = B64chars[n & 0x3F];
    }
    if (d--)    /// padding
    {
        int n = d ? int(p[len - 2]) << 8 | p[len - 1] : p[len - 1];
        str64[str64.size() - 2] = d ? B64chars[(n & 0xF) << 2] : '=';
        str64[str64.size() - 3] = d ? B64chars[n >> 4 & 0x03F] : B64chars[(n & 3) << 4];
        str64[str64.size() - 4] = d ? B64chars[n >> 10] : B64chars[n >> 2];
    }
    return str64;
}

std::string decode(const void* data, const size_t len)
{
    unsigned char* p = (unsigned char*)data;
    int pad = len > 0 && (len % 4 || p[len - 1] == '=');
    const size_t L = ((len + 3) / 4 - pad) * 4;
    std::string str(L / 4 * 3 + pad, '\0');

    for (size_t i = 0, j = 0; i < L; i += 4)
    {
        int n = B64index[p[i]] << 18 | B64index[p[i + 1]] << 12 | B64index[p[i + 2]] << 6 | B64index[p[i + 3]];
        str[j++] = n >> 16;
        str[j++] = n >> 8 & 0xFF;
        str[j++] = n & 0xFF;
    }
    if (pad)
    {
        int n = B64index[p[L]] << 18 | B64index[p[L + 1]] << 12;
        str[str.size() - 1] = n >> 16;

        if (len > L + 2 && p[L + 2] != '=')
        {
            n |= B64index[p[L + 2]] << 6;
            str.push_back(n >> 8 & 0xFF);
        }
    }
    return str;
}

std::string decode(const std::string& str64)
{
    return decode(str64.c_str(), str64.size());
}

void incAndWriteKeyTo(std::string& out, const char* key)
{
    char decoded[3];
    
    out.assign(key, 12);
    
    char* p = const_cast<char*>(out.data() + 8);
    
    // read 4 bytes
    int n = B64index[p[0]] << 18 | B64index[p[1]] << 12 | B64index[p[2]] << 6 | B64index[p[3]];
    decoded[0] = n >> 16;
    decoded[1] = n >> 8 & 0xFF;
    decoded[2] = n & 0xFF;
    
    // increment
    decoded[2] |= 0x02;
    
    // write 4 bytes
    *p++ = B64chars[(decoded[0] >> 2) & 0x3F];
    *p++ = B64chars[((decoded[0] & 0x3) << 4) |
                    ((int) (decoded[1] & 0xF0) >> 4)];
    *p++ = B64chars[((decoded[1] & 0xF) << 2) |
                    ((int) (decoded[2] & 0xC0) >> 6)];
    *p++ = B64chars[decoded[2] & 0x3F];
}

void decAndWriteKeyTo(std::string& out, const char* key)
{
    char decoded[3];
    
    out.assign(key, 12);
    
    char* p = const_cast<char*>(out.data() + 8);
    
    // read 4 bytes
    int n = B64index[p[0]] << 18 | B64index[p[1]] << 12 | B64index[p[2]] << 6 | B64index[p[3]];
    decoded[0] = n >> 16;
    decoded[1] = n >> 8 & 0xFF;
    decoded[2] = n & 0xFF;
    
    // decrement
    decoded[2] &= 0xFE;
    
    // write 4 bytes
    *p++ = B64chars[(decoded[0] >> 2) & 0x3F];
    *p++ = B64chars[((decoded[0] & 0x3) << 4) |
                    ((int) (decoded[1] & 0xF0) >> 4)];
    *p++ = B64chars[((decoded[1] & 0xF) << 2) |
                    ((int) (decoded[2] & 0xC0) >> 6)];
    *p++ = B64chars[decoded[2] & 0x3F];
}

} // b64
