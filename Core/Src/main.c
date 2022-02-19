#include "main.h"
#include "hts221.h"

void SystemClock_Config(void);
void I2C1Init();

void Pb0_Config(void);
void Pb7_Config(void);
void Pb14_Config(void);

void Set_Green_Diode(void);
void Set_Blue_Diode(void);
void Set_Red_Diode(void);

void Reset_Green_Diode(void);
void Reset_Blue_Diode(void);
void Reset_Red_Diode(void);

int main(void)
{
  // GPIOF clock enable for data ready signal from hts221
  RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOFEN;

  HAL_Init();
  SystemClock_Config();

  // I2C configuration
  I2C1Init();

  Pb0_Config();
  Pb7_Config();
  Pb14_Config();

  // Humidity temperature sensor configuration
  hts221_init(0x3F, 0x85, 0x00, 0x04);

  while (1)
  {
	  if(GPIOF -> IDR & 0x1000){
		  float temperature, humidity;

		  // Reading humidity, temperature
		  temperature = (float)(hts221_get_temperature() / 10);
		  humidity = (float)(hts221_get_humidity() / 10);
	  }

  }

}

void I2C1Init(){
	RCC -> AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
	GPIOB -> MODER |= GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1;
	GPIOB -> OTYPER |= GPIO_OTYPER_OT8 | GPIO_OTYPER_OT9;
	GPIOB -> OSPEEDR |= GPIO_OSPEEDR_OSPEEDR8 | GPIO_OSPEEDR_OSPEEDR9;
	GPIOB -> PUPDR = 0;
	GPIOB -> AFR[1] |= GPIO_AF4_I2C1 | (GPIO_AF4_I2C1 << 4);

	RCC -> APB1ENR |= RCC_APB1ENR_I2C1EN;
	I2C1 -> CR1 &= ~I2C_CR1_PE;
	while(I2C1 -> CR1 & I2C_CR1_PE);
	I2C1 -> TIMINGR = (0x9 | (0x3 << 8) | (0x2 << 16) | (0x3 << 20) | (0x1 << 28));
	I2C1 -> CR1 = I2C_CR1_PE | I2C_CR1_NACKIE;
}
// Light up red diode
void Set_Green_Diode() {
	GPIOB->BSRR |= GPIO_BSRR_BS0;
}
// Light up blue diode
void Set_Blue_Diode() {
	GPIOB->BSRR |= GPIO_BSRR_BS7;
}
// Light up green diode
void Set_Red_Diode() {
	GPIOB->BSRR |= GPIO_BSRR_BS14;
}
// Turn off red diode
void Reset_Green_Diode(void) {
	GPIOB -> BSRR = 1<<16;
}
// Turn off blue diode
void Reset_Blue_Diode(void) {
	GPIOB -> BSRR = 1<<23;
}
// Turn off green diode
void Reset_Red_Diode(void) {
	GPIOB -> BSRR = 1<<30;
}
// Red diode configuration
void Pb0_Config(void) {
	GPIOB->MODER |= GPIO_MODER_MODER0_0;
}
// Blue diode configuration
void Pb7_Config(void) {
	GPIOB->MODER |= GPIO_MODER_MODER7_0;
}
// Green diode configuration
void Pb14_Config(void) {
	GPIOB->MODER |= GPIO_MODER_MODER14_0;
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
