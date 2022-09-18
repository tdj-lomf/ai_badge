#include <ArduinoBLE.h>

BLEService newService("180A"); // creating the service

BLEUnsignedCharCharacteristic randomReading("2A58", BLERead | BLENotify); // creating the Analog Value characteristic
BLEByteCharacteristic switchChar("2A57", BLERead | BLEWrite); // creating the LED characteristic
BLEByteCharacteristic eyeChar("2c36f468-2e63-40d7-8433-9942d2cbd241", BLERead | BLEWrite);
BLEByteCharacteristic eyeSlowChar("ca6b7de6-91f7-4641-8666-f8507d2ec582", BLERead | BLEWrite);
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
  newService.addCharacteristic(eyeSlowChar);  // slow mode
  newService.addCharacteristic(eyelidChar);
  newService.addCharacteristic(randomReading);

  BLE.addService(newService);  // adding the service

  switchChar.writeValue(0); //set initial value for characteristics
  eyeChar.writeValue(0);
  eyeSlowChar.writeValue(0);
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

    long motorOnMillis = millis();

    while (central.connected()) { // while the central is connected:
      long currentMillis = millis();
      long duration = currentMillis - motorOnMillis;
      if (duration >= 2000) {
        setMotorPower(false);
      }

      if (duration >= 8000) {
        double ex = random(100) - 50;
        double ey = random(100) - 50;
        if (duration >= 15000) {
          moveEyeSync(ex, ey, 250); // 15秒に1回以上は瞳をやや動かす
          motorOnMillis = millis();
        }
        else if (duration % 1000 == 0) {  // 1秒おきに判定
          if (random(500) < 10) {  // 2%? 想定より頻度が高くなっている気がする
            moveEyeSync(ex, ey, 250); // たまに瞬きする
            motorOnMillis = millis();
          }
        }
      }

      if (currentMillis - previousMillis >= 10) {
        previousMillis = currentMillis;

        int randomValue = analogRead(A1);
        randomReading.writeValue(randomValue);

        if (switchChar.written()) {
          if (switchChar.value()) {   // any value other than 0
            Serial.println("LED on");
            digitalWrite(LED3, HIGH);         // will turn the LED on
          } else {                              // a 0 value
            Serial.println(("LED off"));
            digitalWrite(LED3, LOW);          // will turn the LED off
          }
        }

        const bool slowMode = eyeSlowChar.written();
        const bool normalMode = eyeChar.written();

        if (normalMode | slowMode) {

          const int value = normalMode ? eyeChar.value() : eyeSlowChar.value();
          int xValue = int(value >> 4);  // [0, 14]
          int yValue = int(value & 0x0F);  // [0, 14]
          const double currentX = getEXState();
          const double currentY = getEYState();
          if (currentY <= currentX && currentY >= -currentX) { // 右方向から移動する場合
            if (7 <= xValue && xValue <= 8 && 6 <= yValue && yValue <= 8) {  // 中心縦3マス+その右が行先のとき、左に修正
              xValue -= 1;              
            }
          }
          const int xRate = (xValue - 7) * 14;  // [0, 14] -> [-96%, 96%]
          const int yRate = (yValue - 7) * 14;  // [0, 14] -> [-96%, 96%]
          Serial.print("eyeX ");
          Serial.println(xRate);
          Serial.print("eyeY ");
          Serial.println(yRate);
          setMotorPower(true);
          motorOnMillis = millis();
          if (normalMode) {
            moveEyeP((double)xRate, (double)yRate);
          } else {
            moveEyeSync((double)xRate, (double)yRate, 1000);
          }
        }

        if (eyelidChar.written()) {
          const int value = eyelidChar.value();
          const int slowEyelid = int(value >> 7);  // 上位8bit目が1のとき、低速モード
          const int commandId = int(value & 0b01111111);
          Serial.print("eyelid command ");
          Serial.println(commandId);
          Serial.print("slow mode ");
          Serial.println(slowEyelid);
          const int time_ms = slowEyelid ? 2000 : 300;
          if (slowEyelid) {
            eyelidCommand(commandId, time_ms);
          }
          motorOnMillis = millis();
        }
      }
    }

    //digitalWrite(LED_BUILTIN, LOW); // when the central disconnects, turn off the LED
    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}