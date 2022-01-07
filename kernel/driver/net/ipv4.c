#include <ethernet.h>

int ipv4_send(void *buf, unsigned char protocol, unsigned char *mac, unsigned int src_address, unsigned int dst_address, unsigned length)
{
    ipv4_header_t *hdr = (ipv4_header_t*)buf;

    // Ethernet header
    fillMac((unsigned char*)&hdr->eh.dst, mac);
    fillMac((unsigned char*)&hdr->eh.src, (unsigned char*)&default_ethernet_device.mac);
    hdr->eh.type = ETHERNET_TYPE_IP4;

    // Ipv4 header
    hdr->verIhl = (4 << 4) | 5;
    hdr->tos = 0;
    hdr->len = switch_endian16(length + (sizeof(ipv4_header_t) - sizeof(ether_header_t)));
    hdr->id = switch_endian16(1);
    hdr->offset = switch_endian16(0x4000);
    hdr->ttl = 64;
    hdr->protocol = protocol;
    hdr->checksum = 0;
    hdr->src = src_address;
    hdr->dst = dst_address;

    unsigned int checksum = 0; //net_checksum( (const unsigned char*)hdr, (const unsigned char*)hdr + sizeof(ipv4_header_t));
    checksum += 0x4500;
    checksum += length + (sizeof(ipv4_header_t) - sizeof(ether_header_t));
    checksum += 1;
    checksum += 0x4000;
    checksum += 0x4000 + protocol;
    checksum += (src_address >> 16) & 0xFFFF;
    checksum += src_address & 0xFFFF; 
    checksum += (dst_address >> 16) & 0xFFFF;
    checksum += dst_address & 0xFFFF;
    checksum = (checksum >> 16) + (checksum & 0xffff);
    checksum += (checksum >> 16);
    hdr->checksum = switch_endian16(checksum);

    // Hardware
    ethernet_package_descriptor_t dsc;
    dsc.buffersize = length + sizeof(ipv4_header_t);
    dsc.buf = (void*) hdr;

    if(send_ethernet_package(dsc)) 
        return 1;

    return 0;
}
