#include <MQUnifiedsensor.h>

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h> // Librería para el sensor DHT11
#include <ESP32Servo.h>
#include <ESP32Tone.h>
#include <LiquidCrystal_I2C.h>
//#include <MQ135.h>
//#include <MQ3.h>
#include <Adafruit_Sensor.h>


#define GAS_SENSOR_PIN 32 // Pin de lectura del sensor de gas AO
#define TYPE_SENSOR "MQ-3" // Tipo de sensor MQ-3
#define PIR_PIN 27        // Pin del sensor PIR
#define SENSOR_PIN 35     // Pin conectado al sensor de llama
#define DHTPIN 15          // Pin en el que está conectado el sensor DHT
#define DHTTYPE DHT11     // Tipo de sensor (DHT11 o DHT22)
#define SERVO_PIN 33 
#define BUZZER_PIN 18


const char* ssid = "HUAWEI Y9s";
const char* password = "f7c0af5ddc73";
const char* mqtt_server = "192.168.43.65";//cambiar por la ip del rasberry
//const char* mqtt_server = "broker.emqx.io";
const char* mqtt_topic = "iot/proyecto/temperatura";
const char* topicServo = "iot/proyecto/servo";
const char* topicbuzzer = "iot/proyecto/buzzer";

int ledPin = 2; //Connection state
int ledPin2 = 4; //Show received messages
 

WiFiClient wifiClient;
PubSubClient client(wifiClient);
//Servo configuration
//const int servoPin = 14;
//Servo myservo;
//MQ135 gasSensor(GAS_SENSOR_PIN);
//MQUnifiedsensor MQUnifiedsensor(GAS_SENSOR_PIN, TYPE_SENSOR);
MQUnifiedsensor MQUnifiedsensor("ESP32", "MQ-3");

DHT dht(DHTPIN, DHTTYPE);
Servo servoMotor; // Crear objeto del servomotor
LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0X27, 16, 2); // Dirección de la pantalla LCD, número de columnas y filas

void setup() {
  Serial.begin(115200);
  dht.begin(); // Inicializar el sensor DHT11
  //MQUnifiedsensor.begin(); // Iniciar el sensor
  MQUnifiedsensor.init();

  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  //buzzer
  pinMode(BUZZER_PIN, OUTPUT); // Configuramos el pin del buzzer como salida
  //servo
  pinMode(PIR_PIN, INPUT);
  pinMode(SENSOR_PIN, INPUT);
  servoMotor.attach(SERVO_PIN); // Adjuntar el pin del servomotor
  // Inicializar la pantalla LCD
  lcd.init();
  // Encender la luz de fondo
  lcd.backlight();
  // Limpiar la pantalla
  lcd.clear();

  // Inicializar el sensor de temperatura y humedad
  //dht.begin();
  // Conectarse a la red WiFi
  WiFi.begin(ssid,password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a la red WiFi...");
  }
  Serial.println("Conexión WiFi establecida.");

  // Conectarse al servidor MQTT
  client.setServer(mqtt_server, 1883);
  while (!client.connected()) {
    if (client.connect("ioTClient")) {
      Serial.println("Conexión MQTT establecida.");
      client.setCallback(callback);
    } else {
      Serial.print("Error de conexión MQTT, estado: ");
      Serial.println(client.state());
      delay(2000);
    }
  }

  // Suscribirse al topic del servo
  client.subscribe(topicServo);
}

void loop() {
  client.loop();

  // Leer valor del sensor de gas
  //float gasValue = gasSensor.getPPM();
  float gasValue = MQUnifiedsensor.readSensor();

  // Leer valor del sensor PIR
  int pirValue = digitalRead(PIR_PIN);

  // Leer valor del sensor de llama
  int flameValue = digitalRead(SENSOR_PIN);

  // Leer valores de humedad y temperatura
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Imprimir los resultados en el monitor serie
  Serial.print("Valor de gas: ");
  Serial.print(gasValue);
  Serial.println(" ppm");

  Serial.print("Valor del sensor PIR: ");
  Serial.println(pirValue);

  Serial.print("Valor del sensor de llama: ");
  Serial.println(flameValue);

  // Leer la temperatura y la humedad del sensor
  float tempC = dht.readTemperature(); // Temperatura en grados Celsius
  float hum = dht.readHumidity(); // Humedad relativa en porcentaje

  // Mostrar los valores en la pantalla LCD
  lcd.setCursor(0, 0);
  lcd.print("Temp: ");
  lcd.print(tempC);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Hum: ");
  lcd.print(hum);
  lcd.print(" %");

  delay(2000); // Esperar 2 segundos antes de volver a leer los valores del sensor

  Serial.print("Humedad: ");
  Serial.print(h);
  Serial.print(" %\t");

  Serial.print("Temperatura: ");
  Serial.print(t);
  Serial.println(" *C");

  // Controlar el servomotor si se detecta algún sensor activado
  if (pirValue == HIGH/gasValue > 0 || pirValue == HIGH || flameValue == HIGH/) {
    //moveServo(); // Llamar a la función del servomotor
    servoMotor.write(0);     // sets the servo position to 0 degrees
    delay(1000);          // waits for 1 second
    servoMotor.write(90);   // sets the servo position to 180 degrees
    delay(4000);          // waits for 1 second
    servoMotor.write(0);     // sets the servo position to 0 degrees
    delay(1000);
  }

  //client.publish(mqtt_topic, msg);
  char msg[64];
  snprintf(msg, 64, "%.2f,%d,%d,%.2f,%.2f", gasValue, pirValue, flameValue, t, h);

  // Publicar el mensaje MQTT
  if (client.connected()) {
    client.publish(mqtt_topic, msg);
  }

  // Esperar 10 segundos antes de publicar la siguiente temperatura y humedad
  delay(1000);

}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Mensaje recibido en el topic ");
  Serial.print(topic);

  String messageTemp;
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println(messageTemp);

  if (String(topic) == topicServo) {
    if (messageTemp == "true") {
      Serial.println("Encendiendo LED");
       tone(BUZZER_PIN, 1000); 
      //digitalWrite(ledPin2, HIGH);
      servoMotor.write(90); 
      // client.publish(topicServo, "true");
    } else if (messageTemp == "false") {
      Serial.println("Apagando LED");
      noTone(BUZZER_PIN); // Detenemos el sonido
      //digitalWrite(ledPin2, LOW);
      servoMotor.write(0); 

      // client.publish(topicServo, "false");
    }
  } else if (String(topic) == topicbuzzer) {
    if (messageTemp == "true") {
      Serial.println("Encendiendo el buzzer");
      tone(BUZZER_PIN, 1000); // Hacemos sonar el buzzer a una frecuencia de 1000 Hz
      // client.publish(topicServo, "true");
    } else if (messageTemp == "false") {
      Serial.println("Apagando buzzer");
      noTone(BUZZER_PIN); // Detenemos el sonido
      // client.publish(topicServo, "false");
    }
  }
}