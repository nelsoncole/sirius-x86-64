#ifndef __COMMUNICATION_H
#define __COMMUNICATION_H

#define COMMUN_TYPE 0x1000
#define COMMUN_TYPE_REBOOT (COMMUN_TYPE + 1)
#define COMMUN_TYPE_POWEROFF (COMMUN_TYPE_REBOOT + 1)
#define COMMUN_TYPE_EXIT (COMMUN_TYPE_POWEROFF + 1)
#define COMMUN_TYPE_FOCO (COMMUN_TYPE_EXIT + 1)
#define COMMUN_TYPE_EXEC (COMMUN_TYPE_FOCO + 1)
#define COMMUN_TYPE_EXEC_CHILD (COMMUN_TYPE_EXEC + 1)

// communication between processes
struct communication{
    unsigned long type;
    unsigned long pid;
    unsigned long apid;
    unsigned char message[1024-24];
};

int communication(struct communication *commun, struct communication *ret);

#endif
