#include <Wire.h>
#include <RFduinoBLE.h>
#include "pitches.h" 
#include "PlayMelody.h"

#define MAX_BUFFER_SIZE 50
//201632004 김지혜

char serialCmdData[MAX_BUFFER_SIZE]; // serial input buffer
String inData;
const int temperaturePin = 2;  //  온도 핀이 연결된 핀을 2번으로 설정  
int button1 = 3;
int button2 =4;
int state = 0;
int buttonState1 = LOW; //왼쪽버튼 상태 - 카운트 값 증가
int buttonState2 = LOW; //오른쪽버튼 상태 - 카운트 값 감소
int speakerPin = 5; // 입력 핀의 상태를 읽기 위한 변수
int led=6;
int numTones = 8;
int DHpin = 1; // 온습도 핀
byte dat [5];

unsigned int tones[] = {262, 294, 330, 349, 392, 440, 494, 523};

char *help[] = {"help", "temp", "school", "onoff", "temp_humid"};

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

void ex_help_command()
{
   
          Serial.print("1. temp, 2. 학교종이 땡땡땡 연주, 3. led on/off(100ms) 4. 온습도 \n");
 
}


void serial_cmd_input()
{
    int count = 0;
    char rx_data = 0;

    while(!Serial.available());
    while (Serial.available() > 0){
        delay(50);
        rx_data = Serial.read();
        serialCmdData[count] = rx_data;
      //  Serial.print(rx_data, HEX);
        
        // Process message when new line character is recieved
        if (rx_data == '\r' || rx_data == '\n' || rx_data == '\a')
        {
            Serial.print("Blueino Received: ");
            serialCmdData[count] = 0;
            Serial.println(serialCmdData);
        }
        count = (count+1) % MAX_BUFFER_SIZE;
    }
}

int cmd_interpret(char *cmd)
{
     for(int i=0; i< (sizeof(help)/sizeof( char *)); i++)
     {
         if(!strcmp(help[i], cmd))
             return i;
     }
     return 0;
}
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


void play_school_bell(void)
{ 
  state=2;
  display_number(state); 
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

void temp()
{
   state=1;
  display_number(state); 
   int reading = analogRead(temperaturePin);    // 아날로그 입력 온도값을 읽음  
   Serial.print(((3.0*reading/1024.0))*100);           // LM35 온도 계산법 =             
                                                             // 온도 = (3.0*reading*100.0)/1024.0
   Serial.println("c");     // 컴퓨터에 “C” 문장을 시리얼 통신으로 보냄                    
   delay(1000);           // 시리얼 포터의 버퍼를 방지하기 위함 
}

void onoff_led(){
  state=3;
  display_number(state); 
 if(digitalRead(led)==HIGH){
    digitalWrite(led,LOW);
 }else if(digitalRead(led)==LOW){
    digitalWrite(led,HIGH);
 }
}

int plus(uint32_t ulPin)
{
  Serial.println("왼쪽 버튼 누름");
  state++;
  Serial.println(state);
  if(state>4) state=1;
  if(state==1){ temp();}
  else if(state==2) {play_school_bell();}
  else if(state==3) {onoff_led();} 
  else if(state==4) {temp_humid();}
}

int minus(uint32_t ulPin)
{
   Serial.println("오른쪽 버튼 누름");
  state--;
  Serial.println(state);
  if (state<1)state=4;
  if(state==1){ temp();}
  else if(state==2) {play_school_bell();}
  else if(state==3) {onoff_led();}
  else if(state==4) {temp_humid();}
}

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
void temp_humid(){
        state=4;
        display_number(state); 
        start_test ();
        Serial.print ("Current humdity =");
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
}
void setup() {
   pinMode(temperaturePin, INPUT);   //  온도 핀을 입력으로 설정
   pinMode(button1, INPUT); 
   pinMode(button2, INPUT);
   pinMode(speakerPin, OUTPUT); 
   pinMode(led,OUTPUT);
   pinMode(DHpin,OUTPUT);
   digitalWrite(DHpin, HIGH);
   attachPinInterrupt(button1,plus,HIGH);//버튼1 HIGH 입력시 plus 호출
   attachPinInterrupt(button2,minus,HIGH);//버튼2 HIGH 입력시 minus 호출
   
   for(int i=1; i<9; i++){
     pinMode(seg7_pin[i], OUTPUT);
     delay(10);
     digitalWrite(seg7_pin[i], LOW);
   } 
   
   Serial.begin(9600);  // 시리얼 데이터를 주고받을 수 있게 준비함 
}

void loop() 
{
   // 문자가 전송되면
    char input;

    if(Serial.available()){
      input = Serial.read();
      Serial.println(input);
    }
    

    switch(input){
         case 'h': ex_help_command(); break;
         case '1': temp(); break;
         case '2': play_school_bell(); break;
         case '3': onoff_led(); break;
         case '4': temp_humid(); break;
                 
   
         default:  break;
     }

} 
