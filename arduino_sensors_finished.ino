String action;


/// temp code
/// ---------

#include "DHT.h"
const int DHTPIN = 6;     // what temp pin we're connected to
//#define DHTTYPE DHT11   // DHT 11 
#define DHTTYPE DHT22     // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)
DHT dht(DHTPIN, DHTTYPE);








/// button code
/// ---------
const int buttonPin = A0;    // the number of the pushbutton pin
const int ledPin    = 13;    // the number of the LED pin
int buttonState     = 0;     // variable for reading the pushbutton status





/// motor driver code
/// ---------
#include <AFMotor.h>
AF_DCMotor motor1(1);
AF_DCMotor motor2(2);






/// rf code
/// ---------
#include <RCSwitch.h>
RCSwitch mySwitch = RCSwitch();






void setup() {

    Serial.begin(2000000);

    

    /// temp code
    /// ---------
    dht.begin();


    
    /// button code
    /// --------- 
    // initialize the LED pin as an output:
    pinMode(ledPin, OUTPUT);
    // initialize the pushbutton pin as an input:
    pinMode(buttonPin, INPUT);


    /// rf code receive
    /// ---------
    mySwitch.enableReceive(1);
    // 0 for uno


    /// rf code transmit
    /// ---------
    mySwitch.enableTransmit(3);
    // 3 for uno
}










void loop() {

    // get action from raspberry pi
    if (Serial.available()) {
      action = Serial.readString();
    }
    

    // split string into parts
    String action_a = getValue(action, '|', 0);
    String action_b = getValue(action, '|', 1);
    String action_c = getValue(action, '|', 2);
    String action_d = getValue(action, '|', 3);


    // convert string parts into integers
    long action_b_int = action_b.toInt();
    int action_c_int  = action_c.toInt();
    int action_d_int  = action_d.toInt();
   
    /// temp code
    /// ---------
    if (action_a == "input_temp") {
      // Reading temperature or humidity takes about 250 milliseconds!
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
      float h = dht.readHumidity();
      float t = dht.readTemperature();
  
      // check if returns are valid, if they are NaN (not a number) then something went wrong!
      if (isnan(t) || isnan(h)) {
          Serial.println("Failed to read from DHT");
      } else {

          aprintf("%f|%f", t, h);
          
      }
    }



      /// button code
      /// ---------
      
      // read the state of the pushbutton value:
      buttonState = digitalRead(buttonPin);
  
      // check if the pushbutton is pressed.
      // if it is, the buttonState is HIGH:
      if (buttonState == HIGH) {
          // turn LED on:
          digitalWrite(ledPin, HIGH);
      }
      else {
          // turn LED off:
          digitalWrite(ledPin, LOW);
      }




      

    /// motor code
    /// ---------
    if (action_a == "input_motor_ph_up") {
      //Serial.println("pH up on");
      motor1.run(FORWARD);
      motor1.setSpeed(action_c_int);
      delay(action_b_int);
      motor1.run(RELEASE);
      Serial.println("done");
    }
    if (action_a == "input_motor_ph_down") {
      //Serial.println("pH down on");
      motor2.run(FORWARD);
      motor2.setSpeed(action_c_int);
      delay(action_b_int);
      motor2.run(RELEASE);
      Serial.println("done");
    }









    /// rf code receive
    /// ---------
    if (action_a == "input_read_rf") {
      
      Serial.print(mySwitch.getReceivedValue());
      Serial.print("|");
      Serial.print(mySwitch.getReceivedBitlength());
      Serial.print("|");
      Serial.print(mySwitch.getReceivedDelay());
      
    }





    /// rf code transmit
    /// ---------
    if (action_a == "input_send_rf") {
      
      // send 3 times to make sure code was successfully sent
      mySwitch.setPulseLength(action_d_int);
      mySwitch.send(action_b_int, action_c_int);

      mySwitch.setPulseLength(action_d_int);
      mySwitch.send(action_b_int, action_c_int);

      mySwitch.setPulseLength(action_d_int);
      mySwitch.send(action_b_int, action_c_int);
      
    }






    // reset the variable action
    action            = "";
    action_a          = "";
    action_b          = "";
    action_c          = "";
    action_d          = "";
    mySwitch.resetAvailable();
}











// functions


// split incoming string into seperate values
String getValue(String data, char separator, int index) {
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}






// print mutiple lines on sames return string
int aprintf(char *str, ...) {
  int i, j, count = 0;

  va_list argv;
  va_start(argv, str);
  for(i = 0, j = 0; str[i] != '\0'; i++) {
    if (str[i] == '%') {
      count++;

      Serial.write(reinterpret_cast<const uint8_t*>(str+j), i-j);

      switch (str[++i]) {
        case 'd': Serial.print(va_arg(argv, int));
          break;
        case 'l': Serial.print(va_arg(argv, long));
          break;
        case 'f': Serial.print(va_arg(argv, double));
          break;
        case 'c': Serial.print((char) va_arg(argv, int));
          break;
        case 's': Serial.print(va_arg(argv, char *));
          break;
        case '%': Serial.print("%");
          break;
        default:;
      };

      j = i+1;
    }
  };
  va_end(argv);

  if(i > j) {
    Serial.write(reinterpret_cast<const uint8_t*>(str+j), i-j);
  }

  return count;
}
