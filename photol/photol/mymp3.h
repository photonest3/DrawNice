#ifndef MINIMP3_H
#define MINIMP3_H

#include <stdint.h>
#include <stdlib.h>

typedef struct
{
	int frame_bytes, frame_offset, channels, hz, layer, bitrate_kbps;
} mp3dec_frame_info_t;

typedef struct
{
	float mdct_overlap[2][9 * 32], qmf_state[15 * 2 * 32];
	int reserv, free_format_bytes;
	unsigned char header[4], reserv_buf[511];
} mp3dec_t;

typedef struct
{
	char *buffer;
	size_t buffer_size;
	size_t samples; 
	int channels, hz, layer, bitrate_kbps, frame_len;
} mp3dec_file_info_t;

typedef struct
{
	const uint8_t *buffer;
	size_t size;
} mp3dec_map_info_t;



#define MP3D_E_PARAM   -1
#define MP3D_E_MEMORY  -2
#define MP3D_E_IOERROR -3
#define MP3D_E_USER    -4  /* can be used to stop processing from callbacks without indicating specific error */
#define MP3D_E_DECODE  -5  /* decode error which can't be safely skipped, such as sample rate, layer and channels change */




#define MAX_FREE_FORMAT_FRAME_SIZE  2304    /* more than ISO spec's */

#define MAX_FRAME_SYNC_MATCHES      10


#define SHORT_BLOCK_TYPE            2
#define HDR_SIZE                    4
#define HDR_IS_MONO(h)              (((h[3]) & 0xC0) == 0xC0)
#define HDR_IS_MS_STEREO(h)         (((h[3]) & 0xE0) == 0x60)
#define HDR_IS_FREE_FORMAT(h)       (((h[2]) & 0xF0) == 0)
#define HDR_IS_CRC(h)               (!((h[1]) & 1))
#define HDR_TEST_PADDING(h)         ((h[2]) & 0x2)
#define HDR_TEST_MPEG1(h)           ((h[1]) & 0x8)
#define HDR_TEST_NOT_MPEG25(h)      ((h[1]) & 0x10)
#define HDR_TEST_I_STEREO(h)        ((h[3]) & 0x10)
#define HDR_TEST_MS_STEREO(h)       ((h[3]) & 0x20)
#define HDR_GET_STEREO_MODE(h)      (((h[3]) >> 6) & 3)
#define HDR_GET_STEREO_MODE_EXT(h)  (((h[3]) >> 4) & 3)
#define HDR_GET_LAYER(h)            (((h[1]) >> 1) & 3)
#define HDR_GET_BITRATE(h)          ((h[2]) >> 4)
#define HDR_GET_SAMPLE_RATE(h)      (((h[2]) >> 2) & 3)
#define HDR_GET_MY_SAMPLE_RATE(h)   (HDR_GET_SAMPLE_RATE(h) + (((h[1] >> 3) & 1) + ((h[1] >> 4) & 1))*3)
#define HDR_IS_FRAME_576(h)         ((h[1] & 14) == 2)
#define HDR_IS_LAYER_1(h)           ((h[1] & 6) == 6)



typedef struct
{
	const uint8_t *buf;
	int pos, limit;
} bs_t;

typedef struct
{
	const uint8_t *sfbtab;
	uint16_t part_23_length, big_values, scalefac_compress;
	uint8_t global_gain, block_type, mixed_block_flag, n_long_sfb, n_short_sfb;
	uint8_t table_select[3], region_count[3], subblock_gain[3];
	uint8_t preflag, scalefac_scale, count1_table, scfsi;
} L3_gr_info_t;

void bs_init(bs_t *bs, const uint8_t *data, int bytes)
{
	bs->buf = data;
	bs->pos = 0;
	bs->limit = bytes * 8;
}

uint32_t get_bits(bs_t *bs, int n)
{
	uint32_t next, cache = 0, s = bs->pos & 7;
	int shl = n + s;
	const uint8_t *p = bs->buf + (bs->pos >> 3);
	if ((bs->pos += n) > bs->limit)
		return 0;
	next = *p++ & (255 >> s);
	while ((shl -= 8) > 0)
	{
		cache |= next << shl;
		next = *p++;
	}
	return cache | (next >> -shl);
}

int hdr_valid(const uint8_t *h)
{
	return h[0] == 0xff &&
		((h[1] & 0xF0) == 0xf0 || (h[1] & 0xFE) == 0xe2) &&
		(HDR_GET_LAYER(h) != 0) &&
		(HDR_GET_BITRATE(h) != 15) &&
		(HDR_GET_SAMPLE_RATE(h) != 3);
}

int hdr_compare(const uint8_t *h1, const uint8_t *h2)
{
	return hdr_valid(h2) &&
		((h1[1] ^ h2[1]) & 0xFE) == 0 &&
		((h1[2] ^ h2[2]) & 0x0C) == 0 &&
		!(HDR_IS_FREE_FORMAT(h1) ^ HDR_IS_FREE_FORMAT(h2));
}

unsigned hdr_bitrate_kbps(const uint8_t *h)
{
	const uint8_t halfrate[2][3][15] = {
	   { { 0,4,8,12,16,20,24,28,32,40,48,56,64,72,80 },{ 0,4,8,12,16,20,24,28,32,40,48,56,64,72,80 },{ 0,16,24,28,32,40,48,56,64,72,80,88,96,112,128 } },
	   { { 0,16,20,24,28,32,40,48,56,64,80,96,112,128,160 },{ 0,16,24,28,32,40,48,56,64,80,96,112,128,160,192 },{ 0,16,32,48,64,80,96,112,128,144,160,176,192,208,224 } },
	};
	return 2 * halfrate[!!HDR_TEST_MPEG1(h)][HDR_GET_LAYER(h) - 1][HDR_GET_BITRATE(h)];
}

unsigned hdr_sample_rate_hz(const uint8_t *h)
{
	const unsigned g_hz[3] = { 44100, 48000, 32000 };
	return g_hz[HDR_GET_SAMPLE_RATE(h)] >> (int)!HDR_TEST_MPEG1(h) >> (int)!HDR_TEST_NOT_MPEG25(h);
}

unsigned hdr_frame_samples(const uint8_t *h)
{
	return HDR_IS_LAYER_1(h) ? 384 : (1152 >> (int)HDR_IS_FRAME_576(h));
}

int hdr_frame_bytes(const uint8_t *h, int free_format_size)
{
	int frame_bytes = hdr_frame_samples(h)*hdr_bitrate_kbps(h) * 125 / hdr_sample_rate_hz(h);
	if (HDR_IS_LAYER_1(h))
	{
		frame_bytes &= ~3; /* slot align */
	}
	return frame_bytes ? frame_bytes : free_format_size;
}

int hdr_padding(const uint8_t *h)
{
	return HDR_TEST_PADDING(h) ? (HDR_IS_LAYER_1(h) ? 4 : 1) : 0;
}

int L3_read_side_info(bs_t *bs, L3_gr_info_t *gr, const uint8_t *hdr)
{
	const uint8_t g_scf_long[8][23] = {
	   { 6,6,6,6,6,6,8,10,12,14,16,20,24,28,32,38,46,52,60,68,58,54,0 },
	   { 12,12,12,12,12,12,16,20,24,28,32,40,48,56,64,76,90,2,2,2,2,2,0 },
	   { 6,6,6,6,6,6,8,10,12,14,16,20,24,28,32,38,46,52,60,68,58,54,0 },
	   { 6,6,6,6,6,6,8,10,12,14,16,18,22,26,32,38,46,54,62,70,76,36,0 },
	   { 6,6,6,6,6,6,8,10,12,14,16,20,24,28,32,38,46,52,60,68,58,54,0 },
	   { 4,4,4,4,4,4,6,6,8,8,10,12,16,20,24,28,34,42,50,54,76,158,0 },
	   { 4,4,4,4,4,4,6,6,6,8,10,12,16,18,22,28,34,40,46,54,54,192,0 },
	   { 4,4,4,4,4,4,6,6,8,10,12,16,20,24,30,38,46,56,68,84,102,26,0 }
	};
	const uint8_t g_scf_short[8][40] = {
	   { 4,4,4,4,4,4,4,4,4,6,6,6,8,8,8,10,10,10,12,12,12,14,14,14,18,18,18,24,24,24,30,30,30,40,40,40,18,18,18,0 },
	   { 8,8,8,8,8,8,8,8,8,12,12,12,16,16,16,20,20,20,24,24,24,28,28,28,36,36,36,2,2,2,2,2,2,2,2,2,26,26,26,0 },
	   { 4,4,4,4,4,4,4,4,4,6,6,6,6,6,6,8,8,8,10,10,10,14,14,14,18,18,18,26,26,26,32,32,32,42,42,42,18,18,18,0 },
	   { 4,4,4,4,4,4,4,4,4,6,6,6,8,8,8,10,10,10,12,12,12,14,14,14,18,18,18,24,24,24,32,32,32,44,44,44,12,12,12,0 },
	   { 4,4,4,4,4,4,4,4,4,6,6,6,8,8,8,10,10,10,12,12,12,14,14,14,18,18,18,24,24,24,30,30,30,40,40,40,18,18,18,0 },
	   { 4,4,4,4,4,4,4,4,4,4,4,4,6,6,6,8,8,8,10,10,10,12,12,12,14,14,14,18,18,18,22,22,22,30,30,30,56,56,56,0 },
	   { 4,4,4,4,4,4,4,4,4,4,4,4,6,6,6,6,6,6,10,10,10,12,12,12,14,14,14,16,16,16,20,20,20,26,26,26,66,66,66,0 },
	   { 4,4,4,4,4,4,4,4,4,4,4,4,6,6,6,8,8,8,12,12,12,16,16,16,20,20,20,26,26,26,34,34,34,42,42,42,12,12,12,0 }
	};
	const uint8_t g_scf_mixed[8][40] = {
	   { 6,6,6,6,6,6,6,6,6,8,8,8,10,10,10,12,12,12,14,14,14,18,18,18,24,24,24,30,30,30,40,40,40,18,18,18,0 },
	   { 12,12,12,4,4,4,8,8,8,12,12,12,16,16,16,20,20,20,24,24,24,28,28,28,36,36,36,2,2,2,2,2,2,2,2,2,26,26,26,0 },
	   { 6,6,6,6,6,6,6,6,6,6,6,6,8,8,8,10,10,10,14,14,14,18,18,18,26,26,26,32,32,32,42,42,42,18,18,18,0 },
	   { 6,6,6,6,6,6,6,6,6,8,8,8,10,10,10,12,12,12,14,14,14,18,18,18,24,24,24,32,32,32,44,44,44,12,12,12,0 },
	   { 6,6,6,6,6,6,6,6,6,8,8,8,10,10,10,12,12,12,14,14,14,18,18,18,24,24,24,30,30,30,40,40,40,18,18,18,0 },
	   { 4,4,4,4,4,4,6,6,4,4,4,6,6,6,8,8,8,10,10,10,12,12,12,14,14,14,18,18,18,22,22,22,30,30,30,56,56,56,0 },
	   { 4,4,4,4,4,4,6,6,4,4,4,6,6,6,6,6,6,10,10,10,12,12,12,14,14,14,16,16,16,20,20,20,26,26,26,66,66,66,0 },
	   { 4,4,4,4,4,4,6,6,4,4,4,6,6,6,8,8,8,12,12,12,16,16,16,20,20,20,26,26,26,34,34,34,42,42,42,12,12,12,0 }
	};

	unsigned tables, scfsi = 0;
	int main_data_begin, part_23_sum = 0;
	int sr_idx = HDR_GET_MY_SAMPLE_RATE(hdr); sr_idx -= (sr_idx != 0);
	int gr_count = HDR_IS_MONO(hdr) ? 1 : 2;

	if (HDR_TEST_MPEG1(hdr))
	{
		gr_count *= 2;
		main_data_begin = get_bits(bs, 9);
		scfsi = get_bits(bs, 7 + gr_count);
	}
	else
	{
		main_data_begin = get_bits(bs, 8 + gr_count) >> gr_count;
	}

	do
	{
		if (HDR_IS_MONO(hdr))
		{
			scfsi <<= 4;
		}
		gr->part_23_length = (uint16_t)get_bits(bs, 12);
		part_23_sum += gr->part_23_length;
		gr->big_values = (uint16_t)get_bits(bs, 9);
		if (gr->big_values > 288)
		{
			return -1;
		}
		gr->global_gain = (uint8_t)get_bits(bs, 8);
		gr->scalefac_compress = (uint16_t)get_bits(bs, HDR_TEST_MPEG1(hdr) ? 4 : 9);
		gr->sfbtab = g_scf_long[sr_idx];
		gr->n_long_sfb = 22;
		gr->n_short_sfb = 0;
		if (get_bits(bs, 1))
		{
			gr->block_type = (uint8_t)get_bits(bs, 2);
			if (!gr->block_type)
			{
				return -1;
			}
			gr->mixed_block_flag = (uint8_t)get_bits(bs, 1);
			gr->region_count[0] = 7;
			gr->region_count[1] = 255;
			if (gr->block_type == SHORT_BLOCK_TYPE)
			{
				scfsi &= 0x0F0F;
				if (!gr->mixed_block_flag)
				{
					gr->region_count[0] = 8;
					gr->sfbtab = g_scf_short[sr_idx];
					gr->n_long_sfb = 0;
					gr->n_short_sfb = 39;
				}
				else
				{
					gr->sfbtab = g_scf_mixed[sr_idx];
					gr->n_long_sfb = HDR_TEST_MPEG1(hdr) ? 8 : 6;
					gr->n_short_sfb = 30;
				}
			}
			tables = get_bits(bs, 10);
			tables <<= 5;
			gr->subblock_gain[0] = (uint8_t)get_bits(bs, 3);
			gr->subblock_gain[1] = (uint8_t)get_bits(bs, 3);
			gr->subblock_gain[2] = (uint8_t)get_bits(bs, 3);
		}
		else
		{
			gr->block_type = 0;
			gr->mixed_block_flag = 0;
			tables = get_bits(bs, 15);
			gr->region_count[0] = (uint8_t)get_bits(bs, 4);
			gr->region_count[1] = (uint8_t)get_bits(bs, 3);
			gr->region_count[2] = 255;
		}
		gr->table_select[0] = (uint8_t)(tables >> 10);
		gr->table_select[1] = (uint8_t)((tables >> 5) & 31);
		gr->table_select[2] = (uint8_t)((tables) & 31);
		gr->preflag = HDR_TEST_MPEG1(hdr) ? get_bits(bs, 1) : (gr->scalefac_compress >= 500);
		gr->scalefac_scale = (uint8_t)get_bits(bs, 1);
		gr->count1_table = (uint8_t)get_bits(bs, 1);
		gr->scfsi = (uint8_t)((scfsi >> 12) & 15);
		scfsi <<= 4;
		gr++;
	} while (--gr_count);

	if (part_23_sum + bs->pos > bs->limit + main_data_begin * 8)
	{
		return -1;
	}

	return main_data_begin;
}



int mp3d_match_frame(const uint8_t *hdr, int mp3_bytes, int frame_bytes)
{
	int i, nmatch;
	for (i = 0, nmatch = 0; nmatch < MAX_FRAME_SYNC_MATCHES; nmatch++)
	{
		i += hdr_frame_bytes(hdr + i, frame_bytes) + hdr_padding(hdr + i);
		if (i + HDR_SIZE > mp3_bytes)
			return nmatch > 0;
		if (!hdr_compare(hdr, hdr + i))
			return 0;
	}
	return 1;
}

int mp3d_find_frame(const uint8_t *mp3, int mp3_bytes, int *free_format_bytes, int *ptr_frame_bytes)
{
	int i, k;
	for (i = 0; i < mp3_bytes - HDR_SIZE; i++, mp3++)
	{
		if (hdr_valid(mp3))
		{
			int frame_bytes = hdr_frame_bytes(mp3, *free_format_bytes);
			int frame_and_padding = frame_bytes + hdr_padding(mp3);

			for (k = HDR_SIZE; !frame_bytes && k < MAX_FREE_FORMAT_FRAME_SIZE && i + 2 * k < mp3_bytes - HDR_SIZE; k++)
			{
				if (hdr_compare(mp3, mp3 + k))
				{
					int fb = k - hdr_padding(mp3);
					int nextfb = fb + hdr_padding(mp3 + k);
					if (i + k + nextfb + HDR_SIZE > mp3_bytes || !hdr_compare(mp3, mp3 + k + nextfb))
						continue;
					frame_and_padding = k;
					frame_bytes = fb;
					*free_format_bytes = fb;
				}
			}
			if ((frame_bytes && i + frame_and_padding <= mp3_bytes &&
				mp3d_match_frame(mp3, mp3_bytes - i, frame_bytes)) ||
				(!i && frame_and_padding == mp3_bytes))
			{
				*ptr_frame_bytes = frame_and_padding;
				return i;
			}
			*free_format_bytes = 0;
		}
	}
	*ptr_frame_bytes = 0;
	return mp3_bytes;
}

void mp3dec_init(mp3dec_t *dec)
{
	dec->header[0] = 0;
}


void mp3dec_skip_id3v1(const uint8_t *buf, size_t *pbuf_size)
{
	size_t buf_size = *pbuf_size;

	if (buf_size >= 128 && !memcmp(buf + buf_size - 128, "TAG", 3))
	{
		buf_size -= 128;
		if (buf_size >= 227 && !memcmp(buf + buf_size - 227, "TAG+", 4))
			buf_size -= 227;
	}

	if (buf_size > 32 && !memcmp(buf + buf_size - 32, "APETAGEX", 8))
	{
		buf_size -= 32;
		const uint8_t *tag = buf + buf_size + 8 + 4;
		uint32_t tag_size = (uint32_t)(tag[3] << 24) | (tag[2] << 16) | (tag[1] << 8) | tag[0];
		if (buf_size >= tag_size)
			buf_size -= tag_size;
	}

	*pbuf_size = buf_size;
}

size_t mp3dec_skip_id3v2(const uint8_t *buf, size_t buf_size)
{
#define MINIMP3_ID3_DETECT_SIZE 10

	if (buf_size >= MINIMP3_ID3_DETECT_SIZE && !memcmp(buf, "ID3", 3) && !((buf[5] & 15) || (buf[6] & 0x80) || (buf[7] & 0x80) || (buf[8] & 0x80) || (buf[9] & 0x80)))
	{
		size_t id3v2size = (((buf[6] & 0x7f) << 21) | ((buf[7] & 0x7f) << 14) | ((buf[8] & 0x7f) << 7) | (buf[9] & 0x7f)) + 10;
		if ((buf[5] & 16))
			id3v2size += 10; /* footer */
		return id3v2size;
	}
	return 0;
}

size_t mp3dec_skip_id3(const uint8_t **pbuf, size_t *pbuf_size)
{
	uint8_t *buf = (uint8_t *)(*pbuf);
	size_t buf_size = *pbuf_size;
	size_t id3v2size = mp3dec_skip_id3v2(buf, buf_size);
	if (id3v2size)
	{
		if (id3v2size >= buf_size)
			id3v2size = buf_size;
		buf += id3v2size;
		buf_size -= id3v2size;
	}
	mp3dec_skip_id3v1(buf, &buf_size);
	*pbuf = (const uint8_t *)buf;
	*pbuf_size = buf_size;

	return id3v2size;
}

int mp3dec_check_vbrtag(const uint8_t *frame, int frame_size, uint32_t *frames, int *delay, int *padding)
{
	const char g_xing_tag[4] = { 'X', 'i', 'n', 'g' };
	const char g_info_tag[4] = { 'I', 'n', 'f', 'o' };
#define FRAMES_FLAG     1
#define BYTES_FLAG      2
#define TOC_FLAG        4
#define VBR_SCALE_FLAG  8
	/* Side info offsets after header:
	/                Mono  Stereo
	/  MPEG1          17     32
	/  MPEG2 & 2.5     9     17*/
	bs_t bs[1];
	L3_gr_info_t gr_info[4];
	bs_init(bs, frame + HDR_SIZE, frame_size - HDR_SIZE);
	if (HDR_IS_CRC(frame))
		get_bits(bs, 16);
	if (L3_read_side_info(bs, gr_info, frame) < 0)
		return 0; /* side info corrupted */

	const uint8_t *tag = frame + HDR_SIZE + bs->pos / 8;
	if (memcmp(g_xing_tag, tag, 4) && memcmp(g_info_tag, tag, 4))
		return 0;
	int flags = tag[7];
	if (!((flags & FRAMES_FLAG)))
		return -1;
	tag += 8;
	*frames = (uint32_t)(tag[0] << 24) | (tag[1] << 16) | (tag[2] << 8) | tag[3];
	tag += 4;
	if (flags & BYTES_FLAG)
		tag += 4;
	if (flags & TOC_FLAG)
		tag += 100;
	if (flags & VBR_SCALE_FLAG)
		tag += 4;
	*delay = *padding = 0;
	if (*tag)
	{   /* extension, LAME, Lavc, etc. Should be the same structure. */
		tag += 21;
		if (tag - frame + 14 >= frame_size)
			return 0;
		*delay = ((tag[0] << 4) | (tag[1] >> 4)) + (528 + 1);
		*padding = (((tag[1] & 0xF) << 8) | tag[2]) - (528 + 1);
	}
	return 1;
}


char* memstr(char* full_data, int full_data_len, char* substr)
{
	if (full_data == NULL || full_data_len <= 0 || substr == NULL) {
		return NULL;
	}

	if (*substr == '\0') {
		return NULL;
	}

	int sublen = strlen(substr);

	int i;
	char* cur = full_data;
	int last_possible = full_data_len - sublen + 1;
	for (i = 0; i < last_possible; i++) {
		if (*cur == *substr) {
			//assert(full_data_len - i >= sublen);
			if (memcmp(cur, substr, sublen) == 0) {
				//found
				return cur;
			}
		}
		cur++;
	}

	return NULL;
}

int mp3dec_load_cb(mp3dec_t *dec, uint8_t *buf, size_t buf_size, mp3dec_file_info_t *info)
{
	if (!dec || !buf || !info || (size_t)-1 == buf_size)
		return MP3D_E_PARAM;
	size_t orig_buf_size = buf_size;
	int to_skip = 0;
	mp3dec_frame_info_t frame_info;
	memset(info, 0, sizeof(*info));
	memset(&frame_info, 0, sizeof(frame_info));

	/* skip id3 */
	size_t filled = 0, consumed = 0;
	int eof = 0, ret = 0;


	size_t id3v2size = mp3dec_skip_id3((const uint8_t **)&buf, &buf_size);
	if (!buf_size)
		return 0;

	if (id3v2size > 0)
	{
		long filesize = 0;
		char *p = memstr((char*)(buf - id3v2size), id3v2size, "APIC");
		if (p != NULL)
		{
			unsigned char * code = (unsigned char *)p + 4;
			filesize = code[0] * 0x1000000
				+ code[1] * 0x10000
				+ code[2] * 0x100
				+ code[3] * 0x1;

			if (filesize < id3v2size)
			{
				char sub[3] = { 0xff,0xd8,0 };
				char *pt = sub;
				char *p1 = memstr(p + 10, filesize, pt);
				if (p1 != NULL)
				{
					filesize -= p1 - p - 10;

					info->buffer_size = filesize;
					info->buffer=p1;
				}
			}
		}
	}

	mp3dec_init(dec);
	int samples, frame_len;
		uint32_t frames =0;
	do
	{
		int i, delay, padding, free_format_bytes = 0, frame_size = 0;
		const uint8_t *hdr;

		{
			i = mp3d_find_frame(buf, buf_size, &free_format_bytes, &frame_size);
			buf += i;
			buf_size -= i;
			hdr = buf;
		}
		if (i && !frame_size)
			continue;
		if (!frame_size)
			return 0;
		frame_info.channels = HDR_IS_MONO(hdr) ? 1 : 2;
		frame_info.hz = hdr_sample_rate_hz(hdr);
		frame_info.layer = 4 - HDR_GET_LAYER(hdr);
		frame_info.bitrate_kbps = hdr_bitrate_kbps(hdr);
		frame_info.frame_bytes = frame_size;
		frame_len = hdr_frame_samples(hdr);
		samples = frame_len*frame_info.channels;
		if (3 != frame_info.layer)
			break;
		int ret = mp3dec_check_vbrtag(hdr, frame_size, &frames, &delay, &padding);
		if (ret > 0)
		{
			uint64_t detected_samples = frames;
			padding *= frame_info.channels;
			to_skip = delay*frame_info.channels;
			detected_samples = samples*(uint64_t)frames;
			if (detected_samples >= (uint64_t)to_skip)
				detected_samples -= to_skip;
			if (padding > 0 && detected_samples >= (uint64_t)padding)
				detected_samples -= padding;
			if (!detected_samples)
				return 0;
		}
		if (ret)
		{
			buf += frame_size;
			buf_size -= frame_size;
		}
		break;
	} while (1);

	info->samples = buf_size / frame_info.frame_bytes;


	if (frames != 0)
	{
			info->samples =frames;
	}
	else
	{
		int k = 1;
		do
		{
			int i, free_format_bytes = 0, frame_size = 0;

			i = mp3d_find_frame(buf, buf_size, &free_format_bytes, &frame_size);
			if (!frame_size)
			{
				break;
			}

			buf += frame_size;
			buf_size -= frame_size;

			k++;
		} while (1);

		info->samples = k;
	}

	info->frame_len = frame_len;
	info->bitrate_kbps = frame_info.bitrate_kbps;
	info->channels = frame_info.channels;
	info->hz = frame_info.hz;
	info->layer = frame_info.layer;

	return ret;
}

int mp3dec_load_buf(mp3dec_t *dec, const uint8_t *buf, size_t buf_size, mp3dec_file_info_t *info)
{
	return mp3dec_load_cb(dec, (uint8_t *)buf, buf_size, info);
}

void mp3dec_close_file(mp3dec_map_info_t *map_info)
{
	if (map_info->buffer)
		UnmapViewOfFile(map_info->buffer);
	map_info->buffer = 0;
	map_info->size = 0;
}

int mp3dec_open_file_h(HANDLE file, mp3dec_map_info_t *map_info)
{
	memset(map_info, 0, sizeof(*map_info));

	HANDLE mapping = NULL;
	LARGE_INTEGER s;
	s.LowPart = GetFileSize(file, (DWORD*)&s.HighPart);
	if (s.LowPart == INVALID_FILE_SIZE && GetLastError() != NO_ERROR)
		goto error;
	map_info->size = s.QuadPart;

	mapping = CreateFileMapping(file, NULL, PAGE_READONLY, 0, 0, NULL);
	if (!mapping)
		goto error;
	map_info->buffer = (const uint8_t*)MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, s.QuadPart);
	CloseHandle(mapping);
	if (!map_info->buffer)
		goto error;

	CloseHandle(file);
	return 0;
error:
	mp3dec_close_file(map_info);
	CloseHandle(file);
	return MP3D_E_IOERROR;
}

int mp3dec_open_file(const wchar_t *file_name, mp3dec_map_info_t *map_info)
{
	if (!file_name)
		return MP3D_E_PARAM;
	HANDLE file = CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	if (INVALID_HANDLE_VALUE == file)
		return MP3D_E_IOERROR;
	return mp3dec_open_file_h(file, map_info);
}

#endif
