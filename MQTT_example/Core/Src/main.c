/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cJSON.h"
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc3;

DAC_HandleTypeDef hdac;

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim5;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC3_Init(void);
static void MX_TIM3_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_DAC_Init(void);
static void MX_TIM4_Init(void);
static void MX_TIM5_Init(void);
static void MX_UART4_Init(void);
#ifdef __GNUC__
     #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
     #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
	HAL_UART_Transmit(&huart2,(uint8_t *)&ch,1,0xFFFF);
	return ch;
}
//khai bao doc cac bien theo kieu JSON
cJSON *str_json,*str_TB1 , *str_TB2 , *str_C1 , *str_C2  , *str_Mode;

//khai bao bien truyen
uint8_t  rx_index, rx_data;
char rx_buffer[100];

// khai bao bien ket noi uart1 voi esp8266
uint8_t  rx_index1, rx_data1;
char rx_buffer1[500];
char ResponseRX[500];

//khai bao bien, doc du lieu tu cong Com thong qua uart2
uint8_t  rx_index2, rx_data2;
char rx_buffer2[100];

//khai bao cac bien phan hoi
uint32_t  rx_indexResponse;
uint8_t ErrorCode = 0;
int ConfigAT = 0;
long last = 0;
uint8_t CheckConnect = 1;
// khai bao cac bien chinh cua he thong
float ND = 0; // bien nhiet do moi truong
unsigned int DA = 0; // bien do am moi truong
float docadc = 0; // bien doc adc cua hat
unsigned int TTTB1 = 0;  // bien den say
unsigned int TTTB2 = 0;  // bien den sang
unsigned int C1 = 0; // bien muc do 1 - 5
unsigned int C2 = 0; // bien muc do 1 - 5
int chedo = 0; // 0 = auto , 1 = man , 2 == cài d?t
float donhietdo, doadc = 0, Vs = 12 , fixadc =0, readadc =0,Rtro = 0,x;
//bien doc adc 
uint16_t adc_value;
uint16_t adc_value2;
int8_t Av = 104;
int16_t COUNTER = 0,  dtacounter=0, output = 2150 ,rpm = 0;
float error = 0, last_error = 0, up, ud, ui, udf, ui_last, alpha = 0.1;
float error_sat, error_windup, udf_last;
long last;
int16_t T = 100, HILIM = 3700, LOLIM=2150;
int32_t last_COUNTER = 0;
// ket noi he thong voi mqtt
/*char *mqtt_server = "ngoinhaiot.com";
char *mqtt_port = "1111";
char *mqtt_user = "mainwig";
char *mqtt_pass = "DF27664D426B44D5";
char *mqtt_sub = "mainwig/nhandulieu"; // nhan du lieu
char *mqtt_pub = "mainwig/guidulieu"; // gui du lieu*/
// khai bao server
char *mqtt_server = "10a28832e8b742fd8dafc1ed14615060.s2.eu.hivemq.cloud";
char *mqtt_port = "8883";
char *mqtt_user = "dungnguyen1809";
char *mqtt_pass = "dungnguyen1809";
char *mqtt_sub = "dungnguyen1809/ReceiveMsg"; // nhan du lieu
char *mqtt_pub = "dungnguyen1809/SendMsg";   //gui du lieu

// cac ham void chin cua he thong
void SettingESP(void);
void Button(void);
void clearbuffer_UART_ESP(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void Send_AT_Commands_Setting(char *AT_Commands, char *DataResponse, uint32_t timesend , uint32_t setting);
void Received_AT_Commands_ESP(void);
void Received_AT_Commands_ESP_MessagerMQTT(void);
void clearResponse(void);
void ConnectMQTT(char *server , char *port , char *user , char *pass , char *sub , char *pub);
void Send_AT_Commands_ConnectMQTT(char *AT_Commands, char *DataResponse , uint32_t timeout , uint32_t setting , uint32_t count);
void Send_AT_Commands_SendMessager(char *AT_Commands, char *DataResponse , uint32_t timeout , uint32_t setting , uint32_t count);
void SendData(char *pub , float ND , unsigned int DA, float docadc,  unsigned int TB1,  unsigned int TB2, unsigned int C1, unsigned int C2);
void CaiDat(char *Data);
void ParseJson(char *DataMQTT);
void XylyCoBan(char *DataMQTT);
void Send_AT_Commands_CheckConnectMQTT(char *AT_Commands, char *DataResponse , uint32_t timeout , uint32_t setting , uint32_t count);
void HenGio(void);
void 	SendMQTT(void);
void ADC_Select_CH10(void);
void ADC_Select_CH13(void);
void adc_1(void);
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);


/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_ADC3_Init();
  MX_TIM3_Init();
  MX_UART4_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_DAC_Init();
  MX_TIM4_Init();
	MX_TIM5_Init();
	HAL_TIM_Encoder_Start_IT(&htim4,TIM_CHANNEL_ALL);
	HAL_DAC_Start(&hdac, DAC1_CHANNEL_1);
	HAL_TIM_Base_Start_IT(&htim5);
	//HAL_GPIO_WritePin(GPIOE, GPIO_PIN_11, GPIO_PIN_SET);
	// cac khai bao 
	HAL_Delay(1200);
  printf("Setup Uart\r\n");
  HAL_UART_Receive_IT(&huart1, &rx_data1, 1);//ket noi cho esp8266
	HAL_UART_Receive_IT(&huart2, &rx_data2, 1);//ket noi cho usb
	HAL_Delay(10000);
	printf("Setting ESP\r\n");
	SettingESP();	
	HAL_Delay(1000);
	ConnectMQTT(mqtt_server , mqtt_port , mqtt_user , mqtt_pass, mqtt_sub , mqtt_pub);
	last = HAL_GetTick();//delay
  
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    SendMQTT();
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
void adc_1(void)
{
////		ADC_Select_CH10();
////		HAL_ADC_Start(&hadc3);
		//HAL_ADC_PollForConversion(&hadc3,1000);	
////		adc_value = HAL_ADC_GetValue(&hadc3);
////		donhietdo =100*(float)adc_value2/4095*10;
	//	ND = donhietdo;
	ND = 12.3;
////		HAL_ADC_Stop(&hadc3);
		HAL_Delay(10);
		
////		ADC_Select_CH13();
////		HAL_ADC_Start(&hadc3);
		//HAL_ADC_PollForConversion(&hadc3,1000);
////		adc_value2= HAL_ADC_GetValue(&hadc3);
////		doadc =(float)adc_value/4095*10;
		//doadc = doadc/4095*10;
////		readadc = doadc;
////		HAL_ADC_Stop(&hadc3);
////		if(readadc > 0.35)
////		{
////			readadc = readadc - 0.35;
////			float Vadc = readadc/Av;
////			DA++;
////			Rtro = fabs(((11100*(10/11 - Vadc/12)- 1000)*12)/11/Vadc);
////			//HAL_Delay(10);
////			x = log10(Rtro);
////			
////			if(Rtro < 425000)
////			{
////				fixadc = 0.734*x*x*x - 3.401*x*x - 3.546*x + 38.88;			
////			}
////			else fixadc = 0.345*x*x - 5.896*x + 33;
////		}
////		else fixadc = 0;
		//docadc = fixadc;
		docadc=45.6;
		//HAL_Delay(100);
		
}
void SendMQTT(void)
{
	while(1)
	{
	if(HAL_GetTick() - last >= 500)
		{
			if(CheckConnect)
			{
				//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
				//chuongtrinhcambien();	
				adc_1();
				SendData( mqtt_pub , ND ,  DA, docadc ,  TTTB1 , TTTB2, C1, C2);
			}
			last = HAL_GetTick();
			break;
		}
	}
}

//gui du lieu tu kieu int sang kieu char
void SendData(char *pub , float ND , unsigned int DA, float docadc , unsigned int TB1,  unsigned int TB2, unsigned int C1, unsigned int C2)
{
	//AT+MQTTPUBRAW=0,"mainwig/guidulieu",5,0,1$0D$0A
	/*
	tra ve 
	OK\r\n\r\n>
	=> send data => ABCD$0D$0A => tra ve +MQTTPUB:OK
	*/
	
	
	char MQTTPUBRAW[100];
	char JSON[100];
	char Str_ND[100];
	char Str_DA[100];
	char Str_docadc[100];
	char Str_TB1[100];
	char Str_TB2[100];
	char Str_C1[100];
	char Str_C2[100];
	char Length[100];

	
	for(int i = 0 ; i < 100; i++)
	{
		MQTTPUBRAW[i] = 0;
		JSON[i] = 0;
		Str_ND[i] = 0;
		Str_DA[i] = 0;
		Str_docadc[i] = 0;
		Str_TB1[i] = 0;
		Str_TB2[i] = 0;
		Str_C1[i] = 0;
		Str_C2[i] = 0;
		Length[i] = 0;
	}
	sprintf(Str_ND, "%0.2f", ND);
	sprintf(Str_DA, "%d", DA);
	sprintf(Str_docadc, "%0.2f", docadc);
	sprintf(Str_TB1, "%d", TB1);
	sprintf(Str_TB2, "%d", TB2);
	sprintf(Str_C1, "%d", C1);
	sprintf(Str_C2, "%d", C2);
	
	strcat(JSON,"{\"ND\":\"");
	strcat(JSON,Str_ND);
	strcat(JSON,"\",");
	
	strcat(JSON,"\"DA\":\"");
	strcat(JSON,Str_DA);
	strcat(JSON,"\",");
	
	strcat(JSON,"\"docadc\":\"");
	strcat(JSON,Str_docadc);
	strcat(JSON,"\",");
	
	strcat(JSON,"\"TB1\":\"");
	strcat(JSON,Str_TB1);
	strcat(JSON,"\",");
	
	strcat(JSON,"\"TB2\":\"");
	strcat(JSON,Str_TB2);
	strcat(JSON,"\",");
	
	strcat(JSON,"\"C1\":\"");
	strcat(JSON,Str_C1);
	strcat(JSON,"\",");
	
  
	strcat(JSON,"\"C2\":\"");
	strcat(JSON,Str_C2);
	strcat(JSON,"\"}");
	strcat(JSON,"\r\n");
	
	printf("DataJson: %s\n", JSON);
	
	int len = 0;
	len = strlen(JSON);
	sprintf(Length, "%d", len);
	
	////AT+MQTTPUBRAW=0,"mainwig/guidulieu",5,0,1$0D$0A
	strcat(MQTTPUBRAW,"AT+MQTTPUBRAW=0,\"");
	strcat(MQTTPUBRAW,pub);
	strcat(MQTTPUBRAW,"\",");
	strcat(MQTTPUBRAW,Length);
	strcat(MQTTPUBRAW,",0,1\r\n");
	printf("MQTTPUBRAW: %s\n",MQTTPUBRAW);
	
	//{"ND":"","DA":"","adc":"","TB1":"","TB2":"","C1":"","C2":""}

  // cho nay check connect mqtt nua
	Send_AT_Commands_SendMessager(MQTTPUBRAW, "OK\r\n\r\n>" , 5000 , 0 , 3);
	
	clearbuffer_UART_ESP();
	
	if(ErrorCode == 0)
	{
		Send_AT_Commands_SendMessager(JSON, "+MQTTPUB:OK" , 5000 , 0 , 5);
		clearbuffer_UART_ESP();
	}
	
	
	ConfigAT = 1;	
}


//ket noi voi MQTT
void ConnectMQTT(char *server , char *port , char *user , char *pass , char *sub , char *pub)
{
	uint32_t id = 0;
	id = rand()%100;// lay so ngau nhien lon hon 0 nho hon 100
	char clientid[100];
	char MathRandom[100];
	char MQTTUSERCFG[100];
	char MQTTCONN[100];
	char MQTTSUB[100];
	
	for(int i = 0 ; i < 100; i++)
	{
		clientid[i] = 0;
		MathRandom[i] = 0;
		MQTTUSERCFG[i] = 0;
		MQTTCONN[i] = 0;
		MQTTSUB[i] = 0;
	}
	sprintf(MathRandom, "%d", id);
	
	strcat(clientid, "ESP"); // noi chuoi
	strcat (clientid, MathRandom); // noi chuoi
  //cac lenh sau noi chuoi muc tieu tao ra lenh sau de ket noi MQTT 
	//AT+MQTTUSERCFG=0,1,"ESP8266","mainwig","DF27664D426B44D5",0,0,""$0D$0A => OK
	strcat(MQTTUSERCFG, "AT+MQTTUSERCFG=0,4,\"");
	strcat(MQTTUSERCFG,clientid);
	strcat(MQTTUSERCFG,"\",\"");
	strcat(MQTTUSERCFG,user);
	strcat(MQTTUSERCFG,"\",\"");
	strcat(MQTTUSERCFG,pass);
	strcat(MQTTUSERCFG,"\",0,0,");
	strcat(MQTTUSERCFG,"\"\"");
	strcat(MQTTUSERCFG,"\r\n");
	//printf("MQTTUSERCFG: %s",MQTTUSERCFG);
	
	//cac lenh sau noi chuoi muc tieu tao ra lenh sau de ket noi MQTT 
	//AT+MQTTCONN=0,"ngoinhaiot.com",1111,1$0D$0A
	strcat(MQTTCONN, "AT+MQTTCONN=0,\"");
	strcat(MQTTCONN, server);
	strcat(MQTTCONN, "\",");
	strcat(MQTTCONN, port);
	strcat(MQTTCONN, ",1\r\n");
	//printf("MQTTCONN: %s",MQTTCONN);
	
	//cac lenh sau noi chuoi muc tieu tao ra lenh sau de ket noi MQTT
	//AT+MQTTSUB=0,"mainwig/nhandulieu",0$0D$0A => OK
	strcat(MQTTSUB, "AT+MQTTSUB=0,\"");
	strcat(MQTTSUB, sub);
	strcat(MQTTSUB, "\",0\r\n");
	//printf("MQTTSUB: %s",MQTTSUB);
	
	// gui lenh qua ESP de ket noi MQTT qua wifi
	Send_AT_Commands_ConnectMQTT(MQTTUSERCFG, "OK" , 5000 , 0 , 5);
	HAL_Delay(1000);
	clearbuffer_UART_ESP();
	
	// gui lenh qua ESP de ket noi MQTT qua wifi
	Send_AT_Commands_ConnectMQTT(MQTTCONN, "+MQTTCONNECTED" , 5000 , 0 , 5);
	HAL_Delay(1000);
	clearbuffer_UART_ESP();
	
	// gui lenh qua ESP de ket noi MQTT qua wifi
	Send_AT_Commands_ConnectMQTT(MQTTSUB, "OK" , 5000 , 0 , 5);
	HAL_Delay(1000);
	clearbuffer_UART_ESP();
	
	ConfigAT = 1;
	ErrorCode = 1;
	
}

// gui ket noi AT den esp8266
void Send_AT_Commands_SendMessager(char *AT_Commands, char *DataResponse , uint32_t timeout , uint32_t setting , uint32_t count)
{
	clearbuffer_UART_ESP();
	last = HAL_GetTick();
	uint32_t Size = 300;
	uint32_t Count = 0;
	ConfigAT = setting;
	char DataHTTP[Size];
	for(int i = 0 ; i < Size; i++)
	{
		DataHTTP[i] = 0;
	}
	// dua data lenh AT_Commands vao mang  SendHTTP
	snprintf(DataHTTP, sizeof(DataHTTP),"%s", AT_Commands);
	HAL_UART_Transmit(&huart1,(uint8_t *)&DataHTTP,strlen(DataHTTP),1000);
	printf("Send AT-Commands Data: %s\r\n", DataHTTP);
	last = HAL_GetTick();
	while(1)
	{
		//chay ham ngat uart
		if(HAL_GetTick() - last >= timeout)
		{
			Count++;
			HAL_UART_Transmit(&huart1,(uint8_t *)&DataHTTP,strlen(DataHTTP),1000);
			printf("Send AT-Commands Send Data MQTT: %s\r\n", DataHTTP);
			last = HAL_GetTick();
		}
		if(strstr(rx_buffer1,DataResponse) != NULL)
		{
			//printf("Reponse DataBlynk: %s\r\n",DataResponse);
			printf("SEND MQTT OK\r\n");
			clearbuffer_UART_ESP();
			ErrorCode = 0;
			CheckConnect = 1;
			last = HAL_GetTick();
			break;
		}
		if(Count >= count)
		{
			printf("SEND MQTT ERROR\r\n"); // gui lenh setting lai
			ErrorCode = 1;
			clearbuffer_UART_ESP();
			last = HAL_GetTick();
			break;
		}
	}
}

void Send_AT_Commands_ConnectMQTT(char *AT_Commands, char *DataResponse , uint32_t timeout , uint32_t setting , uint32_t count)
{
	clearbuffer_UART_ESP();
	last = HAL_GetTick();
	uint32_t Size = 300;
	uint32_t Count = 0;
	ConfigAT = setting;
	char DataHTTP[Size];
	for(int i = 0 ; i < Size; i++)
	{
		DataHTTP[i] = 0;
	}
	// dua data lenh AT_Commands vao mang  SendHTTP
	snprintf(DataHTTP, sizeof(DataHTTP),"%s", AT_Commands);
	HAL_UART_Transmit(&huart1,(uint8_t *)&DataHTTP,strlen(DataHTTP),1000);
	printf("Send AT-Commands Data: %s\r\n", DataHTTP);
	last = HAL_GetTick();
	while(1)
	{
		//chay ham ngat uart
		if(HAL_GetTick() - last >= timeout)
		{
			Count++;
			HAL_UART_Transmit(&huart1,(uint8_t *)&DataHTTP,strlen(DataHTTP),1000);
			printf("Send AT-Commands Data TimeOut: %s\r\n", DataHTTP);
			last = HAL_GetTick();
		}
		if(strstr(rx_buffer1,DataResponse) != NULL)
		{
			//printf("Reponse DataBlynk: %s\r\n",DataResponse);
			printf("MQTT Connect OK\r\n");
			clearbuffer_UART_ESP();
			ErrorCode = 1;
			CheckConnect = 1;
			last = HAL_GetTick();
			
			break;
		}
		if(Count >= count)
		{
			printf("MQTT Connect ERROR\r\n"); // gui lenh setting lai
			ErrorCode = 0;
			CheckConnect = 0;
			clearbuffer_UART_ESP();
			last = HAL_GetTick();
			break;
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	// che do setting ConfigAT = 0
	if(ConfigAT == 0)
	{
		if(huart -> Instance == USART1)
		{
			Received_AT_Commands_ESP();
			HAL_UART_Receive_IT(&huart1,&rx_data1,1);
		}
	}
	else if(ConfigAT == 1)
	{
		if(huart -> Instance == USART1)
		{
			Received_AT_Commands_ESP_MessagerMQTT();
			HAL_UART_Receive_IT(&huart1,&rx_data1,1);
		}
	}
}

void Received_AT_Commands_ESP(void)
{
	rx_buffer1[rx_index1++] = rx_data1;
}

void Received_AT_Commands_ESP_MessagerMQTT(void)
{
	
		if(rx_data1 != '\n')
		{
			ResponseRX[rx_indexResponse++] = rx_data1;
		}
		else
		{
			ResponseRX[rx_indexResponse++] = rx_data1;
			rx_indexResponse = 0;	
			printf("Data MQTT_XQ1:%s\r\n",ResponseRX);	
			
			//CheckConnect
			if(strstr(ResponseRX,"MQTTCONNECTED") != NULL) 
			{
			//	printf("Connect MQTT\r\n");	
				CheckConnect = 1;
				last = HAL_GetTick();
			}
			else if(strstr(ResponseRX,"MQTTDISCONNECTED") != NULL)
			{
			//	printf("Not Connect MQTT\r\n");
				CheckConnect = 0;
				last = HAL_GetTick();
			}
			else if(strstr(ResponseRX,"+MQTTSUBRECV") != NULL)
			{
				char *DataMQTT;
			//+MQTTSUBRECV:0,"mainwig/nhandulieu",9,{"A":"1"}\n
				DataMQTT = strtok(ResponseRX,",");
				DataMQTT = strtok(NULL,",");
				DataMQTT = strtok(NULL,",");
				DataMQTT = strtok(NULL,"\n");		
				printf("DATA MQTT_XQ2: %s\r\n",DataMQTT);		
				
				ParseJson(DataMQTT);
				//XylyCoBan(DataMQTT);
				/*	
				if(strstr(DataMQTT,"A0B") != NULL)
				{
					TTTB1 = 0;
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
				}
				else if(strstr(DataMQTT,"A1B") != NULL)
				{
					TTTB1 = 1;
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
				}
				else if(strstr(DataMQTT,"C0D") != NULL)
				{
					TTTB2 = 0;
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);
				}
				else if(strstr(DataMQTT,"C1D") != NULL)
				{
					TTTB2 = 1;
					HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);
				}
				else 
				{
					CaiDat(DataMQTT);
				}
				*/
				
				last = HAL_GetTick();
			}		
			last = HAL_GetTick();
			clearResponse();				
		}
}

void ParseJson(char *DataMQTT)
{
	str_json = cJSON_Parse(DataMQTT);
	if (!str_json)
  {
			//printf("JSON error\r\n"); 
			return;
  }
	else
	{
		//printf("JSON OKE\r\n"); 
		//{"TB1":"0"} {"TB1":"1"}
		str_TB1 = cJSON_GetObjectItem(str_json, "TB1"); 

      if (str_TB1->type == cJSON_String)
      {
				
				if(strstr(str_TB1->valuestring,"0") != NULL)
				{
					//printf("OFF 1\r\n");
					TTTB1 = 0;
					HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
				}
				else if(strstr(str_TB1->valuestring,"1") != NULL)
				{
					//printf("ON 1\r\n");
					TTTB1 = 1;
					HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);
					HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
				}
      }

			str_TB2 = cJSON_GetObjectItem(str_json, "TB2"); //Get information about the value corresponding to the name key
      if (str_TB2->type == cJSON_String)
      {
			
				if(strstr(str_TB2->valuestring,"0") != NULL)
				{
					//printf("OFF 2\r\n");
					TTTB2 = 0;
					HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
				}
				else if(strstr(str_TB2->valuestring,"1") != NULL)
				{
				//	printf("ON 2\r\n");
					TTTB2 = 1;
					HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);
				}
      }
			
			str_C1 = cJSON_GetObjectItem(str_json, "C1"); //Get information about the value corresponding to the name key
     
			if (str_C1->type == cJSON_String)
      {
			//	printf("C1:%s \r\n", str_C1->valuestring);
				C1 = atoi(str_C1->valuestring);
			
      }
			
			str_C2 = cJSON_GetObjectItem(str_json, "C2"); //Get information about the value corresponding to the name key
      if (str_C2->type == cJSON_String)
      {
				//printf("C2:%s \r\n", str_C2->valuestring);
				C2 = atoi(str_C2->valuestring);
			
      }
			
			str_Mode = cJSON_GetObjectItem(str_json, "Mode"); //Get information about the value corresponding to the name key
      if (str_Mode->type == cJSON_String)
      {
				//printf("C2:%s \r\n", str_C2->valuestring);
				chedo = atoi(str_Mode->valuestring);	
      }
		
		cJSON_Delete(str_json);
	}
}

void XylyCoBan(char *DataMQTT)
{
				if(strstr(DataMQTT,"A0B") != NULL)
				{
					TTTB1 = 0;
					HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_RESET);
				}
				else if(strstr(DataMQTT,"A1B") != NULL)
				{
					TTTB1 = 1;
					HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7, GPIO_PIN_SET);
				}
				else if(strstr(DataMQTT,"C0D") != NULL)
				{
					TTTB2 = 0;
					HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_RESET);
				}
				else if(strstr(DataMQTT,"C1D") != NULL)
				{
					TTTB2 = 1;
					HAL_GPIO_WritePin(GPIOE, GPIO_PIN_8, GPIO_PIN_SET);
				}
				else 
				{
					CaiDat(DataMQTT);
				}
}

void CaiDat(char *Data)
{
	char *DataX;
	if(strstr(Data,"E") != NULL && strstr(Data,"F") != NULL)
	{
		
		DataX = strtok(Data,"E");
		DataX = strtok(DataX,"F");
		C1 = atoi(DataX);
		printf("C1:%d\r\n",C1);	
	}
	else if(strstr(Data,"G") != NULL && strstr(Data,"H") != NULL)
	{
		DataX = strtok(Data,"G");
		DataX = strtok(DataX,"H");
		C2 = atoi(DataX);
		printf("C2:%d\r\n",C2);
	}
}

void clearbuffer_UART_ESP(void)
{
	for(int i = 0 ; i < 500 ; i++)
	{
		rx_buffer1[i] = 0;
	}
	rx_index1 = 0;	
}

void clearResponse(void)
{
	for(int i = 0 ; i < 500; i++)
	{
		ResponseRX[i] = 0;
	}
	rx_indexResponse = 0;
}

void Send_AT_Commands_Setting(char *AT_Commands, char *DataResponse, uint32_t timesend , uint32_t setting)
{
	last = HAL_GetTick();
	ConfigAT = setting;
	char DataSendAT[50];
	for(int i = 0 ; i < 50; i++)
	{
		DataSendAT[i] = 0;
	}
	// dua data lenh AT_Commands vao mang  DataSendAT
	snprintf(DataSendAT, sizeof(DataSendAT),"%s\r\n", AT_Commands);
	// send ESP qua cong UART2
	HAL_UART_Transmit(&huart1,(uint8_t *)&DataSendAT,strlen(DataSendAT),1000);
	printf("Send AT-Commands Setting: %s\r\n", DataSendAT);
	// dung doi phan hoi + doi lau qua thi goi tiep ( phan hoi tu ham ngat uart2) 
	last = HAL_GetTick();
	while(1)
	{
		// qua 5s thi gui lai lenh cu ( gui khi nao ok thi thoi)
		if(HAL_GetTick() - last >= timesend)
		{
			HAL_UART_Transmit(&huart1,(uint8_t *)&DataSendAT,strlen(DataSendAT),1000);
			printf("Send AT-Commands Setting TimeSend: %s\r\n", DataSendAT);
			last = HAL_GetTick();
		}
		if(strstr(rx_buffer1,DataResponse) != NULL)
		{
			//printf("Data Buffer2: %s\r\n",rx_buffer2);
			printf("Reponse Setting: %s\r\n",DataResponse);
			clearbuffer_UART_ESP();
			break;
		}
	}
}

void SettingESP(void)
{
	// reset ESP
	Send_AT_Commands_Setting("AT+RST\r\n", "OK", 10000 , 0);
	HAL_Delay(3000);
	//Send_AT_Commands_Setting("AT+CWQAP\r\n", "WIFI DISCONNECT\r\n\r\nOK" , 2000, 0);
	//HAL_Delay(3000);
	// esp co dang hoat dong khong
	Send_AT_Commands_Setting("AT\r\n", "OK", 300, 0);
	HAL_Delay(3000);
	// tat phan hoi khong can
	Send_AT_Commands_Setting("ATE0\r\n", "OK" , 2000, 0);
	HAL_Delay(3000);
	// cai dat che do hoat dong wife thay cho 1,1	
	Send_AT_Commands_Setting("AT+CWMODE=1,1\r\n", "OK", 2000, 0);
	HAL_Delay(3000);
	// wifi can ket noi
	//Send_AT_Commands_Setting("AT+CWJAP=\"TP-Link_9B0E\",\"39072177\"\r\n", "WIFI CONNECTED", 10000, 0);
	Send_AT_Commands_Setting("AT+CWJAP=\"My Duyen\",\"07028173d\"\r\n", "WIFI CONNECTED", 10000, 0);
//	Send_AT_Commands_Setting("AT+CWJAP=\"BinBin\",\"1234567890\"\r\n", "WIFI CONNECTED", 10000, 0);
	// \"user name\",\"pass"   ( \"Hung Han\",\"1234567890\")
	HAL_Delay(3000);
	// wifi can ket noi
	Send_AT_Commands_Setting("AT+CIPMUX=0\r\n", "OK", 2000 , 0);
	HAL_Delay(3000);
	ErrorCode = 0;
}
void ADC_Select_CH10(void)
{
 ADC_ChannelConfTypeDef sConfig = {0};
 sConfig.Channel = ADC_CHANNEL_10;
 sConfig.Rank = 1;
 sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
 if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
 {
   Error_Handler();
 }
}
void ADC_Select_CH13(void)
{
 ADC_ChannelConfTypeDef sConfig = {0};
 sConfig.Channel = ADC_CHANNEL_13;
 sConfig.Rank = 1;
 sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
 if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
 {
   Error_Handler();
 }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 13;
  RCC_OscInitStruct.PLL.PLLN = 195;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC3_Init(void)
{

  /* USER CODE BEGIN ADC3_Init 0 */

  /* USER CODE END ADC3_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};
  ADC_InjectionConfTypeDef sConfigInjected = {0};

  /* USER CODE BEGIN ADC3_Init 1 */

  /* USER CODE END ADC3_Init 1 */
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc3.Instance = ADC3;
  hadc3.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc3.Init.Resolution = ADC_RESOLUTION_12B;
  hadc3.Init.ScanConvMode = ENABLE;
  hadc3.Init.ContinuousConvMode = DISABLE;
  hadc3.Init.DiscontinuousConvMode = DISABLE;
  hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc3.Init.NbrOfConversion = 2;
  hadc3.Init.DMAContinuousRequests = DISABLE;
  hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc3) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_13;
  sConfig.Rank = 2;
  if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configures for the selected ADC injected channel its corresponding rank in the sequencer and its sample time
  */
  sConfigInjected.InjectedChannel = ADC_CHANNEL_10;
  sConfigInjected.InjectedRank = 1;
  sConfigInjected.InjectedNbrOfConversion = 1;
  sConfigInjected.InjectedSamplingTime = ADC_SAMPLETIME_3CYCLES;
  sConfigInjected.ExternalTrigInjecConvEdge = ADC_EXTERNALTRIGINJECCONVEDGE_NONE;
  sConfigInjected.ExternalTrigInjecConv = ADC_INJECTED_SOFTWARE_START;
  sConfigInjected.AutoInjectedConv = DISABLE;
  sConfigInjected.InjectedDiscontinuousConvMode = DISABLE;
  sConfigInjected.InjectedOffset = 0;
  if (HAL_ADCEx_InjectedConfigChannel(&hadc3, &sConfigInjected) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC3_Init 2 */

  /* USER CODE END ADC3_Init 2 */

}

/**
  * @brief DAC Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC_Init(void)
{

  /* USER CODE BEGIN DAC_Init 0 */

  /* USER CODE END DAC_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC_Init 1 */

  /* USER CODE END DAC_Init 1 */
  /** DAC Initialization
  */
  hdac.Instance = DAC;
  if (HAL_DAC_Init(&hdac) != HAL_OK)
  {
    Error_Handler();
  }
  /** DAC channel OUT1 config
  */
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC_Init 2 */

  /* USER CODE END DAC_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim4, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 59999;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 39;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim5, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */

}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{

  /* USER CODE BEGIN UART4_Init 0 */

  /* USER CODE END UART4_Init 0 */

  /* USER CODE BEGIN UART4_Init 1 */

  /* USER CODE END UART4_Init 1 */
  huart4.Instance = UART4;
  huart4.Init.BaudRate = 115200;
  huart4.Init.WordLength = UART_WORDLENGTH_8B;
  huart4.Init.StopBits = UART_STOPBITS_1;
  huart4.Init.Parity = UART_PARITY_NONE;
  huart4.Init.Mode = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN UART4_Init 2 */

  /* USER CODE END UART4_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11, GPIO_PIN_RESET);
	
	/*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

  /*Configure GPIO pins : PE2 PE3 PE4 PE5
                           PE6 PE12 PE13 PE14
                           PE15 PE0 PE1 */
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14
                          |GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pins : PC13 PC1 PC2 PC4
                           PC5 PC6 PC7 PC8
                           PC9 PC10 PC11 PC12 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
                          |GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA5 PA6 PA7 PA8
                           PA11 PA12 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
                          |GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB1 PB2 PB10 PB11
                           PB12 PB13 PB14 PB15
                           PB6 PB7 PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PE7 PE8 PE9 PE10
                           PE11 */
  GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10
                          |GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	/*Configure GPIO pins : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PD8 PD9 PD10 PD11
                           PD14 PD15 PD0 PD1
                           PD2 PD3 PD4 */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11
                          |GPIO_PIN_14|GPIO_PIN_15|GPIO_PIN_0|GPIO_PIN_1
                          |GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PD5 PD6 PD7 */
  GPIO_InitStruct.Pin = GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PB3 PB4 PB5 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
