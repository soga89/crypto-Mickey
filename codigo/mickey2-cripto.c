#include<stdio.h>
#include<string.h>

int RTAPS[] = {0, 1, 3, 4, 5, 6, 9, 12, 13, 16, 19, 20, 21, 22, 25, 28, 37, 38, 41, 42, 45, 46, 50, 52, 54, 56, 58,60, 61, 63, 64, 65, 66, 67, 71, 72, 79, 80, 81, 82, 87, 88, 89, 90, 91, 92, 94, 95, 96, 97};
int SIZE_RTAPS = (int)(sizeof(RTAPS) / sizeof(RTAPS[0]));


int COMP0[] = {'\0',0,0,0,1,1,0,0,0,1,0,1,1,1,1,0,1,0,0,1,0,1,0,1,0,1,0,1,0,1,1,0,1,0,0,1,0,0,0,0,0,0,0,1,0,1,0,1,0,1,0,0,0,0,1,0,1,0,0,1,1,1,1,0,0,1,0,1,0,1,1,1,1,1,1,1,1,1,0,1,0,1,1,1,1,1,1,0,1,0,1,0,0,0,0,0,0,1,1,'\0'};
int SIZE_COMP0 = (int)(sizeof(COMP0) / sizeof(COMP0[0]));

int COMP1[] = {'\0',1,0,1,1,0,0,1,0,1,1,1,1,0,0,1,0,1,0,0,0,1,1,0,1,0,1,1,1,0,1,1,1,1,0,0,0,1,1,0,1,0,1,1,1,0,0,0,0,1,0,0,0,1,0,1,1,1,0,0,0,1,1,1,1,1,1,0,1,0,1,1,1,0,1,1,1,1,0,0,0,1,0,0,0,0,1,1,1,0,0,0,1,0,0,1,1,0,0,'\0'};
int SIZE_COMP1 = (int)(sizeof(COMP1) / sizeof(COMP1[0]));

int FB0[] = {1,1,1,1,0,1,0,1,1,1,1,1,1,1,1,0,0,1,0,1,1,1,1,1,1,1,1,1,1,0,0,1,1,0,0,0,0,0,0,1,1,1,0,0,1,0,0,1,0,1,0,1,0,0,1,0,1,1,1,1,0,1,0,1,0,1,0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0,1,1,0,1,1,1,0,0,1,1,1,0,0,1,1,0,0,0};
int SIZE_FB0 = (int)(sizeof(FB0) / sizeof(FB0[0]));

int FB1[] = {1,1,1,0,1,1,1,0,0,0,0,1,1,1,0,1,0,0,1,1,0,0,0,1,0,0,1,1,0,0,1,0,1,1,0,0,0,1,1,0,0,0,0,0,1,1,0,1,1,0,0,0,1,0,0,0,1,0,0,1,0,0,1,0,1,1,0,1,0,1,0,0,1,0,1,0,0,0,1,1,1,1,0,1,1,1,1,1,0,0,0,0,0,0,1,0,0,0,0,1};
int SIZE_FB1 = (int)(sizeof(FB1) / sizeof(FB1[0]));


// Se declara la llave y el vector de inicializacion que por ahora tendran 10 y 4 caracteres
int KEY[] ={1,1,1,1,0,1,0,1,0,1};
int IV[] ={1,1,0,1};

// Registros que generaran la clave, son de 100 posiciones
int R[100],S[100];

// Vector donde se almacenara el resultado de algoritmo
int Z[100];


int xor(int a, int b){return a == b ? 0 : 1;}

int and(int a, int b){return (a == 1 && b == 1) ? 1 : 0;}

int incluido_en_RTAPS(int valor){
	for(int i = 0; i < SIZE_RTAPS; i++){
		if(valor == RTAPS[i])
			return 1;
	}
	return 0;
}

void inicializar_vector(int vector[], int size){
	for(int i = 0 ; i < size ; i ++){
		vector[i] = 0 ;
	}
}

void clock_r(int r[], int r_entrada, int r_control){
	int FEEDBACK_BIT = xor(r[99],r_entrada);
	int r1[100], k = 0;
	r1[0] = 0;
	for(k = 1; k < 100; k++){
		r1[k] = r[k-1];
	}
	for(k = 0; k < 100 ; k++){
		if(incluido_en_RTAPS(k)){
			r1[k] = xor(r1[k],FEEDBACK_BIT);
		}
	}
	if(r_control){
		for(k = 0; k < 100; k++){
			r1[k] = xor(r1[k],r[k]);
		}
	}
	r = r1;
}

void clock_s(int s[], int s_entrada, int s_control){
	int FEEDBACK_BIT = xor(s[99], s_entrada);
	int s1[100], k = 0;
	s1[0] = 0;
	s1[99] = s[98];
	for(k = 1; k < 99 ; k++){
		s1[k] = xor( s[k-1], and(xor(s[k], COMP0[k]) , xor(s[k+1], COMP1[k])));
	}

	if(!s_control){
		for(k = 0; k < 100; k++){
			s1[k] = xor(s1[k], and(FB1[k], FEEDBACK_BIT));
		}
	}
	s = s1;
}

void clock_kg(int r[], int s[], int MIXING, int entrada){

	int CONTROL_R = xor(s[34], r[67]);
	int CONTROL_S = xor(s[67], r[33]);
	int ENTRADA_R, ENTRADA_S = 0;

	if(MIXING){
		ENTRADA_R = xor(entrada, s[50]);
	}else{
		ENTRADA_R = entrada;
	}
	ENTRADA_S = entrada;
	clock_r(r, ENTRADA_R, CONTROL_R);
	clock_s(s, ENTRADA_S, CONTROL_S);
}

void key_load_initialization(){
	for(int i = 0; i <4; i++){
		clock_kg(R,S,1,IV[i]);
	}
	
	for(int i = 0; i <10; i++){
		clock_kg(R,S,1,KEY[i]);
	}
	
	for(int i = 0; i <100; i++){
		clock_kg(R,S,1,0);
	}
}

void keystream_generation(){
	for(int j = 0; j< 99; j++){
		Z[j] = xor(R[0],S[0]);
		clock_kg(R,S,0,0);
	}
}


void test_funciones(){
	printf("El valor del xor es %d\n", xor(1,0));
	printf("El valor del and es %d\n", and(1,1));
	printf("tamaño del array %d\n", SIZE_RTAPS);
	printf("tamaño del array %d\n", SIZE_COMP0);
	printf("tamaño del array %d\n", SIZE_COMP1);
	printf("tamaño del array %d\n", incluido_en_RTAPS(14));
}

int main(){

	// INICIALIZACION 

	inicializar_vector(R, 100);
	inicializar_vector(S, 100);
	inicializar_vector(Z, 100);
	
	// test_funciones();

	// Se deben ingresar los vectores para el key y el vector de inicializacion.
	key_load_initialization();

	// Se aplica el algoritmo
	keystream_generation();

	return 0;
}

