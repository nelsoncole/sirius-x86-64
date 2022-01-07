#include <ethernet.h>

static unsigned int net_checksum_acc(const unsigned char *data, const unsigned char *end, unsigned int sum)
{
    unsigned int len = end - data;
    unsigned short *p = (unsigned short *)data;

    while (len > 1)
    {
        sum += *p++;
        len -= 2;
    }

    if (len)
    {
        sum += *(unsigned char *)p;
    }

    return sum;
}

static unsigned short net_checksum_final(unsigned int sum)
{
    sum = (sum & 0xffff) + (sum >> 16);
    sum += (sum >> 16);

    unsigned short temp = ~sum;
    return ((temp & 0x00ff) << 8) | ((temp & 0xff00) >> 8);
}

unsigned short net_checksum(const unsigned char *data, const unsigned char *end)
{
    unsigned int sum = net_checksum_acc(data, end, 0);
    return net_checksum_final(sum);
}
