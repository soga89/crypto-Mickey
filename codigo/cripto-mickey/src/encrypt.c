#include "mickey.h"

unsigned int R_Mask[4];
unsigned int Comp0[4];
unsigned int Comp1[4];
unsigned int S_Mask0[4];
unsigned int S_Mask1[4];

void init_regs(void) {

	R_Mask[0] = 0x1279327b;
	R_Mask[1] = 0xb5546660;
	R_Mask[2] = 0xdf87818f;
	R_Mask[3] = 0x00000003;

	Comp0[0] = 0x6aa97a30;
	Comp0[1] = 0x7942a809;
	Comp0[2] = 0x057ebfea;
	Comp0[3] = 0x00000006;

	Comp1[0] = 0xdd629e9a;
	Comp1[1] = 0xe3a21d63;
	Comp1[2] = 0x91c23dd7;
	Comp1[3] = 0x00000001;

	S_Mask0[0] = 0x9ffa7faf;
	S_Mask0[1] = 0xaf4a9381;
	S_Mask0[2] = 0x9cec5802;
	S_Mask0[3] = 0x00000001;

	S_Mask1[0] = 0x4c8cb877;
	S_Mask1[1] = 0x4911b063;
	S_Mask1[2] = 0x40fbc52b;
	S_Mask1[3] = 0x00000008;
}

/* The following routine clocks register R in conf with given input and control bits */

void CLOCK_R(CONF *conf, int input_bit, int control_bit) {
	int Feedback_bit;
	int Carry0, Carry1, Carry2;

	Feedback_bit = ((conf->R[3] >> 3) & 1) ^ input_bit;
	Carry0 = (conf->R[0] >> 31) & 1;
	Carry1 = (conf->R[1] >> 31) & 1;
	Carry2 = (conf->R[2] >> 31) & 1;

	if (control_bit) {
		conf->R[0] ^= (conf->R[0] << 1);
		conf->R[1] ^= (conf->R[1] << 1) ^ Carry0;
		conf->R[2] ^= (conf->R[2] << 1) ^ Carry1;
		conf->R[3] ^= (conf->R[3] << 1) ^ Carry2;
	} else {
		conf->R[0] = (conf->R[0] << 1);
		conf->R[1] = (conf->R[1] << 1) ^ Carry0;
		conf->R[2] = (conf->R[2] << 1) ^ Carry1;
		conf->R[3] = (conf->R[3] << 1) ^ Carry2;
	}

	if (Feedback_bit) {
		conf->R[0] ^= R_Mask[0];
		conf->R[1] ^= R_Mask[1];
		conf->R[2] ^= R_Mask[2];
		conf->R[3] ^= R_Mask[3];
	}
}

void CLOCK_S(CONF *conf, int input_bit, int control_bit) {
	int Feedback_bit;
	int Carry0, Carry1, Carry2;

	Feedback_bit = ((conf->S[3] >> 3) & 1) ^ input_bit;
	Carry0 = (conf->S[0] >> 31) & 1;
	Carry1 = (conf->S[1] >> 31) & 1;
	Carry2 = (conf->S[2] >> 31) & 1;

	conf->S[0] = (conf->S[0] << 1)
			^ ((conf->S[0] ^ Comp0[0])
					& ((conf->S[0] >> 1) ^ (conf->S[1] << 31) ^ Comp1[0])
					& 0xfffffffe);
	conf->S[1] = (conf->S[1] << 1)
			^ ((conf->S[1] ^ Comp0[1])
					& ((conf->S[1] >> 1) ^ (conf->S[2] << 31) ^ Comp1[1]))
			^ Carry0;
	conf->S[2] = (conf->S[2] << 1)
			^ ((conf->S[2] ^ Comp0[2])
					& ((conf->S[2] >> 1) ^ (conf->S[3] << 31) ^ Comp1[2]))
			^ Carry1;
	conf->S[3] = (conf->S[3] << 1)
			^ ((conf->S[3] ^ Comp0[3]) & ((conf->S[3] >> 1) ^ Comp1[3]) & 0x7)
			^ Carry2;

	if (Feedback_bit) {
		if (control_bit) {
			conf->S[0] ^= S_Mask1[0];
			conf->S[1] ^= S_Mask1[1];
			conf->S[2] ^= S_Mask1[2];
			conf->S[3] ^= S_Mask1[3];
		} else {
			conf->S[0] ^= S_Mask0[0];
			conf->S[1] ^= S_Mask0[1];
			conf->S[2] ^= S_Mask0[2];
			conf->S[3] ^= S_Mask0[3];
		}
	}
}

int CLOCK_KG(CONF *conf, int mixing, int input_bit) {
	int Keystream_bit;
	int control_bit_r;
	int control_bit_s;

	Keystream_bit = (conf->R[0] ^ conf->S[0]) & 1;
	control_bit_r = ((conf->S[1] >> 2) ^ (conf->R[2] >> 3)) & 1;
	control_bit_s = ((conf->R[1] >> 1) ^ (conf->S[2] >> 3)) & 1;

	if (mixing)
		CLOCK_R(conf, ((conf->S[1] >> 18) & 1) ^ input_bit, control_bit_r);
	else
		CLOCK_R(conf, input_bit, control_bit_r);
	CLOCK_S(conf, input_bit, control_bit_s);

	return Keystream_bit;
}

void setear_key(CONF *conf, const unsigned char *key,	unsigned int keysize, unsigned int ivsize) {
	int i;

	for (i = 0; i < 10; i++)
		conf->key[i] = key[i];

	conf->ivsize = ivsize;
}

void setear_iv(CONF *conf, const unsigned char *iv) {
	int i;
	int iv_or_key_bit;

	for (i = 0; i < 4; i++) {
		conf->R[i] = 0;
		conf->S[i] = 0;
	}

	for (i = 0; i < conf->ivsize; i++) {
		iv_or_key_bit = (iv[i / 8] >> (7 - (i % 8))) & 1;
		CLOCK_KG(conf, 1, iv_or_key_bit);
	}

	for (i = 0; i < 80; i++) {
		iv_or_key_bit = (conf->key[i / 8] >> (7 - (i % 8))) & 1;
		CLOCK_KG(conf, 1, iv_or_key_bit);
	}

	for (i = 0; i < 100; i++)
		CLOCK_KG(conf, 1, 0);
}

void encrypt_desencrypt_data(CONF *conf, const unsigned char *iv, const unsigned char *input, unsigned char *output, unsigned int msglen)
{
	setear_iv(conf, iv);
	unsigned int i, j;

	for (i = 0; i < msglen; i++) {
		output[i] = input[i];

		for (j = 0; j < 8; j++)
			output[i] ^= CLOCK_KG(conf, 0, 0) << (7 - j);
	}
}

