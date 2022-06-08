#include <ArduinoBLE.h>

BLEService newService("180A"); // creating the service

BLEUnsignedCharCharacteristic randomReading("2A58", BLERead | BLENotify); // creating the Analog Value characteristic
BLEByteCharacteristic switchChar("2A57", BLERead | BLEWrite); // creating the LED characteristic
BLEByteCharacteristic eyeChar("2c36f468-2e63-40d7-8433-9942d2cbd241", BLERead | BLEWrite);
BLEByteCharacteristic eyelidChar("af13d297-a502-419a-be4e-a3cd3552bcac", BLERead | BLEWrite);

long previousMillis = 0;

void setup_ble() {
  //initialize ArduinoBLE library
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy failed!");
    while (1);
  }

  BLE.setLocalName("MKR WiFi 1010"); //Setting a name that will appear when scanning for Bluetooth® devices
  BLE.setAdvertisedService(newService);

  newService.addCharacteristic(switchChar); //add characteristics to a service
  newService.addCharacteristic(eyeChar);
  newService.addCharacteristic(eyelidChar);
  newService.addCharacteristic(randomReading);

  BLE.addService(newService);  // adding the service

  switchChar.writeValue(0); //set initial value for characteristics
  eyeChar.writeValue(0);
  eyelidChar.writeValue(0);
  randomReading.writeValue(0);

  BLE.advertise(); //start advertising the service
  Serial.println("Bluetooth® device active, waiting for connections...");
}


void loop_ble() {

  BLEDevice central = BLE.central(); // wait for a Bluetooth® Low Energy central

  if (central) {  // if a central is connected to the peripheral
    Serial.print("Connected to central: ");

    Serial.println(central.address()); // print the central's BT address

    //digitalWrite(LED3, HIGH); // turn on the LED to indicate the connection

    while (central.connected()) { // while the central is connected:
      long currentMillis = millis();

      if (currentMillis - previousMillis >= 200) {
        previousMillis = currentMillis;

        int randomValue = analogRead(A1);
        randomReading.writeValue(randomValue);

        if (switchChar.written()) {
          if (switchChar.value()) {   // any value other than 0
            Serial.println("LED on");
            digitalWrite(LED3, HIGH);         // will turn the LED on
          } else {                              // a 0 value
            Serial.println(F("LED off"));
            digitalWrite(LED3, LOW);          // will turn the LED off
          }
        }

        if (eyeChar.written()) {
          int value = eyeChar.value();
          Serial.print("eye command ");
          Serial.println(value);
          eyeCommand(value);
        }

        if (eyelidChar.written()) {
          int value = eyelidChar.value();
          Serial.print("eyelid command ");
          Serial.println(value);
          eyelidCommand(value);
        }
      }
    }

    //digitalWrite(LED_BUILTIN, LOW); // when the central disconnects, turn off the LED
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}