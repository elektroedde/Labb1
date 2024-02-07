// Simulated simple MasterNode
// 2021-01-25
// Jens A Andersson, EIT, LTH

int bin_to_int(byte bin_array[], int start, int len);

const int IO1 = 1; // Tx
const int IO2 = 13; //Rx
const byte SYMBOL_OUT[] = {1,0,1,0,1,0,1,0,0,1,1,1,1,1,1,0,0,0,1,1,1,1,0,0,0,0,0,1,0,0,0,1,0,0,0,0,1,1,1,1,0,1,0,0,1,0,1,1};
const int T_S = 100; // symbol time
const int NUM_SYMBS = 48; // 8+8+32

int i;
byte symbol_in[48];

int frameType;
int framePayload;

void setup() {
	pinMode(IO2, INPUT);
	pinMode(IO1, OUTPUT);
	digitalWrite(IO1,LOW);
}

void loop() {
	while (digitalRead(IO2) == LOW) {
	} // wait for PREAMBLE start
	delay(0.5*T_S);
	for (i = 0; i < NUM_SYMBS; i++) {
		symbol_in[i] = digitalRead(IO2);
		delay(T_S);
	}			

	frameType = bin_to_int(symbol_in,24,4);
	framePayload = bin_to_int(symbol_in,32,8);

	for (i = 0; i < NUM_SYMBS; i++) {
		digitalWrite(IO1, SYMBOL_OUT[i]);
		delay(T_S);
	}
	digitalWrite(IO1, LOW);
}

int bin_to_int(byte bin_array[], int start, int len) {
	int s, j, result;
	result = 0;
	s = start; 
	j = 0;
	while (j<len) {
		//Serial.println(result);
		result <<= 1;
		result |= (bin_array[s] & 1);
		++j;
		++s;
	}
	return result;
}
