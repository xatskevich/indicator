
#include "main.h"


void ADC1_COMP_IRQHandler(void)
{
	int16_t analog;
	int32_t temp;

	if(ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET){
		ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
	}

	//analog=ADC1->DR;
	analog_tot += ADC1 -> DR;
	analog_count++;
	if(analog_count == 8){

		analog_count = 0;
		analog = analog_tot / 8;
		analog_tot = 0;

		level=(level*4-level+analog)/4;	//новое значение
	}
	temp=(level-min)*koeff/2048;
	if(temp < 0) temp = 0;
	if(temp  >8) temp = 8;
	percent=(level-min)*koef_perc/256;
	if(percent < 0) percent = 0;
	if(percent > 100) percent = 100;

	if((GPIOB -> IDR & 8) == 0){			//нажатие кнопки
		press++;
	}
if(GPIOB -> IDR & 8){								//кнопка отпущена
		if(press > 60){		//верхний уровень
			max = level;

			if(max-min) koeff=koef_k/(max-min);
			else koeff=koef_k;

			if(max-min) koef_perc=koef_c/(max-min);
			else koef_perc=koef_c;

			//стирание flash
			flash_erase();
			//запись
			flash_prog_all();
			w_morg=0x80;

		} else if(press > 12){		//нижний уровень
			min = level;

			if(max-min) koeff=koef_k/(max-min);
			else koeff=koef_k;

			if(max-min) koef_perc=koef_c/(max-min);
			else koef_perc=koef_c;

			//стирание flash
			flash_erase();
			//запись
			flash_prog_all();
			w_morg=0x80;

		}

		press = 0;
	}

	if(w_morg){
		 if((w_morg & 0x80) == 0){	//если спад шкалы
		 	if(w_morg != 0) w_morg--;
		 } else {
		 	w_morg++;
		 	if((w_morg & 0xf)==8) w_morg &= ~0x80; //если максимум шкалы, то на спад
		 }
		 temp = w_morg & 0xf;
	}
	if(temp != 0){	//уровень есть
		GPIO_ResetBits(GPIOA, GPIO_Pin_3);
		lvl_out = 0;
		while(temp != 0){
			lvl_out = lvl_out << 1;
			lvl_out++;
			temp--;
		}
		if(lvl_out & 0x1) GPIO_SetBits(GPIOA, GPIO_Pin_4); else GPIO_ResetBits(GPIOA, GPIO_Pin_4);
		if(lvl_out & 0x2) GPIO_SetBits(GPIOA, GPIO_Pin_5); else GPIO_ResetBits(GPIOA, GPIO_Pin_5);
		if(lvl_out & 0x4) GPIO_SetBits(GPIOA, GPIO_Pin_6); else GPIO_ResetBits(GPIOA, GPIO_Pin_6);
		if(lvl_out & 0x8) GPIO_SetBits(GPIOA, GPIO_Pin_7); else GPIO_ResetBits(GPIOA, GPIO_Pin_7);
		if(lvl_out & 0x10) GPIO_SetBits(GPIOB, GPIO_Pin_4); else GPIO_ResetBits(GPIOB, GPIO_Pin_4);
		if(lvl_out & 0x20) GPIO_SetBits(GPIOB, GPIO_Pin_5); else GPIO_ResetBits(GPIOB, GPIO_Pin_5);
		if(lvl_out & 0x40) GPIO_SetBits(GPIOB, GPIO_Pin_6); else GPIO_ResetBits(GPIOB, GPIO_Pin_6);
		if(lvl_out & 0x80) GPIO_SetBits(GPIOB, GPIO_Pin_7); else GPIO_ResetBits(GPIOB, GPIO_Pin_7);
	} else {
		GPIOA -> BRR = 0xf0;
		GPIOB -> BRR = 0xf0;
		if(morg) GPIO_SetBits(GPIOA, GPIO_Pin_3);
		else GPIO_ResetBits(GPIOA, GPIO_Pin_3);
	}
	//}
}


void Init_ADC(){
	//параметры АЦП по умолчанию
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_10b;
	ADC_Init(ADC1, &ADC_InitStructure);

	//каналы воды пены оборотов
	//ADC_ChannelConfig(ADC1, ADC_Channel_6, ADC_SampleTime_239_5Cycles);	//для напряжения
	ADC_ChannelConfig(ADC1, ADC_Channel_2, ADC_SampleTime_239_5Cycles);	//для тока
	ADC_GetCalibrationFactor(ADC1);
	//прерывания
	ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = ADC1_COMP_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00; //!!!!!!!!!!!!!!!
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	ADC_Cmd(ADC1, ENABLE);
	/* Wait the ADRDY flag */
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY));

	ADC_StartOfConversion(ADC1);
}
