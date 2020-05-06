/*
 * sm.c
 *
 *  Created on: 10/09/2015
 *      Author: gusta
 */

#include "sm.h"
#include <stdio.h>
#include <string.h>



static void stSTX(sm_t *sm, unsigned char data)
{
    if (data == STX) {
        sm->idx = 0;
        sm->qtd = 0;
        sm->chk = 0;
        sm->state = ST_ADDR;
        sm->flag_addr = 0;
        //sm.result = false;
    }
}


static void stAddr(sm_t *sm, unsigned char data)
{
	if (!sm->flag_addr) {
		sm->addr = (unsigned short)(data);
		sm->chk = data;
		sm->flag_addr = 1;
    }else{
    	sm->flag_addr = 0;
		sm->addr |= (unsigned short)(data << 8);
    	if (sm->addr == sm->my_addr){
        	sm->state = ST_QTD;
        	sm->chk += data;
    	}else{
    		sm->state = ST_STX;
    		if (sm->HandleError != NULL){
    		    sm->HandleError("Endereço errado...\n\r");
    		}
    	}
    }
}

static void stQtd(sm_t *sm, unsigned char data)
{
    if ((data >= 1) && (data <= 128)) {
		sm->qtd = data;
		sm->chk += data;
		sm->state = ST_DATA;
    }else{
    	sm->state = ST_STX;
    }
}


static void stData(sm_t *sm, unsigned char data)
{
    sm->buffer[sm->idx++] = data;
    sm->chk += data;
    if (--sm->qtd == 0) {
        sm->state = ST_CHK;
    }
}


static void stChk(sm_t *sm, unsigned char data)
{
    if (data == sm->chk) {
        //sm.result = true;
    	sm->state = ST_ETX;
    }
    else {
    	//sm.result = false;
    	sm->state = ST_STX;
    	if (sm->HandleError != NULL){
    	    sm->HandleError("Falha de Checksum...\n\r");
    	}
    }
}


static void stETX(sm_t *sm, unsigned char data)
{
    if (data == ETX) {
    	//if (sm.result == true){
    		sm->buffer[sm->idx] = '\0';
    		if (sm->HandlePackage != NULL){
    		    sm->HandlePackage(sm->buffer);
    		}
    	//}
    }
    sm->state = ST_STX;
}

void initSM(sm_t *sm, unsigned short my_addr, Handle_t handle_function, Handle_t error_function)
{
    sm->state = ST_STX;
    sm->my_addr = my_addr;
    sm->buffer[0] = 0;
    sm->chk = 0;
    sm->idx = 0;
    sm->qtd = 0;
    sm->action[ST_STX] = (Action_t)stSTX;
    sm->action[ST_ADDR] = (Action_t)stAddr;
    sm->action[ST_QTD] = (Action_t)stQtd;
    sm->action[ST_DATA] = (Action_t)stData;
    sm->action[ST_CHK] = (Action_t)stChk;
    sm->action[ST_ETX] = (Action_t)stETX;
    sm->HandlePackage = handle_function;
    sm->HandleError = error_function;
}


void ExecSM(sm_t *sm, unsigned char *data, int qtd)
{
	int i;

    for (i = 0; i < qtd; i++) {
        sm->action[sm->state](sm, data[i]);
    }

}


#if 0
static void stSTX(unsigned char data)
{
    if (data == STX) {
        sm.idx = 0;
        sm.qtd = 0;
        sm.chk = 0;
        sm.state = ST_ADDR;
        //sm.result = false;
    }
}


static void stAddr(unsigned char data)
{
    static char flag = 0;
	if (!flag) {
		sm.addr = (unsigned short)(data);
		sm.chk = data;
		flag = 1;
    }else{
    	flag = 0;
		sm.addr |= (unsigned short)(data << 8);
    	if (sm.addr == sm.my_addr){
        	sm.state = ST_QTD;
        	sm.chk += data;
    	}else{
    		sm.state = ST_STX;
    	}
    }
}

static void stQtd(unsigned char data)
{
    if ((data >= 1) && (data <= 128)) {
		sm.qtd = data;
		sm.chk += data;
		sm.state = ST_DATA;
    }else{
    	sm.state = ST_STX;
    }
}


static void stData(unsigned char data)
{
    sm.buffer[sm.idx++] = data;
    sm.chk += data;
    if (--sm.qtd == 0) {
        sm.state = ST_CHK;
    }
}


static void stChk(unsigned char data)
{
    if (data == sm.chk) {
        //sm.result = true;
    	sm.state = ST_ETX;
    }
    else {
    	//sm.result = false;
    	sm.state = ST_STX;
    }
}


static void stETX(unsigned char data)
{
    if (data == ETX) {
    	//if (sm.result == true){
    		sm.buffer[sm.idx] = '\0';
    		sm.HandlePackage(sm.buffer);
    	//}
    }
    sm.state = ST_STX;
}

void initSM(unsigned short my_addr, Handle handle_function)
{
    sm.state = ST_STX;
    sm.my_addr = my_addr;
    sm.buffer[0] = 0;
    sm.chk = 0;
    sm.idx = 0;
    sm.qtd = 0;
    sm.action[ST_STX] = stSTX;
    sm.action[ST_ADDR] = stAddr;
    sm.action[ST_QTD] = stQtd;
    sm.action[ST_DATA] = stData;
    sm.action[ST_CHK] = stChk;
    sm.action[ST_ETX] = stETX;
    sm.HandlePackage = handle_function;
}

void ExecSM(unsigned char *data, int qtd)
{
	int i;

    for (i = 0; i < qtd; i++) {
        sm.action[sm.state](data[i]);
    }

}
#endif
