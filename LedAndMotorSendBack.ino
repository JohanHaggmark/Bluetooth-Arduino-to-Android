#include <SoftwareSerial.h>


int bluetoothTx = 11;
int bluetoothRx = 10;
SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

int ledPin = 4;
int motorPin = 3;
int motorSpeedConverted = 0;
int percentageMotorSpeed = 0;
String data = "";
boolean ledOn;
boolean motorOn;
boolean completePackage;

void setup() {
  Serial.begin(9600);
  Serial.println(">> START<<");

  bluetooth.begin(9600);
 
  pinMode(ledPin, OUTPUT);
  pinMode(motorPin, OUTPUT);
  
}

void loop() {
  while(bluetooth.available() && !completePackage){
    char character = (char)bluetooth.read();
    data += character; 
    Serial.println(data);
    //When bluetooth reads E, the message is done transmitted,
    //and Arduino should read the data that was received.
    if(character == 'E'){
      completePackage = true;
    }
  }  
  
  if(completePackage){
    readData(data);
    //after the data package is read, it should be erased
    data = "";
    completePackage = false;   
    }
    
  if(motorOn){
    motorSpeedConverted = map(percentageMotorSpeed, 0, 99, 0, 255);
    analogWrite(motorPin, motorSpeedConverted);  
  } else {
    analogWrite(motorPin, 0);
  }
         
  if(ledOn){
    digitalWrite(ledPin, HIGH);
    } else {
      digitalWrite(ledPin, LOW);
    } 
}

void readData(String data){
    //The first letter indicates what type of message it is. (L for Led) (M for Motor)         
    if(data[0] == 'L'){       
      if(data == "LONE"){   
        bluetooth.write("on");    
        ledOn = true;      
        } 
        else if(data == "LOFE"){
         bluetooth.write("koff");
         ledOn = false;
         }       
      } else if(data[0] == 'M'){
         //checks if data has 1 or 2 digits. Then the digit/digits converts to 
         //an Integer variable
         //C language does not handle modern String variable. We want to remove the letters
         //from the data-String, but that is not possible. 
         //Data is not a variable with dynamically memory allocation. 
         //Instead we need to copy the digits from data variable into a new char array:
         char speedMotor[2];
         if(isdigit(data[2])){
          // char speedMotor[2];
           strncpy(speedMotor, data.c_str()+1, 2);
           percentageMotorSpeed = atoi(speedMotor);         
         } else {
        //   char speedMotor[1];          
           strncpy(speedMotor, data.c_str()+1, 1);
           percentageMotorSpeed = atoi(speedMotor);
           
         }    
         bluetooth.write(speedMotor);
         if(percentageMotorSpeed > 0){
          motorOn = true;  
         } else {
          motorOn = false;
         }
      }
}

