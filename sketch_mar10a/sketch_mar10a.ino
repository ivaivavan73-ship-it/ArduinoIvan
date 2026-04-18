#include <Servo.h>

Servo myServo;
const int trigPin = 11;
const int echoPin = 12;
const int servoPin = 9;

int count = 0;
const int triggerCount = 5;
const float detectionDistance = 20.0;  // см
const unsigned long debounceTime = 500;  // мс — защита от дребезга
unsigned long lastDetection = 0;
int currentPosition = 0;  // текущее положение серво (0° или 180°)

void setup() {
  Serial.begin(9600);
  myServo.attach(servoPin);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  myServo.write(currentPosition);  // устанавливаем начальное положение
  Serial.println("Система готова. Ожидаю пронос руки...");
}

float getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);  // таймаут 30 мс
  if (duration == 0) return -1;  // ошибка измерения
  float distance = duration * 0.034 / 2;
  return distance;
}

bool isObjectDetected() {
  float dist1 = getDistance();
  delay(10);
  float dist2 = getDistance();
  float dist3 = getDistance();

  // Медиана из трёх измерений для устойчивости
  float distances[3] = {dist1, dist2, dist3};
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2 - i; j++) {
      if (distances[j] > distances[j + 1]) {
        float temp = distances[j];
        distances[j] = distances[j + 1];
        distances[j + 1] = temp;
      }
    }
  }
  float median = distances[1];
  return (median > 0 && median <= detectionDistance);
}

void toggleServo() {
  if (currentPosition == 0) {
    currentPosition = 180;
    Serial.println("Поворот на 180°");
  } else {
    currentPosition = 0;
    Serial.println("Возврат на 0°");
  }
  myServo.write(currentPosition);
  delay(500);  // небольшая задержка для завершения поворота
}

void loop() {
  unsigned long currentTime = millis();

  if (isObjectDetected()) {
    if (currentTime - lastDetection >= debounceTime) {  // защита от дребезга
      count++;
      lastDetection = currentTime;
      Serial.print("Обнаружен пронос руки №");
      Serial.println(count);

      // Ждём, пока объект уйдёт, чтобы избежать повторного счёта
      while (isObjectDetected()) {
        delay(50);
      }

      if (count >= triggerCount) {
        Serial.println( "!");
        toggleServo();  // поворачиваем на 180° в другую сторону
        count = 0;
        Serial.println("!!!!!");
      }
    }
  }
  delay(50);  // стабильность работы
}









  
  
