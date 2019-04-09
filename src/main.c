
#include "stm32f10x.h"

#define IN_MS_TO_POPUGAIS(MS)	(2 * MS - 1)

void led_init(void);
void buttons_init(void);
void timer_init(void);

uint16_t timer_com = IN_MS_TO_POPUGAIS(500);
uint16_t timer_pause = IN_MS_TO_POPUGAIS(500);

int main(void)
{
	led_init(); 	//�������������� LED (�13)
	buttons_init(); //�������������� ������ 1-4
	timer_init();   //�������������� ������ 3

	uint32_t last_state = GPIOC->IDR & GPIO_IDR_IDR14;

	for(;;)
	{
		uint32_t curr_state = GPIOC->IDR & GPIO_IDR_IDR14; //c�������� ���������� ��������� ������ 14

		if (curr_state != last_state)  //���� ��������� ����� ���������
		{
			TIM3->CR1 &= ~TIM_CR1_CEN; // ����������� ������
			GPIOC->ODR |= GPIO_ODR_ODR13; //��������� ����
			if (curr_state == 0) //���� ������ ������
			{
				TIM3->ARR = UINT16_MAX; // �������������  ������������ �������� �������
			}
			else
			{
				if (TIM3->CNT >= IN_MS_TO_POPUGAIS(100)) //���� ������� �������� ������
				{
					timer_com = TIM3->CNT; // ����� ���������� ����������� �������� �������
				}

				TIM3->ARR = timer_com; // ������������ �������� ������� ������������ � ������� ������� �����
			}
			TIM3->CNT = 0; // �������� �������

			TIM3->CR1 |= TIM_CR1_CEN; // ������ ������

			last_state = curr_state;
		}
// ��������� ������� ������� � ����������� �� ������
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
//������� ����������� ���������� �� ������� 3
{
//  ���� ��������� ���������� �� ������������ �������
//	���������� ���� ������������
	if (TIM3->SR & TIM_SR_UIF)
	{
		TIM3->SR &= ~TIM_SR_UIF; // Clean UIF Flag
		// ��������� ���������� ��������� ������ ���������� � ����������� ���������
		if ( GPIOC->ODR & GPIO_ODR_ODR13 ) //���� ���� �� �����
		{
			GPIOC->BSRR = GPIO_BSRR_BR13;  //Reset13 ������ � ��� ����� �����
			TIM3->ARR = timer_pause; //������������ ������������ �������� ������� � ������� ������� �����
		}
		else
		{
			GPIOC->BSRR = GPIO_BSRR_BS13; //Set13 � ������� (��������� �)
			TIM3->ARR = timer_com; // ������������ ������������ �������� ������� � ������� ���������� �����
		}
	}
}

void led_init(void)
{
//  �������� ������������ ����� C
//	RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
//  ����� ��������� ����� � pin 13
//	GPIOC->CRH &= ~(GPIO_CRH_CNF13_1 | GPIO_CRH_CNF13_0 | GPIO_CRH_MODE13_1 | GPIO_CRH_MODE13_0);
//	GPIO_ResetBits(GPIOC, GPIO_Pin_13);

//  ����������� LED ��� ����� (push/pull out speed 2MHz) RM p.160
//	GPIOC->CRH |= GPIO_CRH_MODE13_1;

	GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;  //  ������������ ������ PC8 � PC9
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    //  �� �����
	GPIO_Init(GPIOC, &GPIO_InitStructure);


}

void buttons_init(void)
// � ���� �������������� ������ 1-4
{
	// �������� ������������ ����������� ������ � � � (C ��� �������)
	//RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN; //  | RCC_APB2ENR_IOPCEN;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	// ����� ��������� ������
//	GPIOC->CRH &=~(GPIO_CRH_CNF14_1 | GPIO_CRH_CNF14_0 | GPIO_CRH_MODE14_1 | GPIO_CRH_MODE14_0 |
//							  //
//				   GPIO_CRH_CNF15_1 | GPIO_CRH_CNF15_0 | GPIO_CRH_MODE15_1 | GPIO_CRH_MODE15_0);
//	//
//	GPIOA->CRL &=~(GPIO_CRL_CNF4_1 | GPIO_CRL_CNF4_0 | GPIO_CRL_MODE4_1 | GPIO_CRL_MODE4_0 |
//
//					GPIO_CRL_CNF7_1 | GPIO_CRL_CNF7_0 | GPIO_CRL_MODE7_1 | GPIO_CRL_MODE7_0);

	// ��������� ������ 3, ��� ���� � ��������� � �����
//		GPIOA->CRL |= GPIO_CRL_CNF7_1;
//		GPIOA->ODR &=~GPIO_ODR_ODR7;

	GPIO_InitTypeDef GPIO_InitStructure_button3;
			GPIO_InitStructure_button3.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_InitStructure_button3.GPIO_Pin = GPIO_Pin_7;  //
			GPIO_InitStructure_button3.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStructure_button3);

	 //
	// ��������� ������ 1,2,4, ��� ���� � ��������� � �����
//	GPIOC->CRH |= GPIO_CRH_CNF14_1;
//	GPIOC->ODR |= GPIO_ODR_ODR14; //������ 1

	GPIO_InitTypeDef GPIO_InitStructure_button1;
			GPIO_InitStructure_button1.GPIO_Speed = GPIO_Speed_2MHz;
			GPIO_InitStructure_button1.GPIO_Pin = GPIO_Pin_14;  //
			GPIO_InitStructure_button1.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure_button1);


//	GPIOA->CRL |= GPIO_CRL_CNF4_1;
//	GPIOA->ODR |= GPIO_ODR_ODR4; //������ 2
	//

	GPIO_InitTypeDef GPIO_InitStructure_button2;
				GPIO_InitStructure_button2.GPIO_Speed = GPIO_Speed_2MHz;
				GPIO_InitStructure_button2.GPIO_Pin = GPIO_Pin_4;  //
				GPIO_InitStructure_button2.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOA, &GPIO_InitStructure_button2);


//	GPIOC->CRH |= GPIO_CRH_CNF15_1;
//	GPIOC->ODR |= GPIO_ODR_ODR15; //������ 4

	GPIO_InitTypeDef GPIO_InitStructure_button4;
					GPIO_InitStructure_button4.GPIO_Speed = GPIO_Speed_2MHz;
					GPIO_InitStructure_button4.GPIO_Pin = GPIO_Pin_15;  //
					GPIO_InitStructure_button4.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(GPIOC, &GPIO_InitStructure_button4);
}


void timer_init(void)
//������������� ������� 3
{
//  �������� ������������
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
//  ��������� ���������� �� ������������ �������
	TIM3->DIER |= TIM_DIER_UIE; // Update  Interrupt Enable
//  ��������� ���������� �������
	NVIC_EnableIRQ(TIM3_IRQn);
//  ��������� ������ 3 �� �������� ������� � 1 kHz
	TIM3->PSC = 36000 - 1;
//  ������ - 1000 ������ => 1000/1000 = 1 Hz
	TIM3->ARR = 1000 - 1;
	TIM3->CR1 |= TIM_CR1_CEN; // Start count
}
