//Include Libraries
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define DEBUG_MEGA

//create an RF24 object
RF24 radio(9, 8);  // CE, CSN

//address through which two modules communicate.
const byte address[6] = "00001";

uint8_t SerialTXBuffer[256];
uint8_t SerialRXBuffer[256];
uint8_t frameIndexCounter = 0;
uint8_t flag_FrameIsFull = 0;
//uint8_t ignore =0;
//uint8_t var = 1;

void readFrameFromSerial() {
//	Serial.print("\nEnter readFrameFromSerial\n");
	for (uint8_t i = 0; i < 32; i++) {
		Serial1.readBytes(SerialTXBuffer + (8 * i), 8);
//		if(SerialTXBuffer[0]!=0x7E){
//			ignore = 1;
//			Serial.print("break");
//			break;
//		}
//		else{
//			ignore = 0;
//		}
	}
//	for (int i = 0; i < 256; i++) {
//		Serial1.readBytes(&SerialTXBuffer[i], 1);
//		Serial.print(SerialTXBuffer[i],HEX);
//	}
//	Serial.print("\nExit readFrameFromSerial\n");
	// for (uint8_t i = 0; i < 256; i++) {
	//   Serial.print(SerialTXBuffer[i]);
	// }
	// Serial.flush();
	//delay(100);
}

void writeToNrf() {
	radio.stopListening(); //switch to TX mode
	for (int i = 0; i < 256; ++i) {
		radio.write(&SerialTXBuffer[i], sizeof(uint8_t));
		//   delayMicroseconds(10);
	}
	radio.startListening(); // swtich back to RX mode
#ifdef DEBUG_MEGA
	Serial.print("Frame Sent to NRF");
#endif
}

void readFrameFromNrf() {
//	Serial.print("\nEnter readFrameFromNrf\n");
#if 1
	//Read the data if available in buffer
	unsigned char text[1] = { 0 };
	for (int j = 0; j < 256; j++) {
		while (!(radio.available()))
			;
		radio.read(&text, sizeof(text));
		SerialRXBuffer[j] = text[0];
	}
#else
	//Read the data if available in buffer
		unsigned char text[256] = { 0x7E,0x4F,0x4E,0x34,0x55,0x4C,0x47,0x60,0x4F,0x55,0x46,0x54,0x49,0x31,0x61,0x21,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xEA,0x5F,0x7E };
		for (int j = 0; j < 256; j++) {
			//radio.read(&text, sizeof(text));
			SerialRXBuffer[j] = text[j];
		}
#endif
}

void writeToSerial() {
//	delay(100);
	for (int i = 0; i < 256; ++i) {
		Serial1.write(SerialRXBuffer[i]);
//	delay(10);
		Serial1.flush();
	}
}

/*--------Debug--------*/
#ifdef DEBUG_MEGA
void printFrameToSerial() {
	Serial.print("\nEnter printFrameToSerial\n");

	for (int i = 0; i < 256; i++) {
		Serial.print(SerialTXBuffer[i], HEX);
	}
	Serial.flush();
	Serial.print("\nExit printFrameToSerial\n");
}
#endif
/*--------Debug--------*/

void setup() {
	// put your setup code here, to run once:
	Serial.begin(9600);
#ifdef DEBUG_MEGA
	Serial1.begin(9600); // extra for Arduino MEGA
#endif
	radio.begin();
	radio.openWritingPipe(address);
	radio.openReadingPipe(0, address);
	radio.startListening(); //default to RX mode
	radio.setPayloadSize(1);
}

void loop() {
	// put your main code here, to run repeatedly:

	//read from serial (LabVIEW) and then write to nRF
	if (Serial1.available()) {
			Serial.print("\nSerial Data is Available\n");
			readFrameFromSerial();
#ifdef DEBUG_MEGA
			printFrameToSerial();
#endif
			writeToNrf();
			//	Serial.print("2Rad");
//			delay(100);
		}
	//read from nRF then write to Serial (LabVIEW)
	if (radio.available()) {
		//Serial.print("1Rad");
		readFrameFromNrf();
#ifdef DEBUG_MEGA
		Serial.print("nrf");
		//delay(100);
		//for (int i= 0; i < 256; ++i) {
		//	Serial.print(SerialRXBuffer[i],HEX);
		//}
#endif
		//Serial.print("2Rad");
		writeToSerial();
	}
}