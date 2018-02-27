

/**
 * Automação de Horta usando ESP32
 * Autor: João Campos
 * Sensores Utilizados
 * 1 sensor de chuva
 * 1 DHT11
 * 1 Solenoide
 * 12 sensores de humidade do solo -- desabilitado
 *
 * Tutoriais Utilizados
 * http://randomnerdtutorials.com/esp32-dht11-dht22-temperature-humidity-web-server-arduino-ide/
 * https://github.com/espressif/esp-idf/issues/199
 * https://github.com/adafruit/DHT-sensor-library/issues/62
 * https://www.arduinoecia.com.br/2014/06/sensor-de-chuva-arduino.html
 */
#include <Modbus.h>
#include <ModbusIP_ESP32.h>
#include <sys/time.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>


void floatToWordArray(float , word * );

uint32_t get_usec();


// Sensor usado é o DHT11
#define DHTTYPE DHT11


// DHT Sensor
const int DHTPin = GPIO_NUM_25;
const int porta_sensor_chuva = GPIO_NUM_36;
const int porta_humidade[] = {GPIO_NUM_39, GPIO_NUM_34, GPIO_NUM_35, GPIO_NUM_32};
const int porta_solenoide = GPIO_NUM_19;



// Inicializar DHT sensor.
DHT dht(DHTPin, DHTTYPE);

//ModbusIP object
ModbusIP mb;



//Declaração de INPUT registers
const int SENSOR_CHUVA = 0;
const int SENSOR_HUMIDADE_DHT_0 = 1;
const int SENSOR_HUMIDADE_DHT_1 = 2;
const int SENSOR_TEMPERATURA_DHT_0 = 3;
const int SENSOR_TEMPERATURA_DHT_1 = 4;
const int SENSOR_HUMIDADE[] = {5, 6, 7, 8};
//Declaração de COIL registers
const int COIL_0 = 0;


// Declarando variáveis
// Porta do sensor de humidade de solo 1

//int porta_humidade_solo_2 = A1;
//int porta_humidade_solo_3 = A2;

hw_timer_t * timer = NULL;
volatile SemaphoreHandle_t timerSemaphore;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;


void IRAM_ATTR onTimer(){
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&timerMux);
  portEXIT_CRITICAL_ISR(&timerMux);

  // Give a semaphore that we can check in the loop
  xSemaphoreGiveFromISR(timerSemaphore, NULL);

  // It is safe to use digitalRead/Write here if you want to toggle an output
}


void setup() {
  Serial.begin(115200);
    delay(10);
 // Criar semáforo para checar no loop
  timerSemaphore = xSemaphoreCreateBinary();
  //Config Modbus IP
    mb.config("MS NET", "internet2018");


     while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    // Declarar tipos de portas
  pinMode(porta_sensor_chuva, INPUT);
  pinMode(porta_solenoide, OUTPUT);
  pinMode(GPIO_NUM_18, OUTPUT);



    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  // Use 1st timer of 4 (counted from zero).
  // Set 80 divider for prescaler (see ESP32 Technical Reference Manual for more
  // info).
  timer = timerBegin(0, 80, true);

  // Attach onTimer function to our timer.
  timerAttachInterrupt(timer, &onTimer, true);

  // Colocar alarme para a função onTimer ser chamada a cada 10 s (valor em microsegundos).
  // Repetir alarme (=true)
  timerAlarmWrite(timer, 10000000, true);

  // Adicionar input registers
  mb.addIreg(SENSOR_CHUVA);
  mb.addIreg(SENSOR_HUMIDADE_DHT_0);
  mb.addIreg(SENSOR_HUMIDADE_DHT_1);
  mb.addIreg(SENSOR_TEMPERATURA_DHT_0);
  mb.addIreg(SENSOR_TEMPERATURA_DHT_1);

// Atuadores a relé
   mb.addCoil(COIL_0);


  // Iniciar alarme
  timerAlarmEnable(timer);

}

void loop() {
  //Call once inside loop() - all magic here
   mb.task();
   digitalWrite(porta_solenoide, mb.Coil(COIL_0));


  // semáforo foi disparado pelo Hardware timer
  if (xSemaphoreTake(timerSemaphore, 0) == pdTRUE){
    
      float h = dht.readHumidity();
      word registro_humidade[2];
     

      floatToWordArray(h, registro_humidade);
      mb.Ireg(SENSOR_HUMIDADE_DHT_0, registro_humidade[1]);    
      mb.Ireg(SENSOR_HUMIDADE_DHT_1, registro_humidade[0]);
     
      // Read temperature as Celsius (the default)
      float t = dht.readTemperature();
    
      word registro_temperatura[2];
      floatToWordArray(t, registro_temperatura);

      mb.Ireg(SENSOR_TEMPERATURA_DHT_0, registro_temperatura[1]);
      mb.Ireg(SENSOR_TEMPERATURA_DHT_1, registro_temperatura[0]);
      word registro[2];

      // Leitura do sensor de chuva
    //int sensor_chuva = analogRead(porta_sensor_chuva);

      //floatToWordArray(pt100_0, registro);

      //mb.Ireg(SENSOR_CHUVA, sensor_chuva);



  }
 delay(100);

  // put your main code here, to run repeatedly:

}


void floatToWordArray(float number, word *reg){
 byte*  ArrayOfFourBytes;
 ArrayOfFourBytes = (byte*) &number;
 reg[0] = (ArrayOfFourBytes[1]<<8)| ArrayOfFourBytes[0];
 reg[1] = (ArrayOfFourBytes[3]<<8)| ArrayOfFourBytes[2];
}



// Similar to uint32_t system_get_time(void)
uint32_t get_usec() {

 struct timeval tv;

 //              struct timeval {
 //                time_t      tv_sec;     // seconds
 //                suseconds_t tv_usec;    // microseconds
 //              };

 gettimeofday(&tv,NULL);
 return (tv.tv_sec*1000000 + tv.tv_usec);

}
