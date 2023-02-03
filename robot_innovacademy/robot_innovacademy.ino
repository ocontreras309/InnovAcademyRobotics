#include <Servo.h>

Servo servo;

int TRIG_PIN = 12;
int ECHO_PIN = 13;

int PWM_VELOCIDAD_IZQUIERDA = 11;
int PWM_VELOCIDAD_DERECHA = 3;
int IZQ_DIR1 = 9;
int IZQ_DIR2 = 10;
int DERECHA_DIR1 = 4;
int DERECHA_DIR2 = 5;
int IZQ_SL = 6;
int CENTRO_SL = 7;
int DERECHA_SL = 8;
int VELOCIDAD = 255;
char modo;

const int MAX_DISTANCIA = 10000;

const char DOBLAR_IZQUIERDA = 1;
const char DOBLAR_DERECHA = 2;
const char ADELANTE = 3;
const char ATRAS = 4;
const char PARAR = 5;
const char FUNC_SIGUELINEAS = 6;
const char FUNC_EVASOR_OBSTACULOS = 7;
const char FUNC_CONTROL_BLUETOOTH = 8;


void setup() {
  pinMode(PWM_VELOCIDAD_IZQUIERDA, OUTPUT);
  pinMode(PWM_VELOCIDAD_DERECHA, OUTPUT);
  pinMode(IZQ_DIR1, OUTPUT);
  pinMode(IZQ_DIR2, OUTPUT);
  pinMode(DERECHA_DIR1, OUTPUT);
  pinMode(DERECHA_DIR2, OUTPUT);
  pinMode(IZQ_SL, INPUT);
  pinMode(CENTRO_SL, INPUT);
  pinMode(DERECHA_SL, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  servo.attach(2);

  Serial.begin(9600);
}

void doblarIzquierda(int velocidad) {
  analogWrite(PWM_VELOCIDAD_IZQUIERDA, velocidad);
  analogWrite(PWM_VELOCIDAD_DERECHA, velocidad);

  digitalWrite(IZQ_DIR1, LOW);
  digitalWrite(IZQ_DIR2, LOW);
  digitalWrite(DERECHA_DIR1, LOW);
  digitalWrite(DERECHA_DIR2, HIGH);
}

void doblarDerecha(int velocidad) {
  analogWrite(PWM_VELOCIDAD_IZQUIERDA, velocidad);
  analogWrite(PWM_VELOCIDAD_DERECHA, velocidad);

  digitalWrite(IZQ_DIR1, LOW);
  digitalWrite(IZQ_DIR2, HIGH);
  digitalWrite(DERECHA_DIR1, LOW);
  digitalWrite(DERECHA_DIR2, LOW);
}

void avanzar(int velocidad) {
  analogWrite(PWM_VELOCIDAD_IZQUIERDA, velocidad);
  analogWrite(PWM_VELOCIDAD_DERECHA, velocidad);
  digitalWrite(IZQ_DIR1, LOW);
  digitalWrite(IZQ_DIR2, HIGH);
  digitalWrite(DERECHA_DIR1, LOW);
  digitalWrite(DERECHA_DIR2, HIGH);
}

void parar() {
  digitalWrite(IZQ_DIR1, LOW);
  digitalWrite(IZQ_DIR2, LOW);
  digitalWrite(DERECHA_DIR1, LOW);
  digitalWrite(DERECHA_DIR2, LOW);
}

void atras(int velocidad) {
  analogWrite(PWM_VELOCIDAD_IZQUIERDA, velocidad);
  analogWrite(PWM_VELOCIDAD_DERECHA, velocidad);
  digitalWrite(IZQ_DIR1, HIGH);
  digitalWrite(IZQ_DIR2, LOW);
  digitalWrite(DERECHA_DIR1, HIGH);
  digitalWrite(DERECHA_DIR2, LOW);
}

int calcularDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  int duracion = pulseIn(ECHO_PIN, HIGH);
  int distancia = duracion * 0.034 / 2;
  return distancia <= 0 ? MAX_DISTANCIA : distancia;
}

void obstaculos() {
  Serial.println(calcularDistancia());
  if (calcularDistancia() < 30) {
    
    doblarIzquierda(VELOCIDAD);
    delay(900);
  }

  avanzar(VELOCIDAD);
}

void sigueLineas() {

  int centro = digitalRead(CENTRO_SL);
  int izquierda = digitalRead(IZQ_SL);
  int derecha = digitalRead(DERECHA_SL);

  if (centro) {
    avanzar(VELOCIDAD);
  } else {
    if (izquierda) {
      doblarDerecha(VELOCIDAD);
    } else if (derecha) {
      doblarIzquierda(VELOCIDAD);
    }
  }
}

void controlBluetooth(char entrada) {
  switch (entrada) {
    case DOBLAR_IZQUIERDA:
      doblarIzquierda(VELOCIDAD);
      delay(300);
      avanzar(VELOCIDAD);
      break;
    case DOBLAR_DERECHA:
      doblarDerecha(VELOCIDAD);
      delay(300);
      avanzar(VELOCIDAD);
      break;
    case ADELANTE:
      avanzar(VELOCIDAD);
      break;
    case ATRAS:
      atras(VELOCIDAD);
      break;
    case PARAR:
      parar();
      break;
  }
}


void loop() {

  if (Serial.available() > 0) {
    char bluetoothValor = Serial.read();
   
    if (bluetoothValor == FUNC_SIGUELINEAS || bluetoothValor == FUNC_EVASOR_OBSTACULOS || bluetoothValor == FUNC_CONTROL_BLUETOOTH) {
      modo = bluetoothValor;
    }

    if (modo == FUNC_CONTROL_BLUETOOTH) {
      controlBluetooth(bluetoothValor);
    }
  } else {
    switch (modo) {
      case FUNC_SIGUELINEAS:
        sigueLineas();
        break;
      case FUNC_EVASOR_OBSTACULOS:
        obstaculos();
        break;
    }
  }

  delay(10);
}
