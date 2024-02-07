#include <datacommsimlib.h>

void l1_send(unsigned long l2frame, int framelen);
boolean l1_receive(int timeout);
void transmit(unsigned long content, int length);
void transmit_l1();
void select_LED();
void set_transmit_frame();
void set_receive_frame();

Shield sh; 
Transmit tx;
Receive rx;
int state = NONE;
int LED = 0;
unsigned long receivedFrame = 0;

void setup() {
	sh.begin();
	
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(PIN_TX, OUTPUT);
	pinMode(DEB_1, OUTPUT);
	pinMode(DEB_2, OUTPUT);

	state = APP_PRODUCE;
}

void loop() 
{
	switch(state)
	{
		case L1_SEND:
			Serial.println("[State] L1_SEND");
			l1_send(tx.frame, LEN_FRAME);
			state = L1_RECEIVE;
			break;
		case L1_RECEIVE:
			Serial.println("[State] L1_RECEIVE");
			if(l1_receive(2000)) 
			{
				state = L2_FRAME_REC;
				break;
			}
			Serial.println("[Timeout]");
			state = APP_PRODUCE;
			break;
		case L2_DATA_SEND:
			Serial.println("[State] L2_DATA_SEND");
			set_transmit_frame();
			state = L1_SEND;
			break;
		case L2_RETRANSMIT:
			Serial.println("[State] L2_RETRANSMIT");
			break;
		case L2_FRAME_REC:
			Serial.println("[State] L2_FRAME_REC");
			set_receive_frame();
			state = APP_PRODUCE;
			break;
		case L2_ACK_SEND:
			Serial.println("[State] L2_ACK_SEND");
			break;
		case L2_ACK_REC:
			Serial.println("[State] L2_ACK_REC");
			break;
		case APP_PRODUCE: 
			Serial.println("[State] APP_PRODUCE");
			select_LED();
			state = L2_DATA_SEND;
			break;
		case APP_ACT:
			Serial.println("[State] APP_ACT");
			break;
		case HALT:  
			Serial.println("[State] HALT");
			sh.halt();
			break;
		default:
			Serial.println("UNDEFINED STATE");
			break;
	}
}

void l1_send(unsigned long frame, int framelen) 
{
	transmit_l1();
}

boolean l1_receive(int timeout) 
{
	byte FIND_SFD = 0;
	int funcStartTime = millis();
	while (sh.sampleRecCh(PIN_RX) == LOW) if((millis() - funcStartTime) > timeout) return false;

	delay(50);

	while(FIND_SFD != SFD_SEQ)
	{
		if((millis() - funcStartTime) > timeout) return false;
		FIND_SFD = (FIND_SFD << 1) | sh.sampleRecCh(PIN_RX);
		int outputState = (sh.sampleRecCh(PIN_RX) == 1) ? HIGH : LOW;
		digitalWrite(DEB_1, outputState);
		delay(T_S);
	}
	Serial.println("[SFD found]");
	digitalWrite(DEB_1, HIGH);

	for(int i = 0; i < 32; i++)
	{
		int bit = sh.sampleRecCh(PIN_RX);
		receivedFrame = (receivedFrame << 1) | bit;
		Serial.print(bit);
		int outputState = (bit == 1) ? HIGH : LOW;
		digitalWrite(DEB_2, outputState);
		delay(T_S);
	}
	Serial.println();
	return true;
}

void transmit(unsigned long content, int length)
{
	for(int i = length; i > 0; i--)
	{
		delay(T_S);
		byte bit = (content >> i - 1) & 0x01;
		Serial.print(bit);
		int outputState = (bit == 1) ? HIGH : LOW;
		digitalWrite(PIN_TX, outputState);
	}
}

void transmit_l1()
{
	transmit(PREAMBLE_SEQ, LEN_PREAMBLE);
	transmit(SFD_SEQ, LEN_SFD);
	transmit(tx.frame, LEN_FRAME);
	Serial.println();
}

void select_LED()
{
	LED = sh.select_led();
}

void set_transmit_frame()
{
	tx.frame_from = 0;
	tx.frame_to = 0;
	tx.frame_type = 0;
	tx.frame_seqnum = 0;
	tx.frame_payload = LED;
	tx.frame_crc = 0;
	tx.frame_generation();
}

void set_receive_frame()
{
	rx.frame = receivedFrame;
	rx.frame_decompose();
	rx.print_frame();
}