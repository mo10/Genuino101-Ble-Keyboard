#include <CurieBLE.h>

const unsigned char  KeyboardReportDescriptor[76] = {
  0x05, 0x01,                         // Usage Page (Generic Desktop)
  0x09, 0x06,                         // Usage (Keyboard)
  0xA1, 0x01,                         // Collection (Application)
  0x05, 0x07,                         //     Usage Page (Key Codes)
  0x19, 0xe0,                         //     Usage Minimum (224)
  0x29, 0xe7,                         //     Usage Maximum (231)
  0x15, 0x00,                         //     Logical Minimum (0)
  0x25, 0x01,                         //     Logical Maximum (1)
  0x75, 0x01,                         //     Report Size (1)
  0x95, 0x08,                         //     Report Count (8)
  0x81, 0x02,                         //     Input (Data, Variable, Absolute)

  0x95, 0x01,                         //     Report Count (1)
  0x75, 0x08,                         //     Report Size (8)
  0x81, 0x01,                         //     Input (Constant) reserved byte(1)

  0x95, 0x05,                         //     Report Count (5)
  0x75, 0x01,                         //     Report Size (1)
  0x05, 0x08,                         //     Usage Page (Page# for LEDs)
  0x19, 0x01,                         //     Usage Minimum (1)
  0x29, 0x05,                         //     Usage Maximum (5)
  0x91, 0x02,                         //     Output (Data, Variable, Absolute), Led report
  0x95, 0x01,                         //     Report Count (1)
  0x75, 0x03,                         //     Report Size (3)
  0x91, 0x01,                         //     Output (Data, Variable, Absolute), Led report padding

  0x95, 0x06,                         //     Report Count (6)
  0x75, 0x08,                         //     Report Size (8)
  0x15, 0x00,                         //     Logical Minimum (0)
  0x25, 0x65,                         //     Logical Maximum (101)
  0x05, 0x07,                         //     Usage Page (Key codes)
  0x19, 0x00,                         //     Usage Minimum (0)
  0x29, 0x65,                         //     Usage Maximum (101)
  0x81, 0x00,                         //     Input (Data, Array) Key array(6 bytes)


  0x09, 0x05,                         //     Usage (Vendor Defined)
  0x15, 0x00,                         //     Logical Minimum (0)
  0x26, 0xFF, 0x00,                   //     Logical Maximum (255)
  0x75, 0x08,                         //     Report Count (2)
  0x95, 0x02,                         //     Report Size (8 bit)
  0xB1, 0x02,                         //     Feature (Data, Variable, Absolute)

  0xC0                                // End Collection (Application)
};
const unsigned char KeyboardReportNone[8] = {0,0,0,0,0,0,0,0};
const unsigned char KeyboardReport[8] = {
  0,  //modifier
  0,  //reserved
  4,  //Key1
  0,  //Key2
  0,  //Key3
  0,  //Key4
  0,  //Key6
  0   //Key7
};
const unsigned char HIDInfo[4]={
  0x11,
  0x01,
  0x00,
  0x03
};
BLEPeripheral blePeripheral;  // BLE Peripheral Device (the board you're programming)

BLEService mouse101("1812"); // 0x1812 = Human Interface Device

//BLEUnsignedCharCharacteristic BootMouseInputReport("2A33", BLERead | BLEWrite | BLENotify);//Write Optional

BLECharacteristic HIDInformation("2A4A", BLERead,4);
BLEUnsignedCharCharacteristic HIDControlPoint("2A4C", BLEWriteWithoutResponse);
BLEUnsignedCharCharacteristic ProtocolMode("2A4E", BLERead | BLEWriteWithoutResponse);
BLECharacteristic ReportMap("2A4B", BLERead,76);
// Here need report HID descriptor!
BLECharacteristic Report("2A4D", BLERead | BLENotify,8);
BLEUnsignedCharCharacteristic ReportOutput("2A4D", BLERead | BLENotify | BLEWrite | BLEWriteWithoutResponse);

BLECharacteristic BootKeyboardInputReport("2A22", BLERead | BLENotify,8);//Write Optional
BLEUnsignedCharCharacteristic BootKeyboardOutputReport("2A32", BLERead | BLEWrite | BLENotify);


const int ledPin = LED_BUILTIN; // pin to use for the LED
unsigned long previousMillis = 0;
const long interval = 500;
bool Keydown=false;
void setup() {
  Serial.begin(9600);

  // set LED pin to output mode
  pinMode(ledPin, OUTPUT);

  // set advertised local name and service UUID:
  blePeripheral.setLocalName("Genuino101 as keyboard");
  blePeripheral.setAdvertisedServiceUuid(mouse101.uuid());

  // add service and characteristic:
  blePeripheral.addAttribute(mouse101);
  blePeripheral.addAttribute(Report);
  blePeripheral.addAttribute(HIDInformation);
  blePeripheral.addAttribute(ProtocolMode);
  blePeripheral.addAttribute(HIDControlPoint);
  blePeripheral.addAttribute(ReportMap);
  blePeripheral.addAttribute(ReportOutput);

  //blePeripheral.addAttribute(BootMouseInputReport);
  blePeripheral.addAttribute(BootKeyboardInputReport);
  blePeripheral.addAttribute(BootKeyboardOutputReport);



  // set the initial value for the characeristic:
  ProtocolMode.setValue(1);
  //Report.setValue(0);
  ReportOutput.setValue(0);
  ReportMap.setValue(KeyboardReportDescriptor,76);
  BootKeyboardInputReport.setValue(KeyboardReportNone,8);
  BootKeyboardOutputReport.setValue(0);
//.  BootMouseInputReport.setValue(0);
  HIDInformation.setValue(HIDInfo,4);
  //HIDControlPoint.setValue(0);

  // begin advertising BLE service:
  blePeripheral.begin();

  Serial.println("BLE LED Peripheral");
}

void loop() {
  // listen for BLE peripherals to connect:
  BLECentral central = blePeripheral.central();

  // if a central is connected to peripheral:
  if (central) {
    Serial.print("Connected to central: ");
    // print the central's MAC address:
    Serial.println(central.address());

    // while the central is still connected to peripheral:
    while (central.connected()) {
      //delay(1000);
      //Serial.println("Send Key:a");
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        // save the last time you blinked the LED
        previousMillis = currentMillis;
        if(Keydown==true){
          Serial.println("Send Key none");
          Report.setValue(KeyboardReportNone,8);
          Keydown=false;
        }else{
          Serial.println("Send Key:a");
          Report.setValue(KeyboardReport,8);
          Keydown=true;
        }

      }
    }

    // when the central disconnects, print it out:
    Serial.print(F("Disconnected from central: "));
    Serial.println(central.address());
  }
}
