#ifndef __WAV_H
#define __WAV_H

typedef struct _WAV_HEADER {

	char 	riff_tag[4];
	int		riff_length;
	char 	wave_tag[4];
	char 	fmt_tag[4];
	int 	fmt_length;
	short	audio_format;
	short	num_channels;
	int		sample_rate;
	int		byte_rate;
	short	block_align;
	short	bits_per_sample;
	char 	data_tag[4];
	int		data_length;

}__attribute__((packed)) WAV_HEADER;



#endif
