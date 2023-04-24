# ProjectsIoT2022

## Integrantes
- Maria Iraida García Martinez
- Winy Daniela Manzano Rangel
- Alyne Elizabeth Rojas Gloria
- Andrea Trujillo Azpeitia

## Objetivo general
Brindar mayor seguridad y libertad a nuestras mascotas.

### Objetivos específicos
- Cuidar a nuestra mascota para prevenir accidentes.
- Tener un control en el monitoreo de nuestra mascota.
- Poder conocer aspectos de nuestro entorno para tomar medidas preventivas.
- Recibir información del entorno de nuestra mascota al momento de querer ingresar al hogar.

- Elemento n de la lista de objetivos específicos

## Tabla de Software utilizado
| Id | Software | Version | Tipo |
|----|----------|---------|------|
| 1   |Visual Studio Code |17.2  |    IDE  |
|  2  |Arduino  |    2.0.0|     IDE |
|    |          |         |      |

## Tabla con el hardware utilizado
| Id | Componente | Descripción | Imagen | Cantidad | Costo total |
|----|------------|-------------|--------|----------|-------------|
|   1|ESP32 |El módulo ESP32 es una solución de Wi-Fi/Bluetooth todo en uno, integrada y certificada que proporciona no solo la radio inalámbrica, sino también un procesador integrado con interfaces para conectarse con varios periféricos. |![image](https://user-images.githubusercontent.com/99991728/214651946-9046d596-00a0-4c35-bc72-5ef110854bde.png)|  1| $260.00 |
|2   |Sensor de temperatura y humedad |El sensor de humedad cuenta con dos microsensores calibrados en función de la humedad relativa del área o la zona. Al entrar en contacto con el aire, el sensor emite una señal a los sistemas de extracción higorregulables situados en la cocina o el baño.|![image](https://user-images.githubusercontent.com/99991728/214654110-86eceeaf-8c2e-4373-a043-2662dfbc657e.png)|   1  |     $65.00   |
| 3  |Sensor de gas|Los sensores de gas son dispositivos que indican la presencia de algún gas específico, en algunos casos pueden configurarse o, en caso de tener sensores más precisos, miden la concentración de gas. Los sensores de gas son usados para prevenir la exposición a gases combustibles y gases tóxicos|![image](https://user-images.githubusercontent.com/99991728/214653387-f3ffbd53-2dfe-42cd-b781-f28868f09d47.png)|  1 |   $95.00 |
|4|Sensor de llama|Un detector de llamas es un tipo de sensor que puede detectar y responder a la presencia de una llama. Estos detectores tienen la capacidad de identificar el líquido sin humo y el humo que puede crear un fuego abierto.|![image](https://user-images.githubusercontent.com/99991728/214637623-818241a0-ad1c-4dc6-bae7-da990a0a3aed.png)| 1|  $435.00|
|5|Sensor de proximidad|Un sensor de proximidad es un transductor que detecta objetos o señales que se encuentran cerca del elemento sensor.|![image](https://user-images.githubusercontent.com/99991728/214636322-31502609-1fc6-4d82-969d-077653952f92.png)| 1|$67.00|
|6|Raspberry pi|Raspberry Pi es un ordenador low cost capaz de realizar las mismas funciones que un ordenador estándar de sobremesa, desarrollado con el objetivo de hacer la informática accesible y asequible para todos.|![image](https://user-images.githubusercontent.com/99991728/214634366-b8ea0712-5d0e-4e19-b2a7-6623a8678f99.png)|1|$2139.00|
|7|Cámara|El sensor de imagen OV7670 cuenta con una cámara VGA de un solo chip y procesador de imágenes para todas las funciones|![image](https://user-images.githubusercontent.com/99991728/214635474-d0ac294e-343b-4e2a-b769-fac96cd34303.png)|1|$135.00|



## Epicas del proyecto (Minimo debe de haber una épica por integrante de equipo)
- Quiero poder saber si mi mascota se encuentra en peligro de intoxicación o si el ambiente que lo rodea hay peligro de una fuga de gas
- Quiero que mi mascota pueda salir cuando ella lo requiera.
- Quiero conocer si mi mascota corre el riesgo de sufrir algun daño por quemadura mientras no me encuentro con el.
- Quiero salir con la seguridad de que mi mascota podrá salir en caso de incendio o intoxicación

## Tabla de historias de usuario
| Id | Historia de usuario | Prioridad | Estimación | Como probarlo | Responsable |
|----|---------------------|-----------|------------|---------------|-------------|
|1|Quiero saber cuando mi mascota tenga la necesidad de salir de casa, teninedo yo la seguirdad de que va a estar bien|  Alta | 3 semanas |Aumentando la temperatura y humedad.  |Alyne Elizabeth Rojas Gloria |
|2|  Quiero que si mi mascota se siente en peligro pueda salir |Alta|            2 semanas |Usando un gas de manera controlada cerca del sensor| Winy Daniela Manzano Rangel|
|3|Quiero conocer si mi mascota corre el riesgo de sufrir algún daño por quemadura mientras no me encuentro con él.| Alta|3 semanas|Exponiendo el proyecto a una llama controlada|Maria Iraida García Martínez|
|4|Quiero que mi mascota tenga mayor libertad y bienestar| Media | 3 semanas| Poner en uso l prototipo| Andrea Trujillo Azpeitia|

## Código C++
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
##Placa fenólica
![image](https://user-images.githubusercontent.com/99991728/233894041-362eb541-34c3-4de3-b407-ca5fadd19961.png)


##Node-Red
![image](https://user-images.githubusercontent.com/99991728/233893897-28f83c07-acf8-4610-a1a5-5a3576e2a750.png)


## Prototipo en dibujo
- 
![image](https://user-images.githubusercontent.com/99991728/214682804-0033cb39-7e6b-4ac7-a4b5-eeea648c5052.png)

##Prototipo fisico

![image](https://user-images.githubusercontent.com/99991728/233893926-dc1dee0b-1658-4e43-bd6c-32b24e63f635.png)

