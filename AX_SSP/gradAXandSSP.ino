#include "Arduino.h"
#include "ssp.h"
#include <stdlib.h>
#include <stdio.h>

#include "ax25.h"
//ay 7aga
/* keep this line when in Arduino is in RX mode, otherwise, comment it out. */
//#define RX_M
/* keep this line when Debugging, otherwise, comment it out. */
#define DEBUG

/* Include nRF Libraries */
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//create an RF24 object
RF24 radio(9, 8);  // CE, CSN

//address through which two modules communicate.
const byte address[6] = "00001";

uint8 SerialTXBuffer[AX25_FRAME_MAX_SIZE];
uint8 SerialRXBuffer[AX25_FRAME_MAX_SIZE];
uint8 info[SSP_FRAME_MAX_SIZE]; /* this is in Control to Framing part */
uint8 SSP_to_Control_Buffer[SIZE_SSP_to_Control_Buffer];
uint8 addr[ADDR_LEN] = { 'O', 'N', '4', 'U', 'L', 'G', 0x60, 'O', 'U', 'F', 'T',
		'I', '1', 0x61 };
uint8 g_info_reciver[SSP_FRAME_MAX_SIZE];
extern uint8 Control_To_SSP[236];
uint8 flag_SSP_to_Control = EMPTY;
uint8 flag_Control_to_Framing = EMPTY;
uint8 flag_Control_to_SSP = EMPTY;
uint8 flag_Deframing_to_Control = EMPTY;
uint8 flag_SerialTXBuffer = EMPTY;
uint8 flag_SerialRXBuffer = EMPTY;
uint8 flag_next_frame = FULL;
extern uint8 flag_controltossp = EMPTY;
uint8 g_infoSize = 236;  //temp set as 236

uint8 txframe[dt];
uint8 data[information];
uint8 data2[information];
uint8 rxframe[dt];
uint8 Rx_data[information];
uint8 layerdata[information];
uint8 ax_rx_data[information];
//uint8 Rx_App_data[information];
uint8 ax_ssp_frame[dt];
uint8 type2;

uint8 checkcontrol = EMPTY;
uint16 data_length;
uint8 dataflag = EMPTY;
uint8 rxflag = EMPTY;
uint16 ax_rx_length = 0;
uint8 typee;
void receive_frame_here() {
	if (rxflag == EMPTY) {
		if (Serial1.available() > 0) {
			Serial1.readBytes(rxframe, 236);
			serial_flush_buffer();
			rxflag = FULL;
			Serial1.println("\n Received frame\n");
			Serial1.flush();
		}
	}
}

void serial_flush_buffer() {
	while (Serial1.read() >= 0)
		; // do nothing
}
#if 0
void printSerialTXBufferToSerial() {
	if (flag_SerialTXBuffer == FULL) {
		for (int i = 0; i < AX25_FRAME_MAX_SIZE; ++i) {
			Serial.print(SerialTXBuffer[i], HEX);

			//  Serial.print(SerialTXBuffer[i], HEX);
			Serial.flush();
		}
		flag_SerialTXBuffer = EMPTY;
		//  Serial.print("\n\n");
	}
}

void readFrameFromSerial() {
	uint8 flag_flagAndDestMatchSerialRXBuffer = SET; /* init value as set */
	uint8 flagAndDestAddress[8] = { 0x7e, 'O', 'N', '4', 'U', 'L', 'G', 0x60 };

	if (Serial.available() && flag_SerialRXBuffer == EMPTY) {
		g_infoSize = SSP_FRAME_MAX_SIZE;

#ifdef DEBUG
		Serial.print("\n waiting for data \n");
#endif

		Serial.readBytes(SerialRXBuffer, 8);
		for (uint8 i = 0; i < 8; i++) {
			if (SerialRXBuffer[i] != flagAndDestAddress[i]) {
				flag_flagAndDestMatchSerialRXBuffer = CLEAR;
			}
		}

//#ifdef DEBUG
//    Serial1.println(SerialRXBuffer[0]);
//#endif
		if (flag_flagAndDestMatchSerialRXBuffer == SET) {
			for (uint8 i = 1; i < 32; i++) {
				Serial.readBytes(SerialRXBuffer + (8 * i), 8);
			}

			//Serial.readBytes(SerialRXBuffer, AX25_FRAME_MAX_SIZE);

			//Serial.flush();
			delay(100);
			flag_SerialRXBuffer = FULL;

#ifdef DEBUG
			Serial.print("\n Received Frame\n");
#endif
			Serial.flush();

			/* prints the frame received from serial on serial monitor */

			for (int i = 0; i < 30; ++i) {

#ifdef DEBUG
				//  Serial.print(SerialRXBuffer[i], HEX);
				Serial.flush();
#endif

			}

#ifdef DEBUG
			Serial.print("\n\n");
#endif

			for (int i = 230; i < 256; ++i) {

#ifdef DEBUG
				// Serial.print(SerialRXBuffer[i], HEX);
				Serial.flush();
#endif
			}

#ifdef DEBUG
			Serial.print("\n\n");
#endif

		}
	}
}
#endif

/*
 * Description: prints the data stored in SerialTXBuffer to Serial or nRF
 * flags: flag_SerialTXBuffer flag.
 */
void printSerialTXBufferToSerial() {
	if (flag_SerialTXBuffer == FULL) {

		radio.stopListening();
		for (int i = 0; i < AX25_FRAME_MAX_SIZE; ++i) {

			//radio.write(&SerialTXBuffer[i], sizeof(uint8)); /* writes Serial RX Buffer to nRF 1 byte at a time */

			// Note: the serial.write and print lines below are deprecated after using nRF Module

			Serial.write(SerialTXBuffer[i]); 		// used to write serially to LabVIEW
			//Serial.print(SerialTXBuffer[i], HEX); // to display array as string in HEX format (mostly used for debugging)
			//Serial.flush();
		}
		flag_SerialTXBuffer = EMPTY;
		radio.startListening();
	}
}

void readFrameFromSerial() {
	uint8 flag_flagAndDestMatchSerialRXBuffer = SET; /* init value as set (do not change init value, changing it will cause unwanted behavior)*/
	uint8 flagAndDestAddress[8] = { 0x7e, 'O', 'N', '4', 'U', 'L', 'G', 0x60 };

#ifdef SerialWire

	if (Serial.available() && flag_SerialRXBuffer == EMPTY) {
		g_infoSize = SSP_FRAME_MAX_SIZE;

#ifdef DEBUG
		Serial1.print("\n waiting for data \n");
#endif

		Serial.readBytes(SerialRXBuffer, 8);
		for (uint8 i = 0; i < 8; i++) {
			if (SerialRXBuffer[i] != flagAndDestAddress[i]) {
				flag_flagAndDestMatchSerialRXBuffer = CLEAR;
			}
		}

//#ifdef DEBUG
//		Serial1.println(SerialRXBuffer[0]);
//#endif
		if (flag_flagAndDestMatchSerialRXBuffer == SET) {
			for (uint8 i = 1; i < 32; i++) {
				Serial.readBytes(SerialRXBuffer + (8 * i), 8);
			}

			//Serial.readBytes(SerialRXBuffer, AX25_FRAME_MAX_SIZE);

			//Serial.flush();
			delay(100);
			flag_SerialRXBuffer = FULL;

#ifdef DEBUG
			Serial1.print("\n Received Frame\n");
#endif
			Serial.flush();

			/* prints the frame received from serial on serial monitor */

			for (int i = 0; i < 30; ++i) {

#ifdef DEBUG
				Serial1.print(SerialRXBuffer[i], HEX);
				Serial.flush();
#endif

			}

#ifdef DEBUG
			Serial1.print("\n\n");
#endif

			for (int i = 230; i < 256; ++i) {

#ifdef DEBUG
				Serial1.print(SerialRXBuffer[i], HEX);
				Serial.flush();
#endif
			}

#ifdef DEBUG
			Serial1.print("\n\n");
#endif

		}
	}
#endif

#ifndef SerialWire
	/* code to read from nrf  */

	if (radio.available() && flag_SerialRXBuffer == EMPTY) {

		Serial.print("\nreceived Frame from nrf\n");

		g_infoSize = SSP_FRAME_MAX_SIZE;

		//Read the data if available in buffer
		unsigned char text[1] = { 0 };
		for (int j = 0; j < 8; j++) {
			while (!(radio.available()))
				;
			radio.read(&text, sizeof(text));
			SerialRXBuffer[j] = text[0];
		}

		for (uint8 i = 0; i < 8; i++) {
			if (SerialRXBuffer[i] != flagAndDestAddress[i]) {
				flag_flagAndDestMatchSerialRXBuffer = CLEAR;
			}
		}

		if (flag_flagAndDestMatchSerialRXBuffer == SET) {

			//Read the data if available in buffer
			unsigned char text_2[1] = { 0 };
			for (int j = 8; j < 256; j++) {
				while (!(radio.available()))
					;
				radio.read(&text_2, sizeof(text_2));
				SerialRXBuffer[j] = text_2[0];
			}

//			delay(100);
			flag_SerialRXBuffer = FULL;
			Serial.flush();
		}
#ifdef DEBUG
			for (int i = 0; i < 256; i++) {
				Serial.print(SerialRXBuffer[i], HEX);
			}
#endif
	}
#endif
}

void setup() {
	Serial.begin(9600);
	Serial1.begin(9600);
	radio.begin();
	//set the address
	radio.openWritingPipe(address);
	radio.openReadingPipe(0, address);
	//Set module as receiver
	radio.startListening();

	 radio.setPayloadSize(1);

//  if (checkcontrol == EMPTY) {
//    getdata(data, &data_length, &dataflag);
//    checkcontrol = FULL;
//  }
//#ifndef RX_M
	// if (flag_SSP_to_Control == EMPTY) {
	//   fillBuffer(SSP_to_Control_Buffer, SIZE_SSP_to_Control_Buffer);
	//   flag_SSP_to_Control = FULL;
//  }
//#endif
}

void loop() {

	static uint8 txflag = EMPTY;
	uint8 i;
	static uint8 crcflag = EMPTY;
	uint8 desti;
	uint8 srce;
	uint8 ax_src;
	uint8 ax_type;
	uint8 desti2;
	static uint16 tx_size = 0;
	uint8 adddest;
	uint8 addsrc;
	static uint8 type = 0;
	static uint16 Rx_length = 0;
	static uint8 layerflag = EMPTY;
	static uint8 deframeflag = EMPTY;
	static uint8 framingflag = EMPTY;
	uint8 dest_to_framing;
	uint8 src_to_framing;
	uint8 type_to_framing;
	//uint8 ax_dest;
	//uint8 ax_src;
	//static uint8 ax_type = 0;
	//static uint8 deframe_ax_flag = EMPTY;
	static uint16 tx_ax_length = 0;
	static uint8 ax_ssp_flag = EMPTY;
	//static uint8 deframetoframeflag = EMPTY;

	uint8 control;
	uint16 frameSize = 0;

//	delay(100);

//delay(1000);
	//Serial1.print(flag_controltossp, HEX);

//if(checkcontrol == EMPTY && deframe_ax_flag==EMPTY&&flag_Deframing_to_Control == FULL)

	if (checkcontrol == EMPTY && flag_controltossp == FULL) {
		//dataflag = EMPTY;

		ssp_ax_deframing(Control_To_SSP, ax_rx_data, &ax_rx_length, &ax_type,
				&ax_src);

		//Serial1.print("\n size w dkhal hena \n");
		//Serial1.print(ax_rx_length, HEX);
		//Serial1.print("\n \n");
		//for (i = 0; i < ax_rx_length; i++) {
		//	data[i] = ax_rx_data[i];
		//Serial1.print(g_info_reciver[i], HEX);
		//	}
		//flag_controltossp=EMPTY;
		//	deframe_ax_flag=EMPTY;
		//	data_length = ax_rx_length;
		//	dataflag = FULL;
		getdata(data, &data_length, &dataflag, ax_type, ax_src, &typee, &desti);

		checkcontrol = FULL;
	}

	if ((checkcontrol == FULL && txflag == EMPTY)
			|| (checkcontrol == EMPTY && layerflag == EMPTY)) {
		control_layer(data, data_length, &desti, &srce, &typee, &type2, data2,
				&desti2, &type, Rx_data, &adddest, &Rx_length, &dataflag,
				&deframeflag, &txflag, layerdata, crcflag, &tx_size, &addsrc,
				&layerflag, &checkcontrol, &dest_to_framing, &src_to_framing,
				&type_to_framing);

		//	layerflag = EMPTY;

	}

	if (txflag == FULL) {
		ssp_build_frame(txframe, data2, desti2, srce, type2, tx_size, &txflag);
	}

	receive_frame_here();

	if (rxflag == FULL && deframeflag == EMPTY) {
		//Serial1.print("DEFRAME");
		ssp_deframing(rxframe, &adddest, &addsrc, &type, Rx_data, &Rx_length,
				&rxflag, &crcflag, &deframeflag);
//Serial1.print("\n hana\n");
	}

	/* Sends next frame */
	//Serial1.print(layerflag,HEX);
	//delay(2000);
//if(deframetoframeflag==FULL){
//	 ax_ssp_framing(ax_ssp_frame,layerdata, desti, srce,typee,Rx_length, &tx_ax_length,&ax_ssp_flag);
//	deframetoframeflag = EMPTY;
//}
//delay(1000);
//	Serial1.print(flag_next_frame,HEX);
	if (flag_next_frame == FULL && layerflag == FULL
			&& flag_SSP_to_Control == EMPTY) {
		Serial1.print("DKHAL EL FUNCTION ASASN ");
		ax_ssp_framing(ax_ssp_frame, layerdata, &dest_to_framing,
				&src_to_framing, &type_to_framing, Rx_length, &tx_ax_length);
		fillBuffer(&tx_ax_length, &layerflag, dest_to_framing, type_to_framing,
				&dataflag, data, &data_length, &checkcontrol, &desti, &typee,
				&src_to_framing);
//Serial1.print("\n el data el mafrod tro7\n\n");
//Serial1.print(tx_ax_length);
//Serial1.print("\n\n\n");
//Serial1.print("\n check gded\n\n");

// for (i = 0;i < Rx_length; i++) {

		//				Serial1.print(layerdata[i], HEX);

		//			}

		//Serial1.print("\n\n\n");

		/*for (i = 0; i < tx_ax_length; i++) {
		 SSP_to_Control_Buffer[i] = ax_ssp_frame[i];

		 Serial1.print(SSP_to_Control_Buffer[i], HEX);

		 }
		 //SIZE_SSP_to_Control_Buffer=Rx_length;

		 g_infoSize = tx_ax_length;
		 ax_ssp_flag=EMPTY;
		 layerflag=EMPTY;
		 flag_SSP_to_Control = FULL;*/

		flag_next_frame = EMPTY;
	}

	/* Calls the manager function */
	if ((flag_SSP_to_Control == FULL && flag_Control_to_Framing == EMPTY)
			|| (flag_Control_to_SSP == EMPTY
					&& flag_Deframing_to_Control == FULL)) {

#ifdef DEBUG
	//	Serial.print("\nManagement\n");
#endif
		AX25_Manager(&control);
	}

	/* Builds Frame after receiving fields */
	if (flag_Control_to_Framing == FULL && flag_SerialTXBuffer == EMPTY) {

#ifdef DEBUG
	//	Serial.print("\nBuild Frame\n");
#endif
		AX25_buildFrame(SerialTXBuffer, info, &frameSize, addr, control,
				g_infoSize);
	}

	/* Prints Serial TX buffer */
	printSerialTXBufferToSerial();

	/* Gets frame from serial */
	readFrameFromSerial();
	//serialFlush();

	/* Calls the de-framing function */
	if (flag_Deframing_to_Control == EMPTY && flag_SerialRXBuffer == FULL) {

#ifdef DEBUG
	//	Serial.print("\nDeframe\n");
#endif
		AX25_deFrame(SerialRXBuffer, frameSize, g_infoSize);
	}

}
