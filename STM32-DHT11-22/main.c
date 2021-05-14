//************************************************************************************************
//          teste
//
//
//
//***********************************************************************************************
#include "main.h"
#include "string.h"
#include <stdio.h>
#include "ssd1306.h"
#include "EEPROM.h"
#include "stdio.h"
#include "ds1307_for_stm32_hal.h"
#include "DHT.h"

I2C_HandleTypeDef hi2c2;
TIM_HandleTypeDef htim7;
UART_HandleTypeDef huart1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C2_Init(void);
static void MX_TIM7_Init(void);
static void MX_USART1_UART_Init(void);

#define ledvd_on   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
#define ledvd_off  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
#define ledvm_on   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
#define ledvm_off  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);

static DHT_sensor livingRoom = {GPIOA, GPIO_PIN_2, DHT11, 0};

#define bot1 HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_3)
#define bot2 HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_4)


//************************************************************************************************
//      var globais
//************************************************************************************************

uint8_t UART1_rxBuffer[6] = {0};

char msg1[100];
char msg2[100];
char msg3[3];
char msg4[50];

char data;

uint8_t pipi = 33;

uint8_t dataw1[] = "hello";
uint8_t datar1[] = "-----";

char d[1]={8};
char c[1]={9};

unsigned char ser_bfm[48];
uint ser_pcm;
uint ser_rcm;
unsigned char ser_ptm;


uint8_t xBuffer[1];

#define DEVICE_ADDRESS      0xA0   /* A0 = A1 = A2 = 0 */
#define MEMORY_ADDRESS  	0x00

const char *DAYS_OF_WEEK[7] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };

uint8_t date = 0;
uint8_t month = 0;
uint16_t year = 0;
uint8_t dow = 0;
uint8_t hour = 0;
uint8_t minute =0;
uint8_t second = 0;
int8_t zone_hr = 0;
uint8_t zone_min = 0;
char buffer[100] = { 0 };

uint16_t filtro = 1000;

char d1=22;
char d2=11;

const unsigned char tbh[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};


//*******************************************************************************************************
void dht(void){


    DHT_data d = DHT_getData(&livingRoom);
    //sprintf(msg4, "T %d°C,  H %d%%", (uint8_t)d.temp, (uint8_t)d.hum);
    sprintf(msg4, "T %dC H %d%%", (uint8_t)d.temp, (uint8_t)d.hum);


}


//*******************************************************************************************************

int putchar(int ch)
{
 uint8_t c[1];
 c[0] = ch & 0x00FF;
 HAL_UART_Transmit(&huart1, &*c, 1, 10);
 return ch;
}

//*******************************************************************************************************
//converte um byte em dois caracteres e envia pela serial do monitor
void evb(unsigned char bt)
{
	putchar( tbh[ bt/16]);
	putchar( tbh[ bt%16]);
};

//********************************************************************************************************
//converte um int em tres ou quatro caracteres e envia pela serial
void evint(uint16_t v, unsigned char q)
{
  unsigned char bt;

  bt = (v/256);
  if (q) putchar( tbh[ bt>>4]);
  putchar( tbh[ bt & 0xf]);
  bt = v&0xff;
  putchar( tbh[ bt/16]);
  putchar( tbh[ bt%16]);
};

//*****************************************************************************************************
//converte dois caracteres formato hexa em um byte
unsigned char hextochar(char c1, char c2)
{
 unsigned char r;
 unsigned char r2;

 if (c1>='0' && c1<='9') { r=c1-48;}
 else if (c1>='A' && c1<='F') { r=c1-55;}
 else if (c1>='a' && c1<='f') { r=c1-87;}
 r=r<<4;
 if (c2>='0' && c2<='9') { r2 = (c2-48);}
 else if (c2>='A' && c2<='F') { r2 = (c2-55);}
 else if (c2>='a' && c2<='f') { r2 = (c2-87);}
 return r |r2;
}

//*****************************************************************************************************
//converte dois caracteres formato hexa em um inteiro
unsigned int hextoint(char c1, char c2, char c3, char c4)
{
 unsigned int ri;
 ri = hextochar(c1,c2);
 ri= ri<<8;
 ri = ri | hextochar(c3,c4);
 return ri;
}


//*****************************************************************************************************
void rtc(void){

   DS1307_SetTimeZone(-4, 00);
   DS1307_SetDate(23);
   DS1307_SetMonth(4);
   DS1307_SetYear(2021);
   DS1307_SetDayOfWeek(4);
   DS1307_SetHour(15);
   DS1307_SetMinute(24);
   DS1307_SetSecond(20);

}

//*****************************************************************************************************
void atu_rtc(void){


	date = DS1307_GetDate();
	month = DS1307_GetMonth();
	year = DS1307_GetYear();
    dow = DS1307_GetDayOfWeek();
    hour = DS1307_GetHour();
	minute = DS1307_GetMinute();
	second = DS1307_GetSecond();
	zone_hr = DS1307_GetTimeZoneHour();
	zone_min = DS1307_GetTimeZoneMin();


}


//*****************************************************************************************************
void tela(void){

	//sprintf(buffer, "ISO8601 FORMAT: %04d-%02d-%02dT%02d:%02d:%02d%+03d:%02d  %s\n",
	//				year, month, date, hour, minute, second, zone_hr, zone_min, DAYS_OF_WEEK[dow]);


	  sprintf(msg1, "%02d:%02d:%02d", hour, minute, second);
	  sprintf(msg2, "%02d-%02d-%04d", date, month, year);

	  sprintf(msg3, "%03hu", pipi);
	  //sprintf(msg4, "%03hu", Temp_byte_1);

	  SSD1306_GotoXY (0,0);
	  SSD1306_Puts (" teste eeprom rtc", &Font_7x10, 1);
	  SSD1306_DrawLine (0, 15, 129, 15, SSD1306_COLOR_WHITE);
	  SSD1306_GotoXY (5, 17);
	  SSD1306_Puts (msg4, &Font_7x10, 1);
	  SSD1306_GotoXY (1, 29);
	  SSD1306_Puts (msg1, &Font_11x18, 1);
	  //SSD1306_GotoXY (9, 45);
	  //SSD1306_Puts (" D2=", &Font_11x18, 1);
	  SSD1306_GotoXY (1, 45);
	  SSD1306_Puts (msg2, &Font_11x18, 1);
	  SSD1306_UpdateScreen();


}

//************************************************************************************************
//      main
//************************************************************************************************
int main(void)
{

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C2_Init();
  MX_TIM7_Init();
  MX_USART1_UART_Init();

  HAL_TIM_Base_Start_IT(&htim7);
  HAL_UART_Receive_IT(&huart1, (uint8_t *)UART1_rxBuffer, 40);



  SSD1306_Init();

  DS1307_Init(&hi2c2);


  SSD1306_GotoXY (0,0);
  SSD1306_Puts ("    Teste", &Font_11x18, 1);
  SSD1306_GotoXY (3, 30);
  SSD1306_Puts (" EEPROM rtc", &Font_11x18, 1);
  SSD1306_UpdateScreen(); //display

  dht();

  HAL_Delay (2000);

  SSD1306_Clear();


//************************************************************************************************
//        Loop
//************************************************************************************************
  while (1)
  {

	  if (bot1 == 0){
		   if (filtro != 0)
		   {
		      filtro --;
		      if (filtro == 0)
		      {
		      ledvd_on;
		      //EEPROM_Write(1, 0, dataw1, strlen((char *)dataw1));
		      //EEPROM_Write_NUM (6, 0, pipi);
		      }
		   }
	  }else ledvd_off;



	  if (bot2 == 0){
		  ledvm_on;
		  //EEPROM_Read(1, 0, datar1, strlen((char *)datar1));
		  //d2 = EEPROM_Read_NUM (6, 0);
		  //HAL_Delay (10);
	  }else ledvm_off;

	  //atu_rtc();

      tela();

      //dht();

	  HAL_Delay (100);

filtro = 1000;
  }
  /* USER CODE END 3 */
}


//************************************************************************************************
//      timer  100ms
//************************************************************************************************
void TIM7_IRQHandler(void)
{
  HAL_TIM_IRQHandler(&htim7);




}

//************************************************************************************************
//      usart1 RX
//************************************************************************************************
void USART1_IRQHandler(void)
{
  HAL_UART_IRQHandler(&huart1);

	data = huart1.Instance->RDR;

  if (data=='@')
    {
     ser_ptm=0;
     ser_rcm=1;
     ser_pcm=0;
    }
 else if (data=='#')
    {
     if (ser_rcm)
        {
  	  if (ser_ptm==8) ser_pcm=1;
        };
        ser_rcm=0;
        ser_ptm=0;
        }
     else
        {
        if (ser_rcm) {ser_bfm[ser_ptm++]= data; if (ser_ptm>10) {ser_ptm=0; ser_rcm=0;}};
        }


}

//************************************************************************************************





void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x00602173;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 50-1;     //99;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 0xffff-1;   //63999;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

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
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3|GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA3 PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB3 PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
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
