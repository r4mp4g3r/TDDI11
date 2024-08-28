/**
  ******************************************************************************
  * @file    main.c
  * @author  A.Rezine
  * @brief   Tic-Tac-Toe Implementation based on shipped STM32L562E-DK BSP demo
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stdio.h"
#include "string.h"


/* Exported variables --------------------------------------------------------*/
__IO FlagStatus UserButtonPressed = RESET;
__IO FlagStatus TouchDetected     = RESET;

/* Private typedef -----------------------------------------------------------*/
typedef enum {WELCOME, BLUE_QUESTION, BLUE_ANSWER, BLUE_UPDATE, \
	RED_QUESTION, RED_ANSWER, RED_UPDATE, BLUE_WINNER, \
	RED_WINNER, NO_WIN, EXIT} Game_States;

typedef enum {FREE, RED, BLUE} Cell_State;

/* Private define ------------------------------------------------------------*/
#define  RADIUS        		29


/* Private macro -------------------------------------------------------------*/
#define  XPOS(i)       (60 + i * 60)
#define  YPOS(j)       (30 + j * 60)


/* Private variables ---------------------------------------------------------*/

char liuids[30] = "yonta612 and rampa765";


uint16_t   x, y;
uint8_t ix, iy;
uint8_t Touch = 	0;
uint8_t Valid = 	0;
uint8_t Win = 		0;
uint8_t Full = 		0;
uint8_t Button =	0;

Game_States State = WELCOME;
Game_States PreviousState = EXIT;

Cell_State BoardState[9];

/* Components Initialization Status */
FlagStatus ButtonInitialized = RESET;
FlagStatus LcdInitialized = RESET;
FlagStatus LedInitialized = RESET;
FlagStatus TsInitialized  = RESET;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void SystemHardwareInit(void);

void Tic_Tac_Toe(void);

void Initialize_Board(Cell_State BoardState[]);

static uint8_t Check_Move_Validity(uint16_t x, uint16_t y, Cell_State BoardState[], uint8_t* p_choice_i, uint8_t* p_choice_j);
static uint8_t Check_Win(Cell_State BoardState[]);
static uint8_t Check_Full(Cell_State BoardState[]);

static void Clear_Display_Welcome(void);
static void Clear_Display_Board(Cell_State BoardState[]);
static void Display_Text(uint8_t* text);
static void Display_Other_Text(uint8_t* text);


/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program with an FSM implementation of TicTacToe
  * @param  None
  * @retval None
  */
int main(void)
{
	// general initializations
	HAL_Init();
	SystemClock_Config();
	SystemHardwareInit();

	// Touch state
	TS_State_t TsState = {0};

	Initialize_Board(BoardState);


	  while (1)
	  {

		  {//Read touch and Button. Do not modify.
			  if (TouchDetected == SET)
			  {// Detects screen touch (PersistentTouch)
				TouchDetected = RESET;

				if (TicTacToe_TS_GetState(0, &TsState) != TicTacToe_ERROR_NONE)
				{
				  Error_Handler();
				}
				if(TsState.TouchDetected >= 1)
				{
					x = TsState.TouchX;
					y = TsState.TouchY;
					Touch = 1;
				}
			  }

			  if (UserButtonPressed == SET)
			  {//Detects button pressure (PersistentButton).
				UserButtonPressed = RESET;
				Button = 1;
			  }
		  }

		  // ************************ LAB: Do not modify before this line **********************
		  //Update State on entry
		  if(State != PreviousState){
			  switch(State)
			  {
				  case WELCOME:
					  Initialize_Board(BoardState);
					  Touch = 0;
					  Clear_Display_Welcome();
					  Display_Text((uint8_t *)"Touch screen to start new game");
					  Display_Other_Text((uint8_t *)"Always exit with push-button");
					  break;
				  case BLUE_QUESTION:
					  Touch = 0;
					  Clear_Display_Board(BoardState);
					  Display_Text((uint8_t*)"Blue's move");
					  break;
				  case BLUE_ANSWER:
					  Valid = Check_Move_Validity(x, y, BoardState, &ix, &iy);
					  break;
				  case BLUE_UPDATE:
					  BoardState[ix + 3 * iy] = BLUE;
					  Win = Check_Win(BoardState); //
					  Full = Check_Full(BoardState); //
					  break;
				  case RED_QUESTION:
					  Touch = 0; //resets touch screen
					  Clear_Display_Board(BoardState);
					  Display_Text((uint8_t*)"Red's move");
					  break;
				  case RED_ANSWER:
					  Valid = Check_Move_Validity(x, y, BoardState, &ix, &iy); //updates move validity
					  break;
				  case RED_UPDATE:
					  BoardState[ix + 3 * iy] = RED;
					  Win = Check_Win(BoardState); //updates win condition
					  Full = Check_Full(BoardState); //updates board full and draw condition
					  break;
				  case BLUE_WINNER:
					  Touch = 0; //resets touchscreen
					  Clear_Display_Board(BoardState); //clears screen
					  Display_Text((uint8_t*)"Blue wins! Touch screen to return to start"); //prints blue wins
					  break;
				  case RED_WINNER:
					  Touch = 0; //resets touchscreen
					  Clear_Display_Board(BoardState);
					  Display_Text((uint8_t*)"Red wins! Touch screen to return to start"); //prints red wins
					  break;
				  case NO_WIN:
					  Touch = 0; //resets touchscreen
					  Clear_Display_Board(BoardState); //clears screen
					  Display_Text((uint8_t*)"Draw! Touch screen to return to start"); //prints no winners
					  break;
				  case EXIT:
				  default:
					  Clear_Display_Welcome();
					  Display_Text((uint8_t*)"Good bye");
//					  HAL_Delay(5000);
					  return 0;
			  } //update state
		  }//on entry

		  // transitions
		  switch(State)
		  {
			  case WELCOME:
				  PreviousState = WELCOME;
				  if(Button){
					  State = EXIT;
				  }else if(Touch){
					  State = BLUE_QUESTION;
				  }
				  break;
			  case BLUE_QUESTION:
				  PreviousState = BLUE_QUESTION;
				  if(Button){
					  State = EXIT;
				  }else if(Touch){
					  State = BLUE_ANSWER;
				  }
				  break;
			  case BLUE_ANSWER:
				  PreviousState = BLUE_ANSWER; //check if previousstate is correct!!!
				  if(Button){
					  State = EXIT;
				  }else if(Valid){
					  State = BLUE_UPDATE;
				  }else{
					  State = BLUE_QUESTION;
				  }
				  break;
			  case BLUE_UPDATE:
				  PreviousState = BLUE_UPDATE;
				  if(Button){
					  State = EXIT;
				  }else if(Win){
					  State = BLUE_WINNER;
				  }else if (Full){
					  State = NO_WIN;
				  }else if(Touch){
					  State = RED_QUESTION;
				  }
				  break;
			  case RED_QUESTION:
				  PreviousState = RED_QUESTION;
				  if(Button){
					  State = EXIT;
				  }else if(Touch){
					  State = RED_ANSWER;
				  }
				  break;
			  case RED_ANSWER:
				  PreviousState = RED_ANSWER;
				  if(Valid){
					  State = RED_UPDATE;
				  }else{
					  State =RED_QUESTION;
				  }
				  break;
			  case RED_UPDATE:
				  PreviousState = RED_UPDATE;
				  if(Button){
					  State = EXIT;
				  }else if(Win){
					  State = RED_WINNER;
				  }else if(Full){
					  State = NO_WIN;
				  }else if(Touch){
					  State = BLUE_QUESTION;
				  }
				  break;
			  case RED_WINNER:
				  PreviousState = RED_WINNER;
				  if(Button){
					  State = EXIT;
				  }else if(Touch){
					  State = WELCOME;
				  }
				  break;
			  case NO_WIN:
				  PreviousState = NO_WIN;
				  if(Button){
					  State = EXIT;
				  }else if(Touch){
					  State = WELCOME;
				  }
				  break;
			  case BLUE_WINNER:
				  PreviousState = BLUE_WINNER;
				  if(Button){
					  State = EXIT;
				  }else if(Touch){
					  State = WELCOME;
				  }
				  break;

			  default:
				  PreviousState = EXIT;
				  State = WELCOME;

		  }// transitions

		  // ************************ LAB: Do not modify after this line **********************


	  } // infinite loop
} //end main



static void Clear_Display_Welcome(void)
{
  UTIL_LCD_SetFont(&Font24);
  UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLACK);
  UTIL_LCD_DisplayStringAt(0, 100, (uint8_t *)"Tic-Tac-Toe!!", CENTER_MODE);
  UTIL_LCD_SetFont(&Font16);
  UTIL_LCD_DisplayStringAt(0, 140, (uint8_t *)liuids, CENTER_MODE);
}


static void Clear_Display_Board(Cell_State BoardState[])
{
  UTIL_LCD_Clear(UTIL_LCD_COLOR_YELLOW);
  UTIL_LCD_FillRect(0, 180, 240, 240, UTIL_LCD_COLOR_BLACK);
  for(int i=0; i < 3; i++){
	  for(int j=0; j < 3; j++){
			switch (BoardState[i + 3 * j])
			{
				case BLUE:
					UTIL_LCD_FillCircle(XPOS(i), YPOS(j), RADIUS-3, UTIL_LCD_COLOR_BLUE);
					break;
				case RED:
					UTIL_LCD_FillCircle(XPOS(i), YPOS(j), RADIUS-3, UTIL_LCD_COLOR_RED);
					break;
				default:
					UTIL_LCD_FillCircle(XPOS(i), YPOS(j), RADIUS-2, UTIL_LCD_COLOR_WHITE);
			}
		}
	}
}


static void Display_Text(uint8_t* text)
{
  UTIL_LCD_SetFont(&Font16);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLACK);
  UTIL_LCD_DisplayStringAt(0, 195, text, CENTER_MODE);
}

static void Display_Other_Text(uint8_t* text)
{
  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLACK);
  UTIL_LCD_DisplayStringAt(0, 225, text, CENTER_MODE);
}


void Initialize_Board(Cell_State BoardState[]){
  for(int i=0; i < 9; i++){
	  BoardState[i] = FREE;
  }
}


static uint8_t Check_Move_Validity(uint16_t x, uint16_t y, Cell_State BoardState[], \
					uint8_t* p_choice_i, uint8_t* p_choice_j){
	for(int i=0; i < 3; i++)
	{
		for(int j=0; j < 3; j++)
		{
			if ((y > (YPOS(j) - RADIUS)) &&
				(y < (YPOS(j) + RADIUS)))
			{

			  if ((x > (XPOS(i) - RADIUS)) &&
				  (x < (XPOS(i) + RADIUS)))
			  {
				if (BoardState[i + 3*j] == FREE)
				{
					*p_choice_i = i;
					*p_choice_j = j;
					return 1;
				}
			  }
			}
		}
	}
	* p_choice_i = -1;
	* p_choice_j = -1;
	return 0;
}


static uint8_t Check_Win(Cell_State BoardState[]){

	// check central
	if(BoardState[1 + 3 * 1] != FREE){
		for(int i=0; i < 4; i++)
		{
			if((BoardState[i] == BoardState[4]) && (BoardState[4]==BoardState[8-i])){
				return 1;
			}
		}
	}
	// check periphery
	if(BoardState[0] != FREE){
		if((BoardState[0] == BoardState[1]) && (BoardState[0] == BoardState[2])){
			return 1;
		}
		if((BoardState[0] == BoardState[3]) && (BoardState[0] == BoardState[6])){
			return 1;
		}
	}
	if(BoardState[8] != FREE){
		if((BoardState[8] == BoardState[7]) && (BoardState[8] == BoardState[6])){
			return 1;
		}
		if((BoardState[8] == BoardState[5]) && (BoardState[8] == BoardState[2])){
			return 1;
		}
	}

	return 0;
}

static uint8_t Check_Full(Cell_State BoardState[]){

	for(uint8_t i = 0; i < 9; i++){
		if(BoardState[i] == FREE){
			return 0;
		}
	}
	return 1;
}


/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follows :
  *            System Clock source            = PLL (MSI)
  *            SYSCLK(Hz)                     = 110000000
  *            HCLK(Hz)                       = 110000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 1
  *            APB2 Prescaler                 = 1
  *            MSI Frequency(Hz)              = 4000000
  *            PLL_M                          = 1
  *            PLL_N                          = 55
  *            PLL_Q                          = 2
  *            PLL_R                          = 2
  *            PLL_P                          = 2
  *            Flash Latency(WS)              = 5
  * @param  None
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* Enable voltage range 0 for frequency above 80 Mhz */
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE0);
  __HAL_RCC_PWR_CLK_DISABLE();

  /* MSI Oscillator enabled at reset (4Mhz), activate PLL with MSI as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 55;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* To avoid undershoot due to maximum frequency, select PLL as system clock source */
  /* with AHB prescaler divider 2 as first step */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* AHB prescaler divider at 1 as second step */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
}

/**
  * @brief  System Power Configuration at Boot
  * @param  None
  * @retval None
  */
static void SystemHardwareInit(void)
{
  /* Init LEDs  */
  if (LedInitialized != SET)
  {
    if (TicTacToe_LED_Init(LED9) != TicTacToe_ERROR_NONE)
    {
      Error_Handler();
    }
    if (TicTacToe_LED_Init(LED10) != TicTacToe_ERROR_NONE)
    {
      Error_Handler();
    }
    LedInitialized = SET;
  }

  /* Init User push-button in EXTI Mode */
  if (ButtonInitialized != SET)
  {
    if (TicTacToe_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI) != TicTacToe_ERROR_NONE)
    {
      Error_Handler();
    }
    ButtonInitialized = SET;
  }

  /* Initialize the LCD */
  if (LcdInitialized != SET)
  {
    LCD_UTILS_Drv_t lcdDrv;

    /* Initialize the LCD */
    if (TicTacToe_LCD_Init(0, LCD_ORIENTATION_PORTRAIT) != TicTacToe_ERROR_NONE)
    {
      Error_Handler();
    }

    /* Set UTIL_LCD functions */
    lcdDrv.DrawBitmap  = TicTacToe_LCD_DrawBitmap;
    lcdDrv.FillRGBRect = TicTacToe_LCD_FillRGBRect;
    lcdDrv.DrawHLine   = TicTacToe_LCD_DrawHLine;
    lcdDrv.DrawVLine   = TicTacToe_LCD_DrawVLine;
    lcdDrv.FillRect    = TicTacToe_LCD_FillRect;
    lcdDrv.GetPixel    = TicTacToe_LCD_ReadPixel;
    lcdDrv.SetPixel    = TicTacToe_LCD_WritePixel;
    lcdDrv.GetXSize    = TicTacToe_LCD_GetXSize;
    lcdDrv.GetYSize    = TicTacToe_LCD_GetYSize;
    lcdDrv.SetLayer    = TicTacToe_LCD_SetActiveLayer;
    lcdDrv.GetFormat   = TicTacToe_LCD_GetFormat;
    UTIL_LCD_SetFuncDriver(&lcdDrv);

    /* Clear the LCD */
    UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

    /* Set the display on */
    if (TicTacToe_LCD_DisplayOn(0) != TicTacToe_ERROR_NONE)
    {
      Error_Handler();
    }

    LcdInitialized = SET;
  }

  /* Initialize the TouchScreen */
  if (TsInitialized != SET)
  {
    TS_Init_t TsInit;

    /* Initialize the TouchScreen */
    TsInit.Width       = 240;
    TsInit.Height      = 240;
    TsInit.Orientation = TS_ORIENTATION_PORTRAIT;
    TsInit.Accuracy    = 10;
    if (TicTacToe_TS_Init(0, &TsInit) != TicTacToe_ERROR_NONE)
    {
      Error_Handler();
    }

    /* Configure TS interrupt */
    if (TicTacToe_TS_EnableIT(0) != TicTacToe_ERROR_NONE)
    {
      Error_Handler();
    }

    TsInitialized = SET;
  }
}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  while(1)
  {
    /* Toggle LED9 */
    (void) TicTacToe_LED_Toggle(LED9);
    HAL_Delay(500);
  }
}

/**
  * @brief  TicTacToe Push Button callback
  * @param  Button Specifies the pin connected EXTI line
  * @retval None.
  */
void TicTacToe_PB_Callback(Button_TypeDef Button)
{
  if (Button == BUTTON_USER)
  {
    UserButtonPressed = SET;
  }
}

/**
  * @brief  TS Callback.
  * @param  Instance TS Instance.
  * @retval None.
  */
void TicTacToe_TS_Callback(uint32_t Instance)
{
  if (Instance == 0)
  {
    TouchDetected = SET;
  }
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
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

