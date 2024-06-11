#include "Encoding.h"

static const char hex_digits[17] = "0123456789abcdef";

/*------ Base64 Encoding Table ------*/
static const char s_encode_base64[] = {
	'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
	'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
	'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
	'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
	'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
	'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
	'w', 'x', 'y', 'z', '0', '1', '2', '3',
	'4', '5', '6', '7', '8', '9', '+', '/'
};
static int s_decode_base64[256] =
{
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 00-0F */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 10-1F */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,  /* 20-2F */
	52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,  /* 30-3F */
	-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,  /* 40-4F */
	15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,  /* 50-5F */
	-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,  /* 60-6F */
	41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1,  /* 70-7F */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 80-8F */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* 90-9F */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* A0-AF */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* B0-BF */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* C0-CF */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* D0-DF */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,  /* E0-EF */
	-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1   /* F0-FF */
};

int base16_encode_size(const char* src)
{
	return (int)(strlen(src) * 2);
}

void base16_encode(char* dst, const unsigned char* src, size_t src_length)
{
	while (src_length--)
	{
		*dst++ = hex_digits[*src >> 4];
		*dst++ = hex_digits[*src++ & 0x0f];
	}
	*dst = '\0';
}

int base16_decode_size(const char* src, size_t src_length)
{
	return (int)(src_length / 2);
}

int base16_decode(char* dst, const char* src, size_t src_length)
{
	unsigned char t[2]{};

	for (unsigned int i = 0; i < src_length; i += 2)
	{
		for (unsigned int d = 0; d < 2; d++)
		{
			if ((src[i + d] >= '0' && src[i + d] <= '9') ||
				(src[i + d] >= 'a' && src[i + d] <= 'f')) {
				for (t[d] = 0; src[i + d] != hex_digits[t[d]]; ++t[d]); /* get position */
			}
			else
			{
				throw("error: non alphabet character found on encoded stream");
			}
		}
		t[0] <<= 4;
		t[0] |= t[1];
		*dst++ = t[0];
	}
	return (int)(src_length >> 1);
}

int base64_encode_size(size_t length)
{
	return (4 * (length / 3)) + (length % 3 ? 4 : 0) + 1;
}

void base64_encode(char* dst, const unsigned char* src, size_t src_length)
{
	unsigned char input[3] = { 0,0,0 };
	unsigned char output[4] = { 0,0,0,0 };

	int index, i, j, size;
	const unsigned char* p;
	const unsigned char* plen;

	plen = src + src_length - 1;
	size = (int)((4 * (src_length / 3)) + (src_length % 3 ? 4 : 0) + 1);
	//(*dst) = (char*)malloc(size);
	j = 0;
	for (i = 0, p = src; p <= plen; i++, p++) {
		index = i % 3;
		input[index] = *p;
		if (index == 2 || p == plen) {
			output[0] = ((input[0] & 0xFC) >> 2);
			output[1] = ((input[0] & 0x3) << 4) | ((input[1] & 0xF0) >> 4);
			output[2] = ((input[1] & 0xF) << 2) | ((input[2] & 0xC0) >> 6);
			output[3] = (input[2] & 0x3F);
			dst[j++] = s_encode_base64[output[0]];
			dst[j++] = s_encode_base64[output[1]];
			dst[j++] = index == 0 ? '=' : s_encode_base64[output[2]];
			dst[j++] = index < 2 ? '=' : s_encode_base64[output[3]];
			input[0] = input[1] = input[2] = 0;
		}
	}

	assert(size - 1 == j, "Missmatch size");
	dst[j] = '\0';
}

int base64_decode_size(const char* code, int len)
{
	int real_code_len = 0;
	if (!code)
		return 0;

	while (real_code_len < len && s_decode_base64[code[real_code_len]] != -1)
	{
		real_code_len++;
	}

	return real_code_len;
}

int base64_decode(char* dst, const char* src, int src_length)
{
	const char* cp;
	int space_idx = 0, phase;
	int d, prev_d = 0;
	unsigned char c;
	space_idx = 0;
	phase = 0;

	for (cp = src; *cp != '\0'; ++cp) {
		d = s_decode_base64[(int)*cp];
		if (d != -1) {
			switch (phase) {
			case 0:
				++phase;
				break;
			case 1:
				c = ((prev_d << 2) | ((d & 0x30) >> 4));
				if (space_idx < src_length)
					dst[space_idx++] = c;
				++phase;
				break;
			case 2:
				c = (((prev_d & 0xf) << 4) | ((d & 0x3c) >> 2));
				if (space_idx < src_length)
					dst[space_idx++] = c;
				++phase;
				break;
			case 3:
				c = (((prev_d & 0x03) << 6) | d);
				if (space_idx < src_length)
					dst[space_idx++] = c;
				phase = 0;
				break;
			}
			prev_d = d;
		}
	}

	return space_idx;
}

int base64_encode_size(int len)
{
	return ((len + 2) / 3 * 4);
}

int base64_decode_size(int len)
{
	return (len / 4) * 3 - 2;
}
