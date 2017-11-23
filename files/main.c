
#include "main.h"

int16_t level, min, max, koeff, koef_perc, analog_tot;
uint8_t lvl_out, can_ctrl, can_error, can_error_mask, morg, press, w_morg, tmr;
int8_t percent, analog_count;

CanTxMsg TxMessage;
GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
CAN_InitTypeDef CAN_InitStructure;
CAN_FilterInitTypeDef CAN_FilterInitStructure;
ADC_InitTypeDef ADC_InitStructure;


int main(void)
{
	uint32_t temp;

	max = 900;
	min = 100;

	FLASH->KEYR = 0x45670123;
	FLASH->KEYR = 0xCDEF89AB;
	//загрузка данных из flash
	temp=(*(__IO uint32_t*) min_address);
	if(temp!=0xffffffff) min=temp;
	temp=(*(__IO uint32_t*) max_address);
	if(temp!=0xffffffff) max=temp;

	//вычисление коэффициента
	if(max-min) koeff=koef_k/(max-min);
	else koeff=1024;//koef_k;

	if(max-min) koef_perc=koef_c/(max-min);
	else koef_perc=128;//koef_c;


	Init_IWDG();			//сторожевой

	Init_RCC();			//тактирование блоков
	//delay_ms(1);
	Init_GPIO();		//инициализация портов

	Init_ADC();
	Init_CAN();
	Init_Timer();


    while(1)
    {

    }
}

//стирание flash
void flash_erase(void){

	while (FLASH->SR & FLASH_SR_BSY);
	if (FLASH->SR & FLASH_SR_EOP) {
		FLASH->SR = FLASH_SR_EOP;
	}
    FLASH->CR |= FLASH_CR_PER;
    FLASH->AR = min_address;
    FLASH->CR |= FLASH_CR_STRT;
    while (FLASH->SR & FLASH_SR_BSY);//while (!(FLASH->SR & FLASH_SR_EOP));
    FLASH->SR = FLASH_SR_EOP;
    FLASH->CR &= ~FLASH_CR_PER;
}

//запись данных во flash
void flash_prog_all(void){

	while (FLASH->SR & FLASH_SR_BSY);
	if (FLASH->SR & FLASH_SR_EOP) {
		FLASH->SR = FLASH_SR_EOP;
	}
	FLASH->CR |= FLASH_CR_PG;
	*(__IO uint16_t*)min_address = min;
	while (!(FLASH->SR & FLASH_SR_EOP));
	FLASH->SR = FLASH_SR_EOP;
	*(__IO uint16_t*)max_address = max;
	while (!(FLASH->SR & FLASH_SR_EOP));
	FLASH->SR = FLASH_SR_EOP;


	FLASH->CR &= ~(FLASH_CR_PG);
}

