#ifndef __DNS_H__
#define __DNS_H__

struct DNS_HEADER
{
    unsigned short id; // identification number

    unsigned short flags;

    unsigned short q_count; // number of question entries
    unsigned short answer_rr; // number of answer entries
    unsigned short authority_rr; // number of authority entries
    unsigned short aditional_rr; // number of resource entries

}__attribute__ ((packed));


#endif
