#include <io.h>

void turn_speaker_on(void) 
{
	outportb(0x61, (inportb(0x61) | 3) );
}

void turn_speaker_off(void) {
	outportb(0x61, (inportb(0x61) & 0xFC) );
}

void set_speaker_frequence(unsigned short freq) {
 	outportb(0x43, 0xB6);
 	outportb(0x42, (unsigned char)freq);
 	outportb(0x42, (unsigned char)(freq >> 8));
}

void play_speaker(unsigned short freq) {
	set_speaker_frequence(freq);
	turn_speaker_on();
}
