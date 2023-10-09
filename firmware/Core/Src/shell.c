/*
 * shell.c
 *
 *  Created on: Oct 3, 2023
 *      Author: grego
 */
#include "usart.h"
#include "shell.h"
#include <stdio.h>
#include <string.h>

uint8_t prompt[]="user@Nucleo-STM32G474RET6>>"; //Contenant le prompt comme sur un shell linux
uint8_t started[]=
		"\r\n*-----------------------------*"
		"\r\n| Welcome on Nucleo-STM32G474 |"
		"\r\n*-----------------------------*"
		"\r\n"; //Contenant un message de bienvenue au démarrage du microprocesseur
uint8_t newline[]="\r\n"; //Contenant la chaine de caractère pour faire un retour à la ligne
uint8_t backspace[]="\b \b";
uint8_t cmdNotFound[]="Command not found\r\n"; //Contenant le message du commande non reconnue
uint8_t brian[]="Brian is in the kitchen\r\n";
uint8_t uartRxReceived; //Flag de récéption d'un caractère sur la liaison uart
uint8_t uartRxBuffer[UART_RX_BUFFER_SIZE]; //buffer de réception de donnée de l'uart
uint8_t uartTxBuffer[UART_TX_BUFFER_SIZE]; //buffer d'émission des données de l'uart

char cmd[CMD_BUFFER_SIZE]; //Contenant la commande en cours
int idxCmd; //Contenant l'index du prochain caractère à remplir
const uint8_t help[]="\r\nVoici les fonctions disponibles : "
					"\r\n pinout : affiche toutes les broches connectees et leur fonction"
					"\r\n start : allume l'etage de puissance du moteur"
					"\r\n stop : eteint l'etage de puissance du moteur"
					"\r\n"
					"\r\n"; //Contenant le message d'aide, la liste des fonctions
const uint8_t pinout[]="\r\nVoici la liste des pin utilisees : "
					"\r\n PA10 : W_PWM_H"
					"\r\n PA9 : V_PWM_H"
					"\r\n PA8 : U_PWM_H"
					"\r\n PB15 : W_PWM_L"
					"\r\n PB14 : V_PWM_L"
					"\r\n PB13 : U_PWM_L"
					"\r\n PC0 : U_VPh"
					"\r\n PC1 : W_VPh"
					"\r\n PC3 : V_VPh"
					"\r\n PC2 : Bus_Imes"
					"\r\n PA0 : Bus_V"
					"\r\n PA1 : U_Imes"
					"\r\n"
					"\r\n"; //Contenant la liste des pin utilisées
const uint8_t powerOn[]="\r\nPower ON"
					"\r\n"
					"\r\n"; //Contenant le message d'allumage du moteur
const uint8_t powerOff[]="\r\nPower OFF"
					"\r\n"
					"\r\n"; //Contenant le message d'extinction du moteur


char	 	cmdBuffer[CMD_BUFFER_SIZE];
int 		idx_cmd;
char* 		argv[MAX_ARGS];
int		 	argc = 0;
char*		token;
int 		newCmdReady = 0;

void Shell_Init(void){
	memset(argv, NULL, MAX_ARGS*sizeof(char*));
	memset(cmdBuffer, NULL, CMD_BUFFER_SIZE*sizeof(char));
	memset(uartRxBuffer, NULL, UART_RX_BUFFER_SIZE*sizeof(char));
	memset(uartTxBuffer, NULL, UART_TX_BUFFER_SIZE*sizeof(char));

	HAL_UART_Receive_IT(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE);
	HAL_UART_Transmit(&huart2, started, strlen((char *)started), HAL_MAX_DELAY);
	HAL_UART_Transmit(&huart2, prompt, strlen((char *)prompt), HAL_MAX_DELAY);
}

void Shell_Loop(void){
	if(uartRxReceived){
		switch(uartRxBuffer[0]){
		case ASCII_CR: // Nouvelle ligne, instruction à traiter
			HAL_UART_Transmit(&huart2, newline, sizeof(newline), HAL_MAX_DELAY);
			cmdBuffer[idx_cmd] = '\0';
			argc = 0;
			token = strtok(cmdBuffer, " ");
			while(token!=NULL){
				argv[argc++] = token;
				token = strtok(NULL, " ");
			}
			idx_cmd = 0;
			newCmdReady = 1;
			break;
		case ASCII_BACK: // Suppression du dernier caractère
			cmdBuffer[idx_cmd--] = '\0';
			HAL_UART_Transmit(&huart2, backspace, sizeof(backspace), HAL_MAX_DELAY);
			break;

		default: // Nouveau caractère
			cmdBuffer[idx_cmd++] = uartRxBuffer[0];
			HAL_UART_Transmit(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE, HAL_MAX_DELAY);
		}
		uartRxReceived = 0;
	}

	if(newCmdReady){
		if(strcmp(argv[0],"WhereisBrian?")==0){
			HAL_UART_Transmit(&huart2, brian, sizeof(brian), HAL_MAX_DELAY);
		}
		else if(strcmp(argv[0],"help")==0){
			int uartTxStringLength = snprintf((char *)uartTxBuffer, UART_TX_BUFFER_SIZE, "Print all available functions here\r\n");
			HAL_UART_Transmit(&huart2, uartTxBuffer, uartTxStringLength, HAL_MAX_DELAY);
			HAL_UART_Transmit(&huart2, help, sizeof(help), HAL_MAX_DELAY);
		}
		else if(strcmp(argv[0],"pinout")==0){
			HAL_UART_Transmit(&huart2, pinout, sizeof(pinout), HAL_MAX_DELAY);
		}
		else if(strcmp(argv[0],"start")==0){
					HAL_UART_Transmit(&huart2, powerOn, sizeof(powerOn), HAL_MAX_DELAY);
		}
		else if(strcmp(argv[0],"stop")==0){
					HAL_UART_Transmit(&huart2, powerOff, sizeof(powerOff), HAL_MAX_DELAY);
		}
		else{
			HAL_UART_Transmit(&huart2, cmdNotFound, sizeof(cmdNotFound), HAL_MAX_DELAY);
		}
		HAL_UART_Transmit(&huart2, prompt, sizeof(prompt), HAL_MAX_DELAY);
		newCmdReady = 0;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart){
	uartRxReceived = 1;
	HAL_UART_Receive_IT(&huart2, uartRxBuffer, UART_RX_BUFFER_SIZE);
}