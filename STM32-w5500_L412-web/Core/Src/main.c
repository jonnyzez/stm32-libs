//*****************************************************************************
//             teste w5500
//*****************************************************************************
#include "main.h"
#include "stm32l4xx_hal.h"
/* USER CODE BEGIN Includes */
/* vim: set ai et ts=4 sw=4: */
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <ctype.h>
#include "socket.h"
#include "dhcp.h"
#include "dns.h"
#include <stdio.h>
#include <stdbool.h>
#include "ssd1306.h"
#include "httpServer.h"
#include "webpages.h"
#include "httpUtil.h"


/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_SPI1_Init(void);

//************************************************************************************************
//      Macros
//************************************************************************************************

#define ledvd_on   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
#define ledvd_off  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
#define ledvm_on   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
#define ledvm_off  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);

#define b1 HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_6)
#define b2 HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_7)
#define jp HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_15)

#define DHCP_SOCKET     0
#define DNS_SOCKET      1
#define HTTP_SOCKET     2

//************************************************************************************************
//      var globais
//************************************************************************************************
uint8_t UART1_rxBuffer[6] = {0};

char msg1[6];
char msg2[6];
char msg3[6];
uint8_t msg4[50]={"rola"};

uint8_t vem[50];

uint16_t c1=0;
uint16_t c2=0;
uint16_t c3=0;
uint16_t c4=0;

uint8_t msg[]={"VAI CARAI !!"};

uint8_t ip_s[4]={192,168,0,33};
uint16_t port_s=9292;
uint16_t port=9291;

uint16_t len;

char pipi[6];

char data;

unsigned char ser_bfm[48];
bool ser_pcm;
bool ser_rcm;
unsigned char ser_ptm;

const unsigned char tbh[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};


//***********************************************************************************************
//           usart tx
//***********************************************************************************************
void UART_Printf(const char* fmt, ...) {
    char buff[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buff, sizeof(buff), fmt, args);
    HAL_UART_Transmit(&huart1, (uint8_t*)buff, strlen(buff),
                      HAL_MAX_DELAY);
    va_end(args);
}

//***********************************************************************************************
//           spi sel
//***********************************************************************************************
void W5500_Select(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
}

void W5500_Unselect(void) {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
}

void W5500_ReadBuff(uint8_t* buff, uint16_t len) {
    HAL_SPI_Receive(&hspi1, buff, len, HAL_MAX_DELAY);
}

void W5500_WriteBuff(uint8_t* buff, uint16_t len) {
    HAL_SPI_Transmit(&hspi1, buff, len, HAL_MAX_DELAY);
}

uint8_t W5500_ReadByte(void) {
    uint8_t byte;
    W5500_ReadBuff(&byte, sizeof(byte));
    return byte;
}

void W5500_WriteByte(uint8_t byte) {
    W5500_WriteBuff(&byte, sizeof(byte));
}

volatile bool ip_assigned = false;

void Callback_IPAssigned(void) {
    UART_Printf("Callback: IP assigned! Leased time: %d sec\r\n", getDHCPLeasetime());
    ip_assigned = true;
}

void Callback_IPConflict(void) {
    UART_Printf("Callback: IP conflict!\r\n");
}


uint8_t predefined_get_cgi_processor(uint8_t * uri_name, uint8_t * buf, uint16_t * len){
	if(strcmp((const char *)uri_name, "led1.cgi") == 0){
	    HAL_GPIO_TogglePin (GPIOB, GPIO_PIN_3);

	    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_3)) {
	    	*len = sprintf((char *)buf, led1_on);
	    	  SSD1306_GotoXY (3, 40);
	    	  SSD1306_Puts ("  ligado   ", &Font_11x18, 1);
	    	  SSD1306_UpdateScreen();
	    } else {
	    	*len = sprintf((char *)buf, led1_off);
	    	  SSD1306_GotoXY (3, 40);
	    	  SSD1306_Puts (" desligado", &Font_11x18, 1);
	    	  SSD1306_UpdateScreen();
	    }

	}

	if(strcmp((const char *)uri_name, "led2.cgi") == 0){
	    HAL_GPIO_TogglePin (GPIOB, GPIO_PIN_4);

	    if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4)) {
	    	*len = sprintf((char *)buf, led2_on);
	    	  SSD1306_GotoXY (3, 40);
	    	  SSD1306_Puts ("  ligado   ", &Font_11x18, 1);
	    	  SSD1306_UpdateScreen();
	    } else {
	    	*len = sprintf((char *)buf, led2_off);
	    	  SSD1306_GotoXY (3, 40);
	    	  SSD1306_Puts (" desligado", &Font_11x18, 1);
	    	  SSD1306_UpdateScreen();
	    }

	}
	return 1;
}

uint8_t predefined_set_cgi_processor(uint8_t * uri_name, uint8_t * uri, uint8_t * buf, uint16_t * len){
	return 0;
}

//***********************************************************************************************
//           init udp
//***********************************************************************************************
void init_udp() {
    UART_Printf("\r\ninit() called!\r\n");

    UART_Printf("Registering W5500 callbacks...\r\n");
    reg_wizchip_cs_cbfunc(W5500_Select, W5500_Unselect);
    reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_WriteByte);
    reg_wizchip_spiburst_cbfunc(W5500_ReadBuff, W5500_WriteBuff);

    UART_Printf("Calling wizchip_init()...\r\n");
    uint8_t rx_tx_buff_sizes[] = {2, 2, 2, 2, 2, 2, 2, 2};
    wizchip_init(rx_tx_buff_sizes, rx_tx_buff_sizes);

    UART_Printf("Calling Static_ip_init()...\r\n");
    wiz_NetInfo net_info = {
        .mac  = { 0xEA, 0x11, 0x22, 0x33, 0x44, 0xEA },
        .ip = {192,168,0,81},
		.sn = {255,255,255,0},
        .gw = {192,168,0,1},
		.dns = {8,8,8,8}

    };

    setSHAR(net_info.mac);
    setSIPR(net_info.ip);
    setSUBR(net_info.sn);
    setGAR(net_info.gw);
    setSn_PORT(1,port);
    socket(1, Sn_MR_UDP, port, 1);

    SSD1306_Clear();
    SSD1306_GotoXY (0,1);
    SSD1306_Puts ("  Modo UDP", &Font_11x18, 1);
    SSD1306_GotoXY (3, 20);
    SSD1306_Puts ("ip 192.168.0.81", &Font_7x10, 1);
    SSD1306_UpdateScreen();

}

//***********************************************************************************************
//          init web
//***********************************************************************************************
void init_web(void){
uint8_t bufSize[] = {2, 2, 2, 2};
reg_wizchip_cs_cbfunc(W5500_Select, W5500_Unselect);
reg_wizchip_spi_cbfunc(W5500_ReadByte, W5500_WriteByte);
wizchip_init(bufSize, bufSize);
wiz_NetInfo netInfo = { .mac 	= {0x00, 0x08, 0xdc, 0xab, 0xcd, 0xef},
                         .ip 	= {192, 168, 0, 82},
                         .sn 	= {255, 255, 255, 0},
                         .gw 	= {192, 168, 0, 1}};
 wizchip_setnetinfo(&netInfo);
 uint8_t socknumlist[] = {2, 3, 4, 5, 6, 7};
   #define DATA_BUF_SIZE 4096
   #define MAX_HTTPSOCK 6
   uint8_t RX_BUF[DATA_BUF_SIZE];
   uint8_t TX_BUF[DATA_BUF_SIZE];
   httpServer_init(TX_BUF, RX_BUF, MAX_HTTPSOCK, socknumlist);
   reg_httpServer_cbfunc(NVIC_SystemReset, NULL);
   reg_httpServer_webContent((uint8_t *)"index.html", (uint8_t *)index_page);

   SSD1306_Clear();
   SSD1306_GotoXY (0,1);
   SSD1306_Puts ("  Modo Web", &Font_11x18, 1);
   SSD1306_GotoXY (3, 20);
   SSD1306_Puts ("ip 192.168.0.82", &Font_7x10, 1);
   SSD1306_UpdateScreen();

}


//***********************************************************************************************
//         evia udp
//***********************************************************************************************
void envia_udp(void)
{

    sendto(1,msg,sizeof(msg),ip_s,port_s);
	//close(1);

}
//***********************************************************************************************
//          recebe udp
//***********************************************************************************************
void recebe_udp(void)
{

	recvfrom(1,msg4,7,ip_s,port);
	UART_Printf(msg4);

}

//************************************************************************************************
//        main
//************************************************************************************************

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_I2C1_Init();
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  HAL_UART_Receive_IT(&huart1, (uint8_t *)UART1_rxBuffer, 40);

  SSD1306_Init();
  SSD1306_GotoXY (0,1);
  SSD1306_Puts ("    Teste", &Font_11x18, 1);
  SSD1306_GotoXY (3, 30);
  SSD1306_Puts (" UDP /  WEB", &Font_11x18, 1);
  SSD1306_UpdateScreen();
  HAL_Delay (1500);
  //SSD1306_Clear();


  if(!jp){init_web();HAL_Delay(100);}else{init_udp();}



//************************************************************************************************
//        Loop
//************************************************************************************************
  while (1)
  {

	  for(uint16_t i = 0; i < MAX_HTTPSOCK; i++)httpServer_run(i);

			if(getSn_IR(1) & Sn_IR_RECV)
		      {
		  	  setSn_IR(1, Sn_IR_RECV);
			  }
			if((len=getSn_RX_RSR(1))>0)
			  {
			 memset(msg4,1,len+1);
			 len = recvfrom(1,msg4, len, ip_s,&port);
			 sendto(1,msg4,len, ip_s, port_s);
			 }
			//UART_Printf(msg4);

	  if(!b1){envia_udp();HAL_Delay(1000);}
	  if(!b2){recebe_udp();HAL_Delay(1000);}

	  //ledvd_on;
      //HAL_Delay(500);
      //ledvd_off;
      HAL_Delay(500);


  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/

















void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00602173;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
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
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3|GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA5 */
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PA6 PA7 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
