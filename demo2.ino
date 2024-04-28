#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <LiquidCrystal_I2C.h>    
#include <Adafruit_TSL2561_U.h>
#include <Adafruit_MPU6050.h>
#include <Wire.h>

Adafruit_BME280 bme;
LiquidCrystal_I2C lcd(0x27, 16, 2);
// Define variables to store BME280 readings
float temperature;
float humidity;
float pressure;

const int LED_COOLING_PIN = 19;
const int LED_LIGHTING_PIN = 18;
const int Switch_mode_button = 4;
const int light_mode_button = 2;
const int Increase_lvl_button = 16;
const int Decrease_lvl_button = 17;


float currentLightingLevel = 0;
float currentCoolingLevel = 0;

const int automatic= 0;
const int manual = 1;
const int gesture = 2;
int mode = 0;
bool light_mode = true;

float accel_y = 0;

Adafruit_MPU6050 mpu;
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);


void configureSensorTSL(void)
{
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
 
  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */
}


 void GestureMode(){
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    Serial.print("Acceleration: ");
    Serial.print(a.acceleration.x);
    Serial.print(", ");
    Serial.print(a.acceleration.y);
    Serial.print(", ");
    Serial.println(a.acceleration.z);

    Serial.print("Gyro: ");
    Serial.print(g.gyro.x);
    Serial.print(", ");
    Serial.print(g.gyro.y);
    Serial.print(", ");
    Serial.println(g.gyro.z);

    accel_y = a.acceleration.y;
    currentLightingLevel = map(accel_y, -10, 30, 0, 255);
    
    currentCoolingLevel = map(accel_y, -10, 30, 0, 255);

 
        analogWrite(LED_LIGHTING_PIN, currentLightingLevel);
 
        analogWrite(LED_COOLING_PIN, currentCoolingLevel);



 }

 void displayBME280() {
  lcd.setCursor(0,0);
  lcd.print("Temp: ");
  lcd.print(temperature);
  lcd.setCursor(6, 0);
}

void displayTSL2561() {
  sensors_event_t event;
  tsl.getEvent(&event);
  float lux = event.light;

  lcd.setCursor(0,1);
  lcd.print("Lux: ");
  lcd.setCursor(5,1);
  lcd.print(lux);
}

void automode(){


    sensors_event_t event;
    tsl.getEvent(&event);
    float lux = event.light;
    currentLightingLevel = map(lux, 0, 350, 0, 255);
    
    currentCoolingLevel = map(temperature, 25, 30, 0, 255);

     
        analogWrite(LED_LIGHTING_PIN, currentLightingLevel);

   
        analogWrite(LED_COOLING_PIN, currentCoolingLevel);

   

    

}


void manualMode(){
    if(digitalRead(Increase_lvl_button) == HIGH){ 
      currentCoolingLevel++;
      currentLightingLevel+= 10;
      analogWrite(LED_COOLING_PIN, currentCoolingLevel);
      analogWrite(LED_LIGHTING_PIN, currentLightingLevel);
    }
    if(digitalRead(Decrease_lvl_button)==HIGH){
      currentCoolingLevel--;
      currentLightingLevel-=10;
      

        analogWrite(LED_LIGHTING_PIN, currentLightingLevel);
    
        analogWrite(LED_COOLING_PIN, currentCoolingLevel);

    }
}

void displayAll(){

  displayBME280();
  displayTSL2561();
}




void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  while (!Serial)
    delay(10); // will pause Zero, Leonardo, etc until serial console opens

  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange()) {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange()) {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth()) {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);


  bool status = bme.begin(0x77);  // Init BME280 sensor, and print error message if it fails
  if (!status) {
  Serial.println("Could not find a valid BME280 sensor, check wiring!");
  while (1);
  }
  lcd.init(); // initialize the lcd
  lcd.backlight();




  configureSensorTSL();
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);




  pinMode(Switch_mode_button, INPUT);

  pinMode(LED_COOLING_PIN, OUTPUT);
  pinMode(LED_LIGHTING_PIN, OUTPUT);


  pinMode(Increase_lvl_button, INPUT);
  pinMode(Decrease_lvl_button, INPUT);
  pinMode(light_mode_button, INPUT);
 


}



void loop() {
  //get readings from BME280 sensor
  temperature = bme.readTemperature();
  humidity = bme.readHumidity();
  pressure = (bme.readPressure() / 100.0F);

  if(digitalRead(light_mode_button)==HIGH){
    delay(50);
    light_mode = !light_mode;
  }

  if (currentCoolingLevel > 255) {
      currentCoolingLevel = 255;
  } else if (currentCoolingLevel < 0) {
      currentCoolingLevel = 0;
  }

  if (currentLightingLevel > 255) {
      currentLightingLevel = 255;
  } else if (currentLightingLevel < 0) {
      currentLightingLevel = 0;
  }

  if(digitalRead(Switch_mode_button)==HIGH){
      delay(50);
      mode++;
        if (mode > gesture) {
            mode = automatic; // Reset to automatic if mode exceeds gesture
        }
 
  }

   switch (mode) {
      case automatic:
          automode();;
          break;
      case manual:
          manualMode();
          break;
      case gesture:
          GestureMode();
          break;

   }

   displayAll();
}
