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

void encrypt_desencrypt_data(CONF*, const unsigned char*, const unsigned char*, unsigned char*, unsigned int);


#endif /* MICKEY_H_ */
