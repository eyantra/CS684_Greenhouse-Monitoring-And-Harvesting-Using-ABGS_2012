
 #include <stdio.h>
 #include <prjParams.h>
 #include <prjCommon.h>
 #include <hal/zigbee.h> 
 #include <assert.h>
 #include <util/setbaud.h>

//Function To Initialize ZigBee
STATUS initZigbee() {
	INT_LOCK();	/* Lock (disable) global interrupts */

	/* Initailize UART1 for serial communiaction */
	// Parameters:
	// desired baud rate:9600
	// actual baud rate:9600 (error 0.0%)
	// char size: 8 bit
	// parity: Disabled

    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */ 
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */


	INT_UNLOCK();  /* Enables the global interrupts */
	return STATUS_OK;
}

STATUS zigbeeSend(char *data, UINT size) {
	UINT idx;
	
	ASSERT(data != NULL);
	if(data == NULL) {
		return !STATUS_OK;
	}
	
	for(idx = 0; idx < size; idx ++) {
		UDR0 = data[idx];
		_delay_ms(2);
	}
	return STATUS_OK;
}

int zigbeeSendByte(char u8Data, FILE *stream)
{
   if(u8Data == '\n')
   {
      zigbeeSendByte('\r', 0);
   }
	//Wait while previous byte is completed
	while(!(UCSR0A&(1<<UDRE0))){};
	// Transmit data
	UDR0 = u8Data;
	return 0;
}

int zigbeeReceiveByte(FILE *stream)
{
	BYTE u8Data;
	
	// Wait for byte to be received
	while(!(UCSR0A&(1<<RXC0))){};
	u8Data=UDR0;

	//echo input data
	//zigbeeSendByte(u8Data,stream);

	// Return received data
	return u8Data;
}
