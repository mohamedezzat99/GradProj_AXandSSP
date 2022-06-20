/*
 ============================================================================
 Name        : AX25.c
 Author      : Mohamed Ezzat
 Version     :
 Copyright   : Your copyright notice
 Description : Form AX25 frame structure
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "ax25.h"
#include "AX25_CRC.h"
#include "Arduino.h"

#define DEBUG
uint8 g_pollFinal = 0;
uint8 g_Received_NR = 0;

uint8 g_received_address[ADDR_LEN], Control_To_SSP[236],
		g_control_recived[CNTRL_LEN], g_padding_recived[PADDING_LEN];
extern uint8 g_info_reciver[SSP_FRAME_MAX_SIZE];
extern uint8 addr[ADDR_LEN], ax_ssp_frame[dt];

uint8 flag_RX_crc; /* used in the Manager function to check whether CRC is correct or not */
uint8 flag_info = EMPTY;

uint8 flag_Status = ACCEPT; /* used in Manager function to determine if the I-frame we sent was accepted or rejected by the Receiver  */
uint8 rejCounter = 0;

extern uint8 flag_SSP_to_Control;
extern uint8 flag_Control_to_Framing;
extern uint8 flag_Control_to_SSP;
extern uint8 flag_Deframing_to_Control;
extern uint8 flag_controltossp;
extern uint8 info[SSP_FRAME_MAX_SIZE];
extern uint8 SSP_to_Control_Buffer[SIZE_SSP_to_Control_Buffer];

extern uint8 g_infoSize;
extern uint8 flag_SerialTXBuffer;
extern uint8 flag_SerialRXBuffer;
extern uint8 flag_next_frame;
/* -------------------- TX Functions --------------------*/

/*
 * Description: computes control byte given the following parameters
 * parameters:
 * frameType: give frame type (I, S, U)
 * secondaryType: RR, RNR, REJ, SREJ, SABME, DISC, DM, UA , UI, TEST
 *
 */
uint8 AX25_getControl(frameType frameType, frameSecondaryType secondaryType,
		uint8 NS, uint8 NR, uint8 pollFinal) {
	uint8 control = 0;
	switch (frameType) {
	case I:
		control = (control & 0x1F) | ((NR << 5) & 0xE0); /* insert N(R) into control field */
		control = (control & 0xF1) | ((NS << 1) & 0x0E); /* insert N(S) into control field */
		control = (control & 0xEF) | ((pollFinal << 4) & 0x10); /* insert P into control field */
		control &= ~(1 << 0); /* insert 0 in rightmost bit */
		break;
	case S:
		control = (control & 0x1F) | ((NR << 5) & 0xE0); /* insert N(R) into control field */
		control = (control & 0xEF) | ((pollFinal << 4) & 0x10); /* insert P/F into control field */
		control = (control & 0xFC) | 0x01; /* insert 01 in the two rightmost bits */
		control = (control & 0xF3) | ((secondaryType << 2) & 0x0C); /* insert S bits into their place */
		break;
	case U:
		control = (control & 0xEF) | ((pollFinal << 4) & 0x10); /* insert P/F into control field */
		control = (control & 0xFC) | 0x03; /* insert 11 in the two rightmost bits */
		control = (control & 0x13) | (secondaryType & 0xEC); /* insert M bits into their proper location */
		break;
	}
	return control;
}

/*
 * TESTING function
 * Description: function fills in info array
 * parameters:
 *  *info: pointer to the global info array
 */
void fillBuffer(uint16 *tx_ax_length, uint8 *layerflag, uint8 dest_to_framing,
		uint8 type_to_framing, uint8 *dataflag, uint8 *data,
		uint16 *data_length, uint8 *checkcontrol, uint8 *Tx_App_desti,
		uint8 *Tx_App_type, uint8 *src_to_framing) {
#ifdef SSP_DEBUG
	Serial1.print("dkhl hena");
#endif
	uint8 i;
	uint8 source1 = 0x05;
	uint8 source2 = 0x03;
	uint8 arr[] = { 0x54, 0x26, 0xc0, 0x34, 0x7 };

#if 0
	static uint8 Data = 0;

	//currently fill buffers this way
	if (flag_Status == REJECT) {
		// stay at same value since we want to send the same data in the frame
	} else {
		//increment to send next frame with different data as normal
		for (int i = 0; i < size; i++) {
			buffer[i] = Data++;
		}
	}
#endif
	if (dest_to_framing == source2) {
		for (i = 0; i < *tx_ax_length; i++) {
			//Serial1.print("source 03");
			SSP_to_Control_Buffer[i] = ax_ssp_frame[i];

			//	Serial1.print(SSP_to_Control_Buffer[i], HEX);

		}
		//SIZE_SSP_to_Control_Buffer=Rx_length;

		g_infoSize = *tx_ax_length;
		//*ax_ssp_flag=EMPTY;
		*layerflag = EMPTY;
		flag_SSP_to_Control = FULL;
		//flag_next_frame=FULL;
	}

	if (dest_to_framing == source1&& *dataflag==EMPTY&&*checkcontrol==EMPTY) {
		Serial1.print("el type");
		Serial1.print(type_to_framing, HEX);
		if (type_to_framing == 0x04) {
			Serial1.print("\nGET\n");
			uint8 i;

			for (i = 0; i < 5; i++) {
				data[i] = arr[i];
			}
			*dataflag = FULL;
			*data_length = 5;
			*Tx_App_type = type_to_framing;
			*Tx_App_desti = *src_to_framing;
			*checkcontrol = FULL;
			*layerflag = EMPTY;
//flag_next_frame=FULL;
		} else if (type_to_framing == 0x00) {
			Serial1.print("\nPING\n");
			uint8 i;

			for (i = 0; i < 5; i++) {
				data[i] = arr[i];
			}
			*dataflag = FULL;
			*data_length = 5;
			*Tx_App_type = type_to_framing;
			*Tx_App_desti = *src_to_framing;
			*checkcontrol = FULL;
			*layerflag = EMPTY;
//flag_next_frame=FULL;
		} else if (type_to_framing == 0x06) {
			Serial1.print("\nREAD\n");
			uint8 i;

			for (i = 0; i < 5; i++) {
				data[i] = arr[i];
			}
			*dataflag = FULL;
			*data_length = 5;
			*Tx_App_type = type_to_framing;
			*Tx_App_desti = *src_to_framing;
			*checkcontrol = FULL;
			*layerflag = EMPTY;
//flag_next_frame=FULL;
		} else if (type_to_framing == 0x07) {
			Serial1.print("\nWRITE\n");

			uint8 i;

			for (i = 0; i < 5; i++) {
				data[i] = arr[i];
			}
			*dataflag = FULL;
			*data_length = 5;
			*Tx_App_type = type_to_framing;
			Serial1.print(*Tx_App_type, HEX);
			*Tx_App_desti = *src_to_framing;
			*checkcontrol = FULL;
			*layerflag = EMPTY;
//flag_next_frame=FULL;
		}

		else {
			Serial1.print("\n GWA el else\n");

			*dataflag = EMPTY;
			*checkcontrol = EMPTY;
			flag_next_frame = EMPTY;
			*layerflag = EMPTY;
			flag_SSP_to_Control = EMPTY;
			flag_controltossp = EMPTY;
		}
	}
}

/*
 * Description: increments the given State Variable (VR or VS) and overflows if reach value of 7.
 * parameters:
 *        *stateVar: pointer to either VR or VS.
 *
 */

void incrementStateVar(uint8 *stateVar) {
	if (*stateVar < 7) {
		*stateVar = *stateVar + 1;
	} else {
		*stateVar = 0;
	}
}

/*
 * Description: this function acts as Control Layer
 * parameters:
 *
 */

void AX25_Manager(uint8 *a_control) {
	uint8 control;
	uint8 prev_state;
	uint8 i;
//  uint8 SSP_to_Control_Buffer_Copy[SIZE_SSP_to_Control_Buffer];
//  uint8 SSP_to_Control_Buffer[SIZE_SSP_to_Control_Buffer];
	uint8 Deframing_To_Control_Buffer[256];
	uint8 Deframing_To_Control_Buffer_Copy[256];
	static uint8 state = idle;
	uint8 pollfinal = 0;
	static uint8 VS = 0; /* holds the number of the frame to be sent */
	static uint8 VR = 0; /* holds the number of the frame that is expected to be received */
	uint8 NS;
	uint8 NR;
	uint8 Received_NS;
	uint8 PollFinal;
	uint8 Received_PollFinal;
	uint8 Sbits;
	uint8 Received_Sbits;
	uint8 Mbits;
	uint8 Received_Mbits;
	uint8 received_control;
	uint8 Address_Copy[ADDR_LEN];
	uint8 myAddress[ADDR_LEN] = { 'O', 'N', '4', 'U', 'L', 'G', 0x60, 'O', 'U',
			'F', 'T', 'I', '1', 0x61 };
	uint8 notMyAddress = CLEAR;

	//uint8 flag_Status = ACCEPT;
	uint8 g_Recieved_NR_1;
	static uint8 flag_NS_VR; /* flag to indicate if received NS equals VR */

	uint8 flag_RxFrameType; /* used when node acts as RX, indicates type of received frame (I or S) *//*todo: hana */

	/* used to determine timeout for AX Frames */
	static uint32 AX_startTimeout;
	static uint32 AX_endTimeout;

	switch (state) {
	case idle:
		/*----------------------- TX part -----------------------*/
		if ((flag_SSP_to_Control == FULL && flag_Control_to_Framing == EMPTY)) {
#ifdef AX_DEBUG
			Serial.print("\nIdle tx part\n");
#endif
			/*TODO: check from Dr. if we should do this copy or not */

			for (i = 0; i < SIZE_SSP_to_Control_Buffer; i++) {
				info[i] = SSP_to_Control_Buffer[i];
			}

			flag_SSP_to_Control = EMPTY;
			flag_Control_to_Framing = FULL;
			NS = VS;
			NR = VR;
			*a_control = AX25_getControl(I, RR, NS, NR, pollfinal);
			state = TX;
			AX_startTimeout = millis();
		}

		/*----------------------- RX part -----------------------*/
		if ((flag_Control_to_SSP == EMPTY && flag_Deframing_to_Control == FULL)) {

#ifdef AX_DEBUG
			Serial.print("\nIdle rx part\n");
#endif

			/* Checks if received address is ours */
			for (i = 0; i < ADDR_LEN; i++) {
				if (g_received_address[i] != myAddress[i]) {
					notMyAddress = SET; /* set the flag to show the address is not ours */
#ifdef AX_DEBUG

					Serial.println("\n Address is NOT ours \n");
#endif
					flag_Deframing_to_Control = EMPTY; /* clears Buffer in case address is not ours */
					break; /* breaks as soon as it finds a difference in the address */
				}
			}

			/* check if type is I-frame */
			received_control = g_control_recived[0]; /* copy the received cntrol byte */
			if ((received_control & 0x01) == 0) {

				/* indicate that we received an I-Frame so that after idle we go to RX state */
				flag_RxFrameType = I;

				/* get subfield values from the control byte */
				g_Received_NR = (received_control & 0xE0) >> 5;
				Received_NS = (received_control & 0x0E) >> 1;
				PollFinal = (received_control & 0x10) >> 4;

#ifdef AX_DEBUG
				Serial.print("\n Received NS = ");
				Serial.print(Received_NS);
				Serial.print("\n VR = ");
				Serial.print(VR);
#endif

				/* check if Received NS equals V(R) */
				if (Received_NS == VR) {
					flag_NS_VR = SET; /* set flag to indicate that received NS == VR */

				} else {

					/*TODO: check from Dr. make send REJ */
					flag_NS_VR = CLEAR;
					state = RX; /* sets the state to RX in order to send REJ in this case */
					// flag_Deframing_to_Control = EMPTY; /* Discards Frame */
				}
			} else if ((received_control & 0x03) == 0x01) {

				/*todo: check this entire else if from DR. */
				/* indicate that we received an S-Frame so that we stay in idle mode */
				flag_RxFrameType = S;

				/* get subfield values from the control byte */
				g_Received_NR = (received_control & 0xE0) >> 5;
				Received_PollFinal = (received_control & 0x10) >> 4;
				Received_Sbits = (received_control & 0x0C) >> 2;

				/* reset to 0 when > 7 */
				g_Recieved_NR_1 = g_Received_NR - 1;
				if (g_Recieved_NR_1 > 7) {
					g_Recieved_NR_1 = 7;
				}

				/* checks if the received frame has correct order and is of ACK type (RR or RNR) */
				if ((g_Recieved_NR_1) == VS
						&& (Received_Sbits == RR || Received_Sbits == RNR)) {
					//incrementStateVar(&VR);  /*todo:check from dr */
				}
				flag_Deframing_to_Control = EMPTY;
			}
#ifdef AX_DEBUG

			Serial.print("\n CHECK FLAGS\n");
			Serial.print(notMyAddress);
			Serial.print(flag_NS_VR);
			Serial.print(flag_controltossp);
			Serial.print("\n CHECK FLAGS\n");
#endif

			if (notMyAddress
					!= SET&& flag_NS_VR == SET&& flag_controltossp==EMPTY) { /*continues if the address is ours and the number of frame is what is expected*/
				/* copy array to upper layer */
				Serial1.print("\n gwa management layer ezzat\n");

				// Serial1.print(ax_rx_length,HEX);
				for (i = 0; i < SSP_FRAME_MAX_SIZE; i++) {
					Control_To_SSP[i] = g_info_reciver[i];
					//Serial1.print(Control_To_SSP[i], HEX);

				}
				if (flag_RxFrameType == I) {
					flag_controltossp = FULL; /*todo: check if it should be done here only or below aswell (before state = idle line) */

					state = RX; /* sets the state to RX */
				} else if (flag_RxFrameType == S) {
					state = idle;
				}
			}
		}
		break;

	case TX:
		if (flag_Deframing_to_Control == FULL) {
#ifdef AX_DEBUG
			Serial.print("\n TX State \n");
#endif

			received_control = g_control_recived[0];

			/* check which type I or S or U */
			if ((received_control & 0x01) == 0) {

				/* type is I frame */
#ifdef AX_DEBUG
				Serial.print("\nI frame\n");
#endif
				g_Received_NR = (received_control & 0xE0) >> 5;
				Received_NS = (received_control & 0x0E) >> 1;
				Received_PollFinal = (received_control & 0x10) >> 4; /* TODO: check if it should be Poll or PollFinal */

				/*TODO: send info field to upper layer*/
				/*TODO: check from Dr since we already copy I frame in RX part above by few lines */
				/* copy array to upper layer */
				for (i = 0; i < SSP_FRAME_MAX_SIZE; i++) {
					Control_To_SSP[i] = g_info_reciver[i];
				}

			} else if ((received_control & 0x03) == 1) {

				/* type is S frame */
#ifdef AX_DEBUG
				Serial.print("\nS frame\n");
#endif
				g_Received_NR = (received_control & 0xE0) >> 5;
				Received_PollFinal = (received_control & 0x10) >> 4;
				Received_Sbits = (received_control & 0x0C) >> 2;

				/* in case the received NR is 0 this means that we ack frames up to 7 from previous window */
				g_Recieved_NR_1 = g_Received_NR - 1;
				if (g_Recieved_NR_1 > 7) {
					g_Recieved_NR_1 = 7;
				}

//#ifdef DEBUG
//        Serial1.print(g_Recieved_NR_1);
//        Serial1.print(VS);
//        Serial1.print(Received_Sbits);
//#endif
				if ((g_Recieved_NR_1) == VS
						&& (Received_Sbits == RR || Received_Sbits == RNR)) { /* check if frame was received properly or not by other side */
					flag_Status = ACCEPT; /* this means that the frame sent was accepted */
					rejCounter = 0; /* reset REJ counter */

					/*TODO: check from Dr. */
					/* original line was @ line 154 */
					/* moved this here so that a new frame is made only when an RR is received otherwise if we receive REJ we will send the same frame */
					//  flag_SSP_to_Control = EMPTY;
#ifdef AX_DEBUG
					Serial.print("\n The Frame we sent was Accepted \n");
#endif

					/* make values of VS range from 0 --> 7 only */
					incrementStateVar(&VS);

					state = idle;
				} else {

					flag_Status = REJECT;

					if (rejCounter == 2) {
#ifdef AX_DEBUG
						Serial.print("\n Reject Counter Reached 3 so frame is skipped \n");
#endif
						flag_Status = ACCEPT; /* this means that the frame sent was skipped but treat it as accepted*/
						flag_SSP_to_Control = EMPTY;
						incrementStateVar(&VS);
						state = idle;
						rejCounter = 0;
					} else {
						rejCounter++;

#ifdef AX_DEBUG
						Serial.print("\nThe Frame we sent was Rejected\n");
						Serial.print("\n rejCounter: ");
						Serial.print(rejCounter);
						Serial.print("\n");
#endif
					}
					state = idle;
				}

				flag_next_frame = FULL;
			} else {

				/* type is U frame */
#ifdef AX_DEBUG
    Serial.print("\nU frame\n");
#endif
				Received_Mbits = (received_control & 0xEC) >> 2;
				Received_PollFinal = (received_control & 0x10) >> 4;
			}
			flag_Deframing_to_Control = EMPTY;
		} else {

			AX_endTimeout = millis();
			if ((AX_endTimeout - AX_startTimeout) > 2000) {

				/* Serial Message */
#ifdef AX_DEBUG
				Serial.print("\nAX Timeout\n");
#endif
				/*clear flags*/
				flag_Deframing_to_Control = EMPTY;

				/*we then clear the timing variables since they are static*/
				AX_endTimeout = 0;
				AX_startTimeout = 0;

				/* change state of protocol */
				state = idle;
			}
		}
		break;

	case RX:
#ifdef AX_DEBUG
		Serial.print("\n RX State \n");
#endif
		flag_Deframing_to_Control = EMPTY; /* clears Buffer after copying data in it */

		/* Generate Required Control Byte */
		NS = VS;
		NR = VR + 1;
		incrementStateVar(&VS);

		/* check on CRC flag (in de-frame function) if True make RR if False make REJ */
		if (flag_RX_crc == SET && flag_NS_VR == SET) {
#ifdef AX_DEBUG
			Serial.print("\n Accept the received frame \n");
#endif
			//*a_control = AX25_getControl(S, RR, NS, g_Received_NR, pollfinal);
			*a_control = AX25_getControl(S, RR, NS, NR, pollfinal);
			incrementStateVar(&VR); /*(TODO: check from DR.) increments VR if I-frame is accepted */
			flag_NS_VR = CLEAR;
		} else {

#ifdef AX_DEBUG
			Serial.print("\n Reject the received frame \n");
#endif
			*a_control = AX25_getControl(S, REJ, NS, NR, pollfinal);
		}

		/*------------------------------------------*/

		g_infoSize = 0; /* set info field size to 0 in S frame */

		/* Fill address array */
		for (i = 0; i < ADDR_LEN; i++) {
			addr[i] = myAddress[i];
		}

		flag_Control_to_Framing = FULL;

		state = idle;

		break;

	default:
		break;
	}

}

void AX25_buildFrame(uint8 *buffer, uint8 *a_info_ptr, uint16 *frameSize,
		uint8 *ADDR, uint8 control, uint8 infoSize) {
	uint16 i;

	/* Put flags at the right place in the buffer. */
	buffer[0] = 0x7E;

	/* Add the address in the buffer. */
	for (i = 1; i < ADDR_LEN + ADDR_OFFSET; i++) {
		buffer[i] = ADDR[i - 1];
	}
	/* Add the control byte */
	for (; i < CNTRL_OFFSET + CNTRL_LEN; i++) {
		buffer[i] = control;
	}
	/* Add the info field in the buffer. */
	for (; i < infoSize + INFO_OFFSET; i++) {
		buffer[i] = *a_info_ptr;
		a_info_ptr++;
	}
	for (; i < FCS_OFFSET; i++) {
		buffer[i] = 0xaa;
	}

	/* Calculation and insertion of the FCS in the buffer. */
	AX25_putCRC(buffer, &i);

	buffer[i] = 0x7E;
	*frameSize = i + 1;

	flag_Control_to_Framing = EMPTY;
	flag_SerialTXBuffer = FULL;
}

/*
 * Description: takes whole frame and splits it into fields (address, control, info).
 *        and also checks on the frame flags (namely 0x7E) and the CRC
 *
 * Parameters:
 *        buffer: the buffer that contains the full frame which will be de-framed.
 *        frameSize: the size of the frame.
 *        infoSize: the size of the info field in that frame.
 *
 * Notes:
 *        Sets the De-framing to Control flag
 *        controls the flag_RX_crc
 *
 */

void AX25_deFrame(uint8 *buffer, uint16 frameSize, uint8 infoSize) {
	uint8 newbuffer[AX25_FRAME_MAX_SIZE]; // this was set to frameSize, i changed it to AX25_FRAME_MAX_SIZE to test it.
	uint16 crc;
	uint8 *ptrz;
	uint16 i = 0;
	uint16 j;
	flag_RX_crc = CLEAR; /* initially assume CRC is wrong, if CRC is correct it will SET the flag. otherwise it will remain CLEAR */

	ptrz = (uint8*) &crc;
	for (; i < AX25_FRAME_MAX_SIZE; i++) {
		newbuffer[i] = buffer[i];
	}

	if (newbuffer[0] == 0x7E) {
		for (i = 1, j = 0; i < ADDR_LEN + ADDR_OFFSET; i++, j++) {
			g_received_address[j] = newbuffer[i];
		}
		for (j = 0; i < CNTRL_LEN + CNTRL_OFFSET; i++, j++) {
			g_control_recived[j] = newbuffer[i];
		}
		//Serial1.print("\n el size \n");
		for (j = 0; i < infoSize + INFO_OFFSET; i++, j++) {
			g_info_reciver[j] = newbuffer[i];

			//Serial1.print(g_info_reciver[j],HEX);
		} //Serial1.print("\n \n");
		for (j = 0; i < FCS_OFFSET; i++, j++) {
//      g_padding_recived[j] = newbuffer[i];
		}
		flag_Deframing_to_Control = FULL;
		flag_SerialRXBuffer = EMPTY;
		crc = computeCRC(newbuffer, &i);
		ptrz++;
		if (*ptrz == newbuffer[i]) {
			i++;
			ptrz--;
			if (*ptrz == newbuffer[i]) {
				flag_RX_crc = SET;

//        i++;
//        printf("\n**received frame**\n");
//        if (newbuffer[i] == 0x7E) {
//          printf("flag=: %x", newbuffer[i]);
//          printf("\n address:\t");
//          for (i = 0; i < ADDR_LEN; i++) {
//            printf("%x", g_received_address[i]);
//          }
//          printf("\n control byte\t");
//          for (i = 0; i < CNTRL_LEN; i++) {
//            printf("control[%d]=%x\t", i, g_control_recived[i]);
//          }
//
//          printf("\n info \n");
//          for (i = 0; i < infoSize; i++) {
//            printf("%x", g_info_reciver[i]);
//          }
//          printf("\n padding: \t");
//          for (i = 0; i < INFO_MAX_LEN - infoSize; i++) {
//            printf("%x", g_padding_recived[i]);
//          }
//          printf("\nFCS\n");
//          printf("\n CRC = %x\n", crc);
//          printf("\n flag = %x \n",
//              newbuffer[AX25_FRAME_MAX_SIZE - 1]);
//        }
			}
		}
	}

	else { /* prevent from continuously going to de-frame */
		flag_SerialRXBuffer = EMPTY;
	}
}

#if 0
void AX25_prepareIFrame(TX_FRAME *frame, uint8 control) {

  frame[0] = 0x7E;
  uint8 SSID_OctetDest = 0, SSID_OctetSource = 0;
  /*--------------------------------------------------------------------------*
   * AX.25 TX header : < Address  | Control >
   *                   < 14 bytes |  1 byte >
   *
   *   - Frame address data from page 10 in documentation
   *   - Address : Destination = NJ7P  (+ 2 spaces), SSID
   *   - Address : Source = N7LEM (+ 1 space) , SSID
   *--------------------------------------------------------------------------*/

  static uint8 AX25_txAddressField[ADDR_L] = { 'N', 'J', '7', 'P', ' ', ' ',
      SSID_OctetDest, 'N', '7', 'L', 'E', 'M', ' ', SSID_OctetSource };

  /*******************************
   *    config SSID subfield     *
   *******************************/
  uint8 SSIDSource = 0xf;
  uint8 SSIDDest = 0xe;

  SSID_OctetSource |= (1 << 0); /* set X bit */
  SSID_OctetSource |= ((SSIDSource & 0x0F) << 1); /* insert SSID into the SSID octet */

  SSID_OctetDest |= (SSIDDest << 1); /* insert SSID into the SSID octet */

  for (uint16 i = 0; i < ADDR_L; i++) {
    frame->address[i] = AX25_txAddressField[i];
  }

  /******************************
   *    config control field
   ******************************/
  IframeControlField();
  SframeControlField();
  UframeControlField();

}
void printTxFrame(uint8 * tx_ptr, uint16 size) {
  for (uint16 i = 0; i < size; i++) {
    printf("%d", tx_ptr[i]);
  }
}

IframeControlField(TX_FRAME *frame) {
  frame->control = (frame->control & 0x1F) | ((NR << 5) & 0xE0); /* insert N(R) into control field */
  frame->control = (frame->control & 0xF1) | ((NS << 1) & 0x0E); /* insert N(S) into control field */
  frame->control &= ~(1 << 0); /* put zero in BIT0 of control field as in page 3 I frame */
  frame->control &= ~(1 << 4); /* clear P bit as it's not used as stated in section 6.2 */

  NR++;
  NS++;
  if (NR > 7) {
    NR = 0;
  }
  if (NS > 7) {
    NS = 0;
  }
}

SframeControlField(TX_FRAME *frame) {
  frame->control = 1; /* to initially make two LSB = 01 */
  frame->control = (frame->control & 0x1F) | ((NR << 5) & 0xE0); /* insert N(R) into control field */

  if (RRFrame) {
    SSBits = 0;
  } else if (RNR) {
    SSBits = 1;
  } else if (REJ) {
    SSBits = 2;
  } else if (SREJ) {
    SSBits = 3;
  }
  frame->control = (frame->control & 0xF3) | ((SSBits << 2) & 0x0C); /* insert SS Bits into control field */

}

UframeControlField() {

#endif
