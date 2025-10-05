#include "S32K144.h" /* include peripheral declarations S32K144 */
#include "clock_and_mode.h"
#define REDLED_PIN 15 /* RED LED*/
#define GREENLED_PIN 16 /* GREEN LED*/
#define BLUELED_PIN 0 /* BLUE LED */
#define POTENTIOMETER_PIN 14
#define POTENTIOMETER_CHANNEL 12

 uint32_t AdcResult = 0;

 void ADC_init(void)
 {
  /* Disable clock to change PCS */
  IP_PCC->PCCn[PCC_ADC0_INDEX] &=~ PCC_PCCn_CGC_MASK;
  /* PCS=1: Select SOSCDIV2 */
  IP_PCC->PCCn[PCC_ADC0_INDEX] |= PCC_PCCn_PCS(1);
  /* Enable bus clock in ADC */
  IP_PCC->PCCn[PCC_ADC0_INDEX] |= PCC_PCCn_CGC_MASK;
  /* ADCH=1F: Module is disabled for conversions*/
  IP_ADC0->SC1[0] = ADC_SC1_ADCH_MASK  ;
  /* ADICLK=0: Input clk = ALTCLK1=SOSCDIV2 */
  /* ADIV=0: Prescaler = 1 */
  /* MODE=1: 12-bit conversion */
  IP_ADC0->CFG1 = 0x000000004;
  /* SMPLTS=12(default): sample time is 13 ADC clks */
  IP_ADC0->CFG2 = 0x00000000C;
  /* ADTRG=0: SW trigger */
  /* ACFE,ACFGT,ACREN=0: Compare func disabled */
  /* DMAEN=0: DMA disabled */
  /* REFSEL=0: Voltage reference pins= VREFH, VREEFL */
  IP_ADC0->SC2 = 0x00000000;
  /* CAL=0: Do not start calibration sequence */
  /* ADCO=0: One conversion performed */
  /* AVGE,AVGS=0: HW average function disabled */
  IP_ADC0->SC3 = 0x00000000;
 }

 void Pick_ADC_channel(uint16_t ADC_Channel)
 { /* For SW trigger mode, SC1[0] is used */
  /* Clear prior ADCH bits */
  IP_ADC0->SC1[0]&=~ADC_SC1_ADCH_MASK;
  /* Initiate Conversion*/
  IP_ADC0->SC1[0] = ADC_SC1_ADCH(ADC_Channel);
 }

 uint8_t ADC_complete(void)
 {
  return ((IP_ADC0->SC1[0] & ADC_SC1_COCO_MASK) >> ADC_SC1_COCO_SHIFT); /* Wait for completion */
 }

 uint32_t Read_ADC(void)
 {
	 uint16_t ADC_result = IP_ADC0->R[0]; /* For SW trigger mode, R[0] is used */
	   volatile uint32_t mv = (5000 * ADC_result)/4096;
	   return mv; /* Convert result to mv for 0-5V range */
 }

void PORT_init (void)
{
 /* Enable clock for PORTD */
 IP_PCC->PCCn[PCC_PORTD_INDEX ]|=PCC_PCCn_CGC_MASK;

 /* Enable clock for PORTC (Potentiometer input) */
 IP_PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK;

 /* Set PTC14 to ADC0_SE12 (analog function) */
 /* Clear MUX bits = 000 → analog */
 IP_PORTC->PCR[POTENTIOMETER_PIN] &= ~PORT_PCR_MUX_MASK;

 IP_PORTD->PCR[BLUELED_PIN] = PORT_PCR_MUX(1); /* Port D0: MUX = GPIO */
 IP_PORTD->PCR[REDLED_PIN] = PORT_PCR_MUX(1); /* Port D15: MUX = GPIO */
 IP_PORTD->PCR[GREENLED_PIN] = PORT_PCR_MUX(1); /* Port D16: MUX = GPIO */
 IP_PTD->PDDR |= 1<<BLUELED_PIN; /* Port D0: Data Direction= output */
 IP_PTD->PDDR |= 1<<REDLED_PIN; /* Port D15: Data Direction= output */
 IP_PTD->PDDR |= 1<<GREENLED_PIN; /* Port D16: Data Direction= output */
 /* turn off all led */
 IP_PTD -> PSOR |= 1<<BLUELED_PIN | 1<<GREENLED_PIN | 1<<REDLED_PIN ;
}

int main(void)
{
 SOSC_init_8MHz(); /* Initialize system oscillator for 8 MHz xtal */
 SPLL_init_160MHz(); /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
 NormalRUNmode_80MHz(); /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
 PORT_init(); /* Init port clocks and gpio outputs */
 ADC_init(); /* Init ADC resolution 12 bit*/

 while(1) {
  /* Convert Channel AD12 to pot on EVB */
  Pick_ADC_channel(POTENTIOMETER_CHANNEL);
  /* Wait for conversion complete flag */
  while(ADC_complete()==0);
  /* Get channel's conversion results in mv */
  AdcResult = Read_ADC();
  /* If result > 3.75V */
  if (AdcResult > 3750) {
  /* turn off blue, green LEDs */
  IP_PTD->PSOR |= 1<<BLUELED_PIN | 1<<GREENLED_PIN;
  /* turn on red LED */
  IP_PTD->PCOR |= 1<<REDLED_PIN;
  }
  /* 2.5 < result < 3.75V */
  else if ((AdcResult > 2500) && (AdcResult <= 3750)){
  /* turn off blue, red LEDs */
  IP_PTD->PSOR |= 1<<BLUELED_PIN | 1<<REDLED_PIN;
  /* turn on green LED */
  IP_PTD->PCOR |= 1<<GREENLED_PIN;
  }
  /* 1.25 < result < 2.5 V */
  else if ((AdcResult > 1250) && (AdcResult <= 2500)) {
  /* turn off red, green LEDs */
  IP_PTD->PSOR |= 1<<REDLED_PIN | 1<<GREENLED_PIN;
  /* turn on blue LED */
  IP_PTD->PCOR |= 1<<BLUELED_PIN;
  }
  else {
  /* Turn off all LEDs if result <= 1250 */
  IP_PTD->PSOR |= 1<<REDLED_PIN | 1<< GREENLED_PIN | 1<<BLUELED_PIN; /* Turn off all LEDs */
  }

  }
}

