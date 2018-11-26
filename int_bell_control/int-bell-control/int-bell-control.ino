#include "pitches.h"
#include "PlayMelody.h"

#define IDLE_STATE  0x00
#define DO_RE_MI    0x01
#define SCHOOL_BELL 0x02
#define MY_SONG     0x03
#define TEMP_HUMID  0x04

#define STATE_MAX  5

#define   TRUE    0x01
#define  FALSE    0x00

int DHpin = 4; // 온습도 핀
int button = 3;  // 버튼 핀
int speakerPin = 2; // 입력 핀의 상태를 읽기 위한 변수
int ledPin = 5;
int led13 = 13;

 byte dat [5];

// 온습도 데이타를 읽기 위한 함수
byte read_data() {
   byte data = 0;
   for (int i = 0; i < 8; i ++) {
     if (digitalRead (DHpin) == LOW) {
       while (digitalRead (DHpin) == LOW); // wait for 50us
       delayMicroseconds (30); // determine the duration of the high level to determine the data is '0 'or '1'
       if (digitalRead (DHpin) == HIGH)
         data |= (1 << (7-i)); // high front and low in the post
       while (digitalRead (DHpin) == HIGH); // data '1 ', wait for the next one receiver
      }
   }
 return data;
 }

// 온습도 칩의 구동을 위한 절차 수행
 void start_test () {
//   pinMode(DHpin, OUTPUT); // bus down, send start signal
//   delay (30); // delay greater than 18ms, so DHT11 start signal can be detected
  
   digitalWrite (DHpin, LOW);
   delay(30);
   digitalWrite (DHpin, HIGH);
   delayMicroseconds (20); // Wait for DHT11 response
  
   pinMode (DHpin, INPUT);
   while (digitalRead (DHpin) == HIGH);
   delayMicroseconds (80); // DHT11 response, pulled the bus 80us
   while(digitalRead (DHpin) == LOW);
   delayMicroseconds (80); // DHT11 80us after the bus pulled to start sending data
   while(digitalRead (DHpin) == HIGH);

  for (int i = 0; i < 4; i ++) // receive temperature and humidity data, the parity bit is not considered
     dat[i] = read_data ();
  
   pinMode (DHpin, OUTPUT);
   digitalWrite (DHpin, HIGH); // send data once after releasing the bus, wait for the host to open the next Start signal
 }


int state_mode = STATE_MAX - 1;
int state_flag = FALSE;
unsigned int led_count = 0x00;
int buttonState = LOW;

int numTones = 8;
unsigned int tones[] = {262, 294, 330, 349, 392, 440, 494, 523};

// 멜로디를 위한 음표들:
unsigned int melody[] = {
   NOTE_G4, NOTE_G4, NOTE_A4, NOTE_A4, NOTE_G4, NOTE_G4, NOTE_E4,
   NOTE_G4, NOTE_G4, NOTE_E4, NOTE_E4, NOTE_D4, 0
};

// 음표 길이들 : 4 = 4분(1/4) 음표, 8 = 8분(1/8) 음표, 
unsigned int noteDurations[] = {
   4, 4, 4, 4, 4, 4, 2,
   4, 4, 4, 4, 2, 2
};

void play_doremi() {
	for(int i=0; i<numTones; i++){
		tone(speakerPin, tones[i]);
        // tone(pin, frquency, duration);
        delay(500);
      }
    noTone(speakerPin);
}

void play_school_bell(void)
{ 
	int noteDuration = 0, thisNote = 0, pauseBetweenNotes = 0;
	for (thisNote = 0; thisNote < sizeof(noteDurations) / sizeof(int); thisNote++) {

		// 음표 길이를 계산하기 위하여, 1초를 음표 타입으로 나눕니다. 
		// 예를 들어, 4분 음표 = 1000 / 4,  8분 음표 = 1000/8 등과 같이
		// 합니다.
		noteDuration = 1000 / noteDurations[thisNote];
		tone(speakerPin, melody[thisNote]);
		delay(noteDuration);

		// 음표들을 구분하기 위하여, 음표 사이에 최소한의 지연 시간을
		// 음표 길이 + 30%가 잘 동작하는 것 같습니다:
		noTone(speakerPin);
		pauseBetweenNotes = noteDuration * 1.30;
		delay(pauseBetweenNotes);
	}
     // 음 발생하는 것을 멈춤니다:
     noTone(speakerPin);
}

void play_my_song(void)
{
  PlayMelody(speakerPin, "t380r8v12l16a>dv13fav14>dfv15afv14d<av13f8r4v12<a>cv13eav14>cev15aev14c<av13er4<c-c<a+b>d+f+abb+a+bn66a+bn66r2erfrerd+r2rdrerdrc+r>>crdrcr<brarb+rbrarg+rfrerd+rerdrcl64ab>cdefgab>cdev11l16<<ag+abb+b>cdedcc-dc<bg+ag+abb+b>cdedcc-dc<bg+ag+a>ceceafdn70dfn70dfe<ab+ab+eaeg+8ef+gef+g+ag+abb+b>cdedcc-dc<bg+ag+abb+b>cdedcc-dc<bg+ag+a>ceceafdn70dfn70dfe<ab+ab+eaeg+8ef+gef+g+v13ag+abb+b>cdfn70dfed+e8ec<a>cc<aea>ec<a>caeceaecebg+eg+aecebg+eg+aecebg+eg+aecebg+eg+aeg+eaeg+eaeg+eaeg+eaeg+eaeg+eaeg+eaeg+ea8r4.g+8r4.v15ar2.r8");                            
  PlayMelody(speakerPin, "O4"); // 옥타브를 다시 초기값 4로 설정
}

// ISR for button click
//
 int btn1_int_handler(uint32_t ulPin)
{ 
  Serial.println("Interrupt_pin1");
//pinMode(led, OUTPUT);
//  digitalWrite(led13, HIGH);
  state_flag = TRUE;
  state_mode = (state_mode + 1) % STATE_MAX;
//  delay(100);
}

//int LED = 3;      // LED핀을 3번으로 설정
//int buttonState = 0; // 입력 핀의 상태를 읽기 위한 변수

 // e, d, c ,dp, b, a, f, g
 // 7, 8, 9, 10, 11, 12, 13, 14
 
int seg7_pin[]={6, 12, 11, 9, 8, 7, 13, 14, 10};
int  digit[][8] ={
   {1, 1, 1, 1, 1, 1, 0, 0}, // 0
   {0, 1, 1, 0, 0, 0, 0, 0}, // 1
   {1, 1, 0, 1, 1, 0, 1, 0},
   {1, 1, 1, 1, 0, 0, 1, 0},
   {0, 1, 1, 0, 0, 1, 1, 0},
   {1, 0, 1, 1, 0, 1, 1, 0},
   {1, 0, 1, 1, 1, 1, 1, 0}, // 6
   {1, 1, 1, 0, 0, 1, 0, 0},
   {1, 1, 1, 1, 1, 1, 1, 0},
   {1, 1, 1, 0, 0, 1, 1, 0},
   {0, 0, 0, 0, 0, 0, 0, 1}, 
 };

void display_number(int n)
 {
   for(int i=1; i<9; i++)
     digitalWrite(seg7_pin[i], digit[n][i-1]);
 }


void setup() {
  pinMode(led13, OUTPUT);  // LED핀을 출력으로 설정
  pinMode(button, INPUT);  
  attachPinInterrupt(button, btn1_int_handler, LOW);  
  pinMode(DHpin, OUTPUT);     
  digitalWrite(DHpin, HIGH);
  pinMode(speakerPin, OUTPUT);   
  pinMode(ledPin, OUTPUT);   

  for(int i=1; i<9; i++){
     pinMode(seg7_pin[i], OUTPUT);
     delay(10);
     digitalWrite(seg7_pin[i], LOW);
   }  
   display_number(state_mode);

  Serial.begin(9600);
//  digitalWrite(13, HIGH);
}

void led13_onoff()
{
    digitalWrite(13, ++led_count%2);
    delay(100);
}


void play_and_state_change(void)
{
  if(state_flag){
    state_flag = FALSE;
    display_number(state_mode);
//       delay(1000);
  }
  else return;

//  buttonState = digitalRead(button);
  switch(state_mode){
    case IDLE_STATE:
	      Serial.println("0. led blink");    
        blink_ledPin();
        break;
      
    case DO_RE_MI:
    	  Serial.println("1. doromi song");
        play_doremi();
        break;
      
    case SCHOOL_BELL:
	      Serial.println("2. school bell");
        play_school_bell();
        break;
      
    case MY_SONG:
	      Serial.println("3. game bell");    
  //      play_my_song();
        break;  

    case TEMP_HUMID:
        start_test ();
        Serial.print ("4. Current humdity =");
        Serial.print (dat [0], DEC); // display the humidity-bit integer;
        Serial.print ('.');
        Serial.print (dat [1], DEC); // display the humidity decimal places;
        Serial.println ('%');
        Serial.print ("Current temperature =");
        Serial.print (dat [2], DEC); // display the temperature of integer bits;
        Serial.print ('.');
        Serial.print (dat [3], DEC); // display the temperature of decimal places;
        Serial.println ('C');
        delay (100);    
        break;  

    default: Serial.println("State Error");
  }
  delay(10);
  
}
void loop() {
  char c=0;
	//  c = digitalRead(button);
	  // 디지털입력 핀인  버튼이 눌려졌는지를 읽음

	//	delay(500);
	//	led13_onoff();

  play_and_state_change();

//        delay(100);
  
  if (Serial.available()) {     
    c = Serial.read();
    if( c >= '0' && c <= '9'){
      c = c - '0';
      state_mode = c % STATE_MAX;
      state_flag = TRUE;
    }
     else {
       Serial.println("type 0 ~ 9 ");
     }
  }
}

void blink_ledPin(){
      digitalWrite(ledPin, HIGH);   // LED 핀을 High로 출력
      delay(200);
      digitalWrite(ledPin, LOW);   // LED 핀을 Low로 출력
      delay(200);      
}
