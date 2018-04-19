#include <Wire.h>
#include <RFduinoBLE.h>
#include "pitches.h" 
#include "PlayMelody.h"

#define IDLE_STATE  0x00
#define ONLY_DO     0x01
#define DO_RE_MI    0x02
#define SCHOOL_BELL 0x03
#define MY_SONG     0x04

//201632004 김지혜
int button = 3;
int state_mode = IDLE_STATE;
int buttonState = LOW; //버튼 상태
int speakerPin = 2; // 입력 핀의 상태를 읽기 위한 변수
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

void setup() {
  pinMode(button, INPUT);  
  pinMode(speakerPin, OUTPUT); 
}

int    play_and_state_change(void)
{
  buttonState = digitalRead(button);
  switch(state_mode){
    case IDLE_STATE:
      if(buttonState == HIGH) {
        tone(speakerPin, 262, 1000);
        delay(2000);
        state_mode = DO_RE_MI;
      }
      break;
      
    case DO_RE_MI:
      if(buttonState == HIGH){
        play_doremi();
        state_mode = SCHOOL_BELL;
      }
      break;
      
    case SCHOOL_BELL:
      if(buttonState == HIGH){
        play_school_bell();
        state_mode = MY_SONG;
      }
      break;
      
    case MY_SONG:
      if(buttonState == HIGH){
        play_my_song();
        state_mode = IDLE_STATE;
      }
      break;  
      
    default: Serial.println("State Error");
  }
  delay(10);
  
}
void loop()
{
  //  c = digitalRead(button);

    // 디지털입력 핀인  버튼이 눌려졌는지를 읽음

  //  delay(500);
  //  led13_onoff();

  play_and_state_change();

  char  temp[100];
  int   length;
  if (Serial.available()) {
    length = Serial.readBytes(temp, 20);
    temp[length] = 0x00;
    Serial.println(temp);
  }
//  play_doremi();
}
