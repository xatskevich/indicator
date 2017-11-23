
#include "main.h"

void TIM14_IRQHandler(void){	//620мс

	TIM14->SR &= ~TIM_SR_UIF;	//сброс флага

	can_error = 0;
	if(can_ctrl == 1){	//если в течение 620мс не было CAN сообщений
		can_error = 1;		//выставляем ошибку
		can_error_mask ^= 1;	//маска моргания индикаторами
	}

	can_ctrl = 1;

	morg ^= 1;		//моргалка


}


void TIM17_IRQHandler(void){	//40 мс

	TIM17->SR &= ~TIM_SR_UIF;	//сброс флага

	IWDG_ReloadCounter();

	tmr ^= 1;

	if(tmr){
		TxMessage.IDE = CAN_Id_Standard;
		TxMessage.StdId=0x018;
		TxMessage.DLC=4;
		TxMessage.Data[0] = percent;
		TxMessage.Data[1] = 0xff;
		TxMessage.Data[2] = 0xff;
		TxMessage.Data[3] = 0xff;
		CAN_Transmit(CAN, &TxMessage);
		if(CAN_GetLastErrorCode(CAN)){		//ошибка отправки
			//GPIO_SetBits(GPIOA, GPIO_Pin_9);
		}

	}


	ADC_StartOfConversion(ADC1);
}

void delay_ms (uint16_t dl){
	TIM3->CNT = 0;
	TIM3->PSC = 48000;		//период 1мс
	TIM3->ARR = dl;		//
	TIM3->CR1 |= TIM_CR1_CEN;	//запуск таймера
	TIM3->SR &= ~TIM_SR_UIF;	//сброс флага
	while((TIM3->SR&TIM_SR_UIF)==0);
	TIM3->CR1 &= ~TIM_CR1_CEN;
}

void Init_Timer(){

	//таймер контроля CAN
	TIM14->PSC = 2399;
	TIM14->ARR = 6200;		//период 620мс
	TIM14->DIER |= TIM_DIER_UIE;
	TIM14->CR1 |= TIM_CR1_CEN;


	//таймер опроса кнопок
	TIM17->PSC = 239;
	TIM17->ARR = 4000;		//период 40 мс
	TIM17->DIER |= TIM_DIER_UIE;
	TIM17->CR1 |= TIM_CR1_CEN;

	//вектор
	NVIC_InitStructure.NVIC_IRQChannel = TIM14_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = TIM17_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}
