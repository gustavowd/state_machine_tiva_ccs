/*
 * sm.h
 *
 *  Created on: 10/09/2015
 *      Author: gusta
 */

#ifndef SM_H_
#define SM_H_

#include <stdbool.h>

/* constantes usadas na comunicacao */
#define STX     '$'
#define ETX     '\n'

/* numero maximo de bytes do buffer de dados */
#define MAX_BUFFER 128

/* possiveis estados da maquina de estados de comunicacao */
typedef enum {
    ST_STX = 0, ST_ADDR, ST_QTD, ST_DATA, ST_CHK, ST_ETX, ST_END
} states_t;

typedef void (*handle_t)(unsigned char *data);

typedef struct StateMachine sm_t;

//Funções executadas nos estados
typedef void (*action_t)(sm_t *sm, unsigned char data);


struct StateMachine {
    states_t state;
    unsigned char buffer[MAX_BUFFER];
    unsigned char chk;
    unsigned char qtd;
    int idx;
    char flag_addr;
    unsigned short addr;
    unsigned short my_addr;
    action_t action[ST_END];
    //bool result;
    handle_t HandlePackage;
    handle_t HandleError;
};


void initSM(sm_t *sm, unsigned short my_addr, handle_t handle_function, handle_t error_function);
void ExecSM(sm_t *sm, unsigned char *data, int qtd);

#endif /* SM_H_ */
