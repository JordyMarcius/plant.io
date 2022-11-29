#include "DHT.h" //library untuk sensor DHT11
#include "ThingerESP32.h" //library untuk connect ke Thiner.io

//mendefinisikan pin yang digunakan
#define SOIL_PIN A0
#define DHTPIN 4
#define DHTTYPE DHT11

//mendefinisikan devices username, id, dan credential dari thinger.io
#define USERNAME "JordyMarcius"
#define DEVICE_ID "esp32_jordy"
#define DEVICE_CREDENTIAL "XCMSW8XHP4xMO$4e"

//mendefinisikan nama wifi dan password wifi
#define WIFI_SSID "Playmedia"
#define WIFI_PASSWORD "1234567890"

ThingerESP32 thing(USERNAME, DEVICE_ID, DEVICE_CREDENTIAL); //memanggil fungsi untuk terhubung dengan thinger.io

//deklarasi variabel
const int value_basah = 1600;
const int value_kering = 3650;

const byte ledpin1 = 27;
const byte ledpin2 = 14;
const byte ledpin3 = 12;
const byte ledpin4 = 13;

int soil_percent;
float kelembaban;
float suhu;

DHT dht(DHTPIN, DHTTYPE); //memanggil fungsi untuk menjalankan sensor DHT11

void setup() 
{
  Serial.begin(115200); //memulai komunikasi serial dengan baudrate 115200

  xTaskCreatePinnedToCore
  (
    baca_sensor_soil_moisture,    //memanggil fungsi baca_sensor_soil_moisture()
    "baca_sensor_soil_moisture",  //nama task
    5000,                         //stack size
    NULL,                         //task parameter (NULL karena tidak ada parameter yang dipassing pada function)
    1,                            //priority
    NULL,                         //task handle
    0                             //core 0
  );

  xTaskCreatePinnedToCore
  (
    baca_sensor_dht11,            //memanggil fungsi baca_sensor_dht11()
    "baca_sensor_dht11",          //nama task
    5000,                         //stack size
    NULL,                         //task parameter (NULL karena tidak ada parameter yang dipassing pada function)
    1,                            //priority
    NULL,                         //task handle
    1                             //core 1
  );
  
  thing.add_wifi(WIFI_SSID, WIFI_PASSWORD);                   //menghubungkan wifi dengan thinger.io
  thing["Kelembaban_Tanah"] >> outputValue(soil_percent);     //kirim data kelembaban tanah ke thinger.io dengan topik "Kelembaban_Tanah"
  thing["Kelembaban_Udara"] >> outputValue(kelembaban);       //kirim data kelembaban udara ke thinger.io dengan topik "Kelembaban_Udara"
  thing["Suhu"] >> outputValue(suhu);                         //kirim data suhu ke thinger.io dengan topik "Suhu"
  
  dht.begin(); //memulai sensor dht11

  ledcAttachPin(ledpin1, 0); //ledpin1 menggunakan channel pwm 0
  ledcAttachPin(ledpin2, 1); //ledpin2 menggunakan channel pwm 1
  ledcAttachPin(ledpin3, 2); //ledpin3 menggunakan channel pwm 2
  ledcAttachPin(ledpin4, 3); //ledpin4 menggunakan channel pwm 3

  ledcSetup(0, 12000, 8); //set ledpin1 pada channel 0 dengan frekuensi 12000Hz dan resolusi 8 (berarti value analog kisaran 0 - 255)
  ledcSetup(1, 12000, 8); //set ledpin2 pada channel 1 dengan frekuensi 12000Hz dan resolusi 8 (berarti value analog kisaran 0 - 255)
  ledcSetup(2, 12000, 8); //set ledpin3 pada channel 2 dengan frekuensi 12000Hz dan resolusi 8 (berarti value analog kisaran 0 - 255)
  ledcSetup(3, 12000, 8); //set ledpin4 pada channel 3 dengan frekuensi 12000Hz dan resolusi 8 (berarti value analog kisaran 0 - 255)

  led_bar_awal(); //pola nyala led yang menandakan alat baru dinyalakan
}

void loop() 
{
  thing.handle(); //mengotomasi pengiriman data ke thinger.io
  
  if(soil_percent >= 0 && soil_percent < 25) //jika kelembaban tanah berkisar 0 - 24 persen maka yang menyala hanya led 1
  {
    //mengkonversi range kelembaban 0 - 24 persen menjadi nyala terang LED 1 dari value 0 - 120 
    //disini saya tidak sampai value 255 dikarenakan cahaya led dari value 120 - 255 tidak terlalu bisa dibedakan dengan kasat mata
    int nyala = map(soil_percent, 0, 24, 0, 120);  
    ledcWrite(0, nyala);
    ledcWrite(1, 0);
    ledcWrite(2, 0);
    ledcWrite(3, 0);
  }
  else if(soil_percent >= 25 && soil_percent < 50) //jika kelembaban tanah berkisar 25 - 49 persen maka yang menyala hanya led 1, dan 2
  {
    //mengkonversi range kelembaban 25 - 49 persen menjadi nyala terang LED 2 dari value 0 - 120 
    //disini saya tidak sampai value 255 dikarenakan cahaya led dari value 120 - 255 tidak terlalu bisa dibedakan dengan kasat mata
    int nyala = map(soil_percent, 25, 49, 0, 120);
    ledcWrite(0, 255);
    ledcWrite(1, nyala);
    ledcWrite(2, 0);
    ledcWrite(3, 0);
  }
  else if(soil_percent >= 50 && soil_percent < 75) //jika kelembaban tanah berkisar 50 - 74 persen maka yang menyala hanya led 1, 2, dan 3
  {
    //mengkonversi range kelembaban 50 - 74 persen menjadi nyala terang LED 3 dari value 0 - 120 
    //disini saya tidak sampai value 255 dikarenakan cahaya led dari value 120 - 255 tidak terlalu bisa dibedakan dengan kasat mata
    int nyala = map(soil_percent, 50, 74, 0, 120);
    ledcWrite(0, 255);
    ledcWrite(1, 255);
    ledcWrite(2, nyala);
    ledcWrite(3, 0);
  }
  else if(soil_percent >= 75) //jika kelembaban tanah berkisar 75 - 100 persen maka yang menyala led 1, 2, 3, dan 4
  {
    //mengkonversi range kelembaban 75 - 100 persen menjadi nyala terang LED 4 dari value 0 - 120 
    //disini saya tidak sampai value 255 dikarenakan cahaya led dari value 120 - 255 tidak terlalu bisa dibedakan dengan kasat mata
    int nyala = map(soil_percent, 75, 100, 0, 120); 
    ledcWrite(0, 255);
    ledcWrite(1, 255);
    ledcWrite(2, 255);
    ledcWrite(3, nyala);
  }

  //print pembacaan sensor di serial monitor
  Serial.print(soil_percent);
  Serial.print(" %   ");
  Serial.print("Kelembaban: ");
  Serial.print(kelembaban);
  Serial.print("   Suhu: ");
  Serial.println(suhu);
  delay(1000); //delay 1 sekon
}

void baca_sensor_soil_moisture(void * parameter)
{
  for(;;)                                                                   //for loop infinit
  {
    int soil = analogRead(SOIL_PIN);                                        //membaca value dari soil moisture sensor
    soil_percent = map(soil, value_kering, value_basah, 0, 100);            //mengkonversi value pembacaan menjadi value 0 - 100 persen
    vTaskDelay(1000 / portTICK_PERIOD_MS);                                  //delay 1 detik untuk for loop berikutnya
  }  
}

void baca_sensor_dht11(void * parameter)
{
  for(;;)                                                                   //for loop infinit
  {
    kelembaban = dht.readHumidity();                                        //membaca kelembaban udara dari sensor dht11
    suhu = dht.readTemperature();                                           //membaca suhu dari sensor dht11
    vTaskDelay(1000 / portTICK_PERIOD_MS);                                  //delay 1 detik untuk for loop berikutnya
  }
}

void led_bar_awal() //fungsi untuk menampilkan nyala led secara berurutan sebagai indikator awal alat dinyalakan
{
  int kecerahan;
  int counter;
  
  for(counter = 0; counter < 3; counter++)
  {
    ledcWrite(0, 0);
    ledcWrite(1, 0);
    ledcWrite(2, 0);
    ledcWrite(3, 0);
    
    for(kecerahan = 0; kecerahan < 100; kecerahan++)
    {
      if(kecerahan >= 0 && kecerahan < 25)
      {
        int nyala = map(kecerahan, 0, 24, 0, 120);
        ledcWrite(0, nyala);
      }
      else if(kecerahan >= 25 && kecerahan < 50)
      {
        int nyala = map(kecerahan, 25, 49, 0, 120);
        ledcWrite(0, 255);
        ledcWrite(1, nyala);
      }
      else if(kecerahan >= 50 && kecerahan < 75)
      {
        int nyala = map(kecerahan, 50, 74, 0, 120);
        ledcWrite(0, 255);
        ledcWrite(1, 255);
        ledcWrite(2, nyala);
      }
      else if(kecerahan >= 75)
      {
        int nyala = map(kecerahan, 75, 100, 0, 120);
        ledcWrite(0, 255);
        ledcWrite(1, 255);
        ledcWrite(2, 255);
        ledcWrite(3, nyala);
      }
      delay(20);
    }
  }

  ledcWrite(0, 0);
  ledcWrite(1, 0);
  ledcWrite(2, 0);
  ledcWrite(3, 0);
}
