#include <stdio.h>
#include <fcntl.h>
//#include <sys/mman.h>
#include <string.h>

#include "mickey.h"


void strip_ext(char *fname)
{
    char *end = fname + strlen(fname);

    while (end > fname && *end != '.' && *end != '\\' && *end != '/') {
        --end;
    }
    if ((end > fname && *end == '.') &&
        (*(end - 1) != '\\' && *(end - 1) != '/')) {
        *end = '\0';
    }
}

int main(int argc, char** argv) {

	CONF conf;
	char *filename = argv[1];


	unsigned char key[10] = { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0x12, 0x34 };
	unsigned char iv[10] = { 0x9c, 0x53, 0x2f, 0x8a, 0xc3, 0xea, 0x4b, 0x2e, 0xa0, 0xf5 };

	init_regs();

	setear_key(&conf, key, 80, 32);

	FILE *exein, *exeout;

	exein = fopen(filename, "rb");
	if (exein == NULL) {
	    perror("error al abrir el archivo");
	    return 0;
	}

	char modify_filename[100];
	strcpy(modify_filename, filename);
	strip_ext(modify_filename);
	strcat(modify_filename, "_mod.bmp");

	exeout = fopen(modify_filename, "wb");
	if (exeout == NULL) {
	    perror("error al crear o abrir el archivo a modificar");
	    return 0;
	}

	setear_iv(conf, iv);

	unsigned char* copiar(unsigned char* m){
		unsigned char *a;
			for (int j = 0; j < 8; j++)
				a = ((int) a) ^ CLOCK_KG(conf, 0, 0) << (7 - j);
		return a;
	}

	fseek(exein, 0L, SEEK_END);
	size_t tama = ftell(exein);

	fseek(exein, 0L, SEEK_SET);
	rewind(exein);

	size_t n, m;
	unsigned char buff[tama];
	unsigned char* aux = malloc(tama);

    fread(buff, 1, 54, exein);
    fwrite(buff,1,54, exeout);

	do {
	    n = fread(buff, 1, sizeof buff - 54, exein);

	    if (n) {
	    		encrypt_desencrypt_data(&conf, &iv, buff, aux, tama);
	    		m = fwrite(aux, 1, n, exeout);
	    }
	    else   m = 0;

	} while ((n > 0) && (n == m));
	if (m) perror("copy");


	if (fclose(exeout)) perror("error al cerrar el archivo");
	if (fclose(exein)) perror("error al cerrar el archivo");

	puts("Se proceso correctamente");

	/*CONF conf;
	char *filename = argv[1];
	int file_origen, file_destino;
	char *puntero_origen, *puntero_destino, *aux_o, *aux_d;
	size_t tamanio;

	file_origen = open(filename, O_RDONLY);

	if(file_origen == -1){
		printf(" No se encontro el archivo buscado.");
		return 0;
	}
	char modify_filename[100];
	strcpy(modify_filename, filename);
	strip_ext(modify_filename);
	strcat(modify_filename, "_mod.bmp");

	tamanio = lseek(file_origen, 0, SEEK_END);

	puntero_origen = mmap(NULL, tamanio, PROT_READ, MAP_PRIVATE, file_origen, 0);

	file_destino = open(modify_filename, O_RDWR | O_CREAT, 0666);

	ftruncate(file_destino, tamanio);

	puntero_destino = mmap(NULL, tamanio, PROT_READ | PROT_WRITE, MAP_SHARED, file_destino, 0);

	memcpy(puntero_destino, puntero_origen, 54);

	//AQUI DEBEMOS HAY QUE SETEAR EL KEY Y EL VECTOR INICIALIZADOR

	//TODO: Se debe hacer modificaciones para que se pasen los vectores de la llave y vector inicializacion
	unsigned char key[10] = { 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0, 0x12, 0x34 };
	unsigned char iv[10] = { 0x9c, 0x53, 0x2f, 0x8a, 0xc3, 0xea, 0x4b, 0x2e, 0xa0, 0xf5 };

	init_regs();

	setear_key(&conf, key, 80, 32);

	aux_o = puntero_origen;
	aux_o += 54;
	aux_d = puntero_destino;
	aux_d += 54;

	encrypt_desencrypt_data(&conf, &iv, aux_o, aux_d, tamanio-54);


	munmap(puntero_origen, tamanio);
	munmap(puntero_destino, tamanio);

	close(file_origen);
	close(file_destino);
	printf("Proceso realizado.\n");
*/
	return 0;
}
