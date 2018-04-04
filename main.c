/*  
  * File:  main.c  
  * Author: Embedded Laboratory
  *  
  * Created on November 10, 2016, 10:24 PM  
  */  
 #include "config.h"  
 #include "lcd_16x2.h"  
 #include "adc.h"  
 uint32_t led_timestamp = 0;  
 static uint8_t led_state = FALSE;  
 uint32_t adcUpdate_timestamp = 0;  
 char lcd_msg[LCD_BUFFER_LEN] = { 0 };  
 #define ADC_BUFFER_LENGTH  40u  
 uint16_t adc_data[ADC_BUFFER_LENGTH] = {0};  
 uint8_t adc_data_index = 0;  
 void main()  
 {  
  // Select 16MHz Internal Oscillator  
  OSCCONbits.IRCF = 0x07;       // From 1MHz to 16MHz  
  InitTimer0 ();  
  LCD_Init ();  
  // Configure ADC  
  ANSELAbits.ANSA0 = 1;   // Disable the Digital Input Buffer on AN0  
  TRISAbits.RA0 = 1;    // AN0 as Input Pin
  ANSELAbits.ANSA1 = 1;   // Disable the Digital Input Buffer on AN0  
  TRISAbits.RA1 = 1
  ADCON1bits.PVCFG = 0x00; // +Vref = AVDD  
  ADCON1bits.NVCFG = 0x00; // -Vref = AVSS  
  ADCON2bits.ADFM = 0x01;  // Right Justified  
  ADCON2bits.ACQT = 0x01;  // 2TAD  
  ADCON2bits.ADCS = 0x05;  // FOSC/16  
  ADCON0bits.CHS = 0x00;  // AN0 Selected  
  ADCON0bits.ADON = 1;   // Enable ADC  
  sprintf (lcd_msg, "  Embedded");  
  LCD_Print_Line (0, lcd_msg);  
  sprintf ( lcd_msg, "AC RMS = %dV", 0u);
  LCD_Print_Line (1, lcd_msg);  
  LCD_Update ();  
  while(1)  
  {  
   // LCD Update Task  
   if( millis() - led_timestamp > 1000u )  
   {  
    led_timestamp = millis();  
    if( led_state )  
    {  
     led_state = FALSE;  
     sprintf (lcd_msg, "  Laboratory");  
    }  
    else  
    {  
     led_state = TRUE;  
     sprintf (lcd_msg, "  Embedded");  
    }  
    LCD_Print_Line (0, lcd_msg);  
    LCD_Update ();  
   }  
   // ADC Update Task  
   if( millis() - adcUpdate_timestamp >= 1u )  
   {  
    uint32_t ac_value = 0;  
    adcUpdate_timestamp = millis ();  
    ADCON0bits.CHS = 0x00;  // AN0 Selected  
    ADCON0bits.GODONE = 1; // Start Conversion  
    ADCON1bits.CHS = 0x00;  // AN0 Selected  
    ADCON1bits.GODONE = 1; // Start Conversion  
    while(ADCON0bits.GO == 1 & ADCON1bits.GO == 1);  
    uint8_t adc_lo = ADRESL;  
    uint8_t adc_hi = ADRESH;  
    adc_data[adc_data_index] = (uint16_t)(adc_hi << 8) | (uint16_t)adc_lo;  
    adc_data_index++;  
    if( adc_data_index >= ADC_BUFFER_LENGTH )  
    {  
     adc_data_index = 0u;  
     // Time to Process and Display Data  
     uint32_t adc_filter_val = 0;  
     uint16_t adc_buf_local = 0;  
     for( uint8_t i=0; i<ADC_BUFFER_LENGTH; i++ )  
     {  
      adc_buf_local = adc_data[i];  
      adc_filter_val += ((uint32_t)(adc_buf_local*(uint32_t)adc_buf_local ));  
     }  
     // 625*625 = 390625  
     // 128*128 = 16384  
     // 625^2 / 128^2 = 23.8414  
     adc_filter_val = (uint32_t)((float)adc_filter_val * 23.84);  
     adc_filter_val /= ADC_BUFFER_LENGTH;   // mean squared sum  
     adc_filter_val *= 2;  
     ac_value = (uint16_t)sqrt(adc_filter_val);// root mean square  
     ac_value *= 68u;  
     ac_value /= 1000u;  
     sprintf ( lcd_msg, "AC RMS = %luV", ac_value);  
     LCD_Print_Line (1, lcd_msg);  
     LCD_Update();  
    }  
   }  
  }  
 }
