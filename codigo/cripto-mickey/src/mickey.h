/*
 * mickey.h
 *
 *  Created on: Jul 1, 2019
 *      Author: aldo
 */

#ifndef MICKEY_H_
#define MICKEY_H_

typedef struct{
    unsigned int R[4];
    unsigned int S[4];
    unsigned char key[10];
    int ivsize;
} CONF;

void init_regs();
void setear_key(CONF *conf, const unsigned char *key, unsigned int keysize, unsigned int ivsize);
//void setear_iv(CONF *conf, const unsigned char *iv) ;

void encrypt_desencrypt_data(CONF*, const unsigned char*, const unsigned char*, unsigned char*, unsigned int);


#endif /* MICKEY_H_ */
