
#include "stm32f10x.h"

#define IN_MS_TO_POPUGAIS(MS)	(2 * MS - 1)

void led_init(void);
void buttons_init(void);
void timer_init(void);

uint16_t timer_com = IN_MS_TO_POPUGAIS(500);
uint16_t timer_pause = IN_MS_TO_POPUGAIS(500);

int main(void)
{
	led_init(); 	//Инициализируем LED (С13)
	buttons_init(); //Инициализируем кнопки 1-4
	timer_init();   //Инициализируем таймер 3

	uint32_t last_state = GPIOC->IDR & GPIO_IDR_IDR14;

	for(;;)
	{
		uint32_t curr_state = GPIOC->IDR & GPIO_IDR_IDR14; //cчитываем логическое состояние вывода 14

		if (curr_state != last_state)  //если произошла смена состояния
		{
			TIM3->CR1 &= ~TIM_CR1_CEN; // остановаить отсчет
			GPIOC->ODR |= GPIO_ODR_ODR13; //выключить диод
			if (curr_state == 0) //если кнопка нажата
			{
				TIM3->ARR = UINT16_MAX; // устанавливаем  максимальное значение таймера
			}
			else
			{
				if (TIM3->CNT >= IN_MS_TO_POPUGAIS(100)) //если счетчик проходит фильтр
				{
					timer_com = TIM3->CNT; // время коммутации приравнивем счетчику таймера
				}

				TIM3->ARR = timer_com; // максимальное значение таймера приравниваем к времени горения диода
			}
			TIM3->CNT = 0; // обнулить счетчик

			TIM3->CR1 |= TIM_CR1_CEN; // начать отсчет

			last_state = curr_state;
		}
// установка времени горения в зависимости от кнопки
	if ((GPIOA->IDR & GPIO_IDR_IDR4)==0)
		{
		timer_pause=IN_MS_TO_POPUGAIS(1000);
		}
	else if ((GPIOA->IDR & GPIO_IDR_IDR7)!=0)
		{
		timer_pause=IN_MS_TO_POPUGAIS(1500);
		}
	else if ((GPIOC->IDR & GPIO_IDR_IDR15)==0)
		{
		timer_pause=IN_MS_TO_POPUGAIS(2000);
		}
	else
		{
		timer_pause=IN_MS_TO_POPUGAIS(500);
		}
	}
}

void TIM3_IRQHandler(void)
//Функция обработчика прерывания от таймера 3
{
//  Если произошло прерывание по переполнению таймера
//	Сбрасываем флаг переполнения
	if (TIM3->SR & TIM_SR_UIF)
	{
		TIM3->SR &= ~TIM_SR_UIF; // Clean UIF Flag
		// Считываем логическое состояние вывода светодиода и инвертируем состояние
		if ( GPIOC->ODR & GPIO_ODR_ODR13 ) //Если диод НЕ горит
		{
			GPIOC->BSRR = GPIO_BSRR_BR13;  //Reset13 сбрось и тем самым зажги
			TIM3->ARR = timer_pause; //приравниваем максимальное значение таймера к времени горения диода
		}
		else
		{
			GPIOC->BSRR = GPIO_BSRR_BS13; //Set13 и выключи (состояние а)
			TIM3->ARR = timer_com; // приравниваем максимальное значение таймера к времени коммутации диода
		}
	}
}

void led_init(void)
{
//  Включаем тактирование порта C
//	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
//  сброс состояния порта С pin 13
//	GPIOC->CRH &= ~(GPIO_CRH_CNF13_1 | GPIO_CRH_CNF13_0 | GPIO_CRH_MODE13_1 | GPIO_CRH_MODE13_0);
//	GPIO_ResetBits(GPIOC, GPIO_Pin_13);

//  настраиваем LED как выход (push/pull out speed 2MHz) RM p.160
//	GPIOC->CRH |= GPIO_CRH_MODE13_1;

	GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;  //  использовать выводы PC8 и PC9
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //  на выход
	GPIO_Init(GPIOC, &GPIO_InitStructure);


}

void buttons_init(void)
// В этой инициализируем кнопки 1-4
{
	// Включаем тактирование недостающих портов А и В (C уже включен)
	//RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN; //  | RCC_APB2ENR_IOPCEN;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	// сброс состояний портов
//	GPIOC->CRH &=~(GPIO_CRH_CNF14_1 | GPIO_CRH_CNF14_0 | GPIO_CRH_MODE14_1 | GPIO_CRH_MODE14_0 |
//							  //
//				   GPIO_CRH_CNF15_1 | GPIO_CRH_CNF15_0 | GPIO_CRH_MODE15_1 | GPIO_CRH_MODE15_0);
//	//
//	GPIOA->CRL &=~(GPIO_CRL_CNF4_1 | GPIO_CRL_CNF4_0 | GPIO_CRL_MODE4_1 | GPIO_CRL_MODE4_0 |
//
//					GPIO_CRL_CNF7_1 | GPIO_CRL_CNF7_0 | GPIO_CRL_MODE7_1 | GPIO_CRL_MODE7_0);

	// Настройка кнопки 3, как вход с подтяжкой к земле
//		GPIOA->CRL |= GPIO_CRL_CNF7_1;
//		GPIOA->ODR &=~GPIO_ODR_ODR7;

	GPIO_InitTypeDef GPIO_InitStructure_button3;
			GPIO_InitStructure_button3.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_InitStructure_button3.GPIO_Pin = GPIO_Pin_7;  //
			GPIO_InitStructure_button3.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStructure_button3);

	 //
	// Настройка кнопки 1,2,4, как вход с подтяжкой к плюсу
//	GPIOC->CRH |= GPIO_CRH_CNF14_1;
//	GPIOC->ODR |= GPIO_ODR_ODR14; //кнопка 1

	GPIO_InitTypeDef GPIO_InitStructure_button1;
			GPIO_InitStructure_button1.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_InitStructure_button1.GPIO_Pin = GPIO_Pin_14;  //
			GPIO_InitStructure_button1.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure_button1);


//	GPIOA->CRL |= GPIO_CRL_CNF4_1;
//	GPIOA->ODR |= GPIO_ODR_ODR4; //кнопка 2
	//

	GPIO_InitTypeDef GPIO_InitStructure_button2;
				GPIO_InitStructure_button2.GPIO_Speed = GPIO_Speed_2MHz;
				GPIO_InitStructure_button2.GPIO_Pin = GPIO_Pin_4;  //
				GPIO_InitStructure_button2.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure_button2);


//	GPIOC->CRH |= GPIO_CRH_CNF15_1;
//	GPIOC->ODR |= GPIO_ODR_ODR15; //кнопка 4

	GPIO_InitTypeDef GPIO_InitStructure_button4;
					GPIO_InitStructure_button4.GPIO_Speed = GPIO_Speed_2MHz;
					GPIO_InitStructure_button4.GPIO_Pin = GPIO_Pin_15;  //
					GPIO_InitStructure_button4.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure_button4);
}


void timer_init(void)
//инициализация таймера 3
{
//  Включаем тактирование
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
//  Разрешаем прерывания по переполнению таймера
	TIM3->DIER |= TIM_DIER_UIE; // Update  Interrupt Enable
//  Включение прерывания таймера
	NVIC_EnableIRQ(TIM3_IRQn);
//  Запускаем таймер 3 на тактовой частоте в 1 kHz
	TIM3->PSC = 36000 - 1;
//  Период - 1000 тактов => 1000/1000 = 1 Hz
	TIM3->ARR = 1000 - 1;
	TIM3->CR1 |= TIM_CR1_CEN; // Start count
}
