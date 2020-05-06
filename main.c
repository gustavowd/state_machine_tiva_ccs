#include "sm.h"
#include <stdio.h>
#include <string.h>


/* main para simular o uso da maquina de estados */
unsigned char data1[] = { 7, 8, 4, STX, 0xBB, 0xAA, 5, 't', 'e'};
unsigned char data2[] = { 's', 't', 'e', 0x8F, ETX };

unsigned char data3[] = { 7, 8, 4, STX, 0xBB, 0xAA, 5, 't', 'e', 's', 't', 'e', 0x8A, ETX};

void Handler(unsigned char* string){
    printf("Imprimindo dados recebidos...\n\r");
    printf("%s\n\r", string);
}

void Error(unsigned char* string){
    printf("%s\n\r", string);
}

int main(void)
{
	sm_t sm;
	sm_t sm2;
    initSM(&sm, 43707, Handler, Error);
    initSM(&sm2, 13500, Handler, Error);

    while(1){
		ExecSM(&sm, data1, sizeof(data1));
		ExecSM(&sm2, data1, sizeof(data1));

		ExecSM(&sm, data2, sizeof(data2));
		ExecSM(&sm2, data2, sizeof(data2));

		ExecSM(&sm, data3, sizeof(data3));
    }
}


#if 0

#define SWITCH  0
#define POINTER 1

#define STATE  POINTER


#if (STATE == SWITCH)

#include <stdio.h>

/* numero maximo de bytes do buffer de dados */
#define MAX_BUFFER 512

/* constantes usadas na comunicacao */
#define STX     0x02
#define ETX     0x03

/* possiveis estados da maquina de estados de comunicacao */
typedef enum {
    ST_STX = 0, ST_QTD, ST_DATA, ST_CHK, ST_ETX
} States;


/* trata dados recebidos */
void handlePackage(unsigned char *data, int qtd)
{
    //int i;

    /*
    printf("Imprimindo dados recebidos...\n");

    for (i = 0; i < qtd; i++)
        printf("Data[%d]=%d\n", i, data[i]);

    */
}


/* Implementa��o da m�quina de estados */
void handleRx(unsigned char *data, int qtd)
{
    static States state = ST_STX;
    static unsigned char buffer[MAX_BUFFER];
    static int indBuffer = 0, qtdBuffer = 0;
    static unsigned char chkBuffer = 0;
    int i;

    for (i = 0; i < qtd; i++) {

        switch (state) {

            case ST_STX:
                if (data[i] == STX) {
                    indBuffer = qtdBuffer = chkBuffer = 0;
                    state = ST_QTD;
                }
                break;

            case ST_QTD:
                qtdBuffer = data[i];
                state = ST_DATA;
                break;

            case ST_DATA:
                buffer[indBuffer++] = data[i];
                chkBuffer ^= data[i];
                if (--qtdBuffer == 0) {
                    state = ST_CHK;
                }
                break;

            case ST_CHK:
                if (data[i] == chkBuffer) {
                    state = ST_ETX;
                }
                else {
                    state = ST_STX;
                }
                break;

            case ST_ETX:
                if (data[i] == ETX) {
                    handlePackage(buffer, indBuffer);
                }
                state = ST_STX;
                break;
        }
    }
}

/* main para simular o uso da maquina de estados */
int main()
{
    /*unsigned char data1[] = { STX, 5, 11, 22, 33, 44 };
    unsigned char data2[] = { 55, 39, ETX };
    */

	unsigned char data1[] = { 39, ETX, STX, 5, 11, 22, 33, 44 };
	unsigned char data2[] = { 55, 39, ETX };

    handleRx(data1, sizeof(data1));
    handleRx(data2, sizeof(data2));

    while(1);
}

#else

#include <stdio.h>

/* numero maximo de bytes do buffer de dados */
#define MAX_BUFFER 512

/* constantes usadas na comunicacao */
#define STX     0x02
#define ETX     0x03

/* possiveis estados da maquina de estados de comunicacao */
typedef enum {
    ST_STX = 0, ST_QTD, ST_DATA, ST_CHK, ST_ETX
} States;

typedef void (*Action)(unsigned char data);

struct StateMachine {
    States state;
    unsigned char buffer[MAX_BUFFER];
    unsigned char chkBuffer;
    int indBuffer;
    int qtdBuffer;
    Action action[5];
} sm;


/* trata dados recebidos */
void handlePackage(unsigned char *data, int qtd)
{
    int i;

    printf("Imprimindo dados recebidos...\n");
    for (i = 0; i < qtd; i++)
        printf("Data[%d]=%d\n", i, data[i]);
}


void stSTX(unsigned char data)
{
    if (data == STX) {
        sm.indBuffer = sm.qtdBuffer = 0;
        sm.chkBuffer = 0;
        sm.state = ST_QTD;
    }
}


void stQtd(unsigned char data)
{
    sm.qtdBuffer = data;
    sm.chkBuffer = data;
    sm.state = ST_DATA;
}


void stData(unsigned char data)
{
    sm.buffer[sm.indBuffer++] = data;
    sm.chkBuffer += data;
    if (--sm.qtdBuffer == 0) {
        sm.state = ST_CHK;
    }
}


void stChk(unsigned char data)
{
    if (data == sm.chkBuffer) {
        sm.state = ST_ETX;
    }
    else {
        sm.state = ST_STX;
    }
}


void stETX(unsigned char data)
{
    if (data == ETX) {
        handlePackage(sm.buffer, sm.indBuffer);
    }
    sm.state = ST_STX;
}


void handleRx(unsigned char *data, int qtd)
{

	int i;

    for (i = 0; i < qtd; i++) {
        sm.action[sm.state](data[i]);
    }

}


void initSM()
{
    sm.state = ST_STX;
    sm.buffer[0] = 0;
    sm.chkBuffer = 0;
    sm.indBuffer = 0;
    sm.qtdBuffer = 0;
    sm.action[ST_STX] = stSTX;
    sm.action[ST_QTD] = stQtd;
    sm.action[ST_DATA] = stData;
    sm.action[ST_CHK] = stChk;
    sm.action[ST_ETX] = stETX;
}


//char vetor[]="$GPRMC,123642.00,A,2611.76085,S,05241.46110,W,0.027,,150514,,,A*76\r\n";

char vetor[]="$GPRMC,123648.00,A,2611.75876,S,05241.46315,W,1.230,319.81,150514,,,A*65\r\n";

/* main para simular o uso da maquina de estados */
unsigned char data1[] = { 7, 8, 4, STX, 5, 't', 'e', 's', 't', 'e' };
unsigned char data2[] = { 42, ETX };

float vetor_media[5]={1,2,3,4,5};

float media(float n, float x)
{
	static float media_ant;
	if (n == 1)
	{
		media_ant = (float)x;
		return media_ant;
	}else
	{
		float media;
		media=((n-1)/n)*media_ant + (1/n)*x;
		media_ant = media;
		return media;
	}
}

float media_rec(float n, float *x)
{
	if (n == 1)
	{
		return (float)x[0];
	}else
	{
		float media;
		media=((n-1)/n)*media_rec((n-1),x) + (1/n)*(x[(int)(n-1)]);
		return media;
	}
}

int main()

{
	volatile float media_recursiva = 0;
	volatile float media_recursiva2 = 0;
	int i;
	for(i=1;i<6;i++)
	{
		media_recursiva=media(i,vetor_media[i-1]);
		//media_recursiva2=media_rec(i,vetor_media);
	}
#if 0
    volatile unsigned char aux=0;
	volatile unsigned char checksum_calc=0;
	volatile unsigned char j=1;
	volatile unsigned char v;

	aux = vetor[j];

	while(aux != '*')

	{
				checksum_calc = checksum_calc^aux;
				j++;
				aux = vetor[j];
	}

	v=((vetor[j+1]-48)<<4) | (vetor[j+2]-48);
#endif

	unsigned char soma = data1[1];
	soma += data1[2];
	soma += data1[3];
	soma += data1[4];
	soma += data1[5];
	soma += data1[6];

    initSM();

    handleRx(data1, sizeof(data1));
    handleRx(data2, sizeof(data2));

    while(1);
}


#endif
#endif
