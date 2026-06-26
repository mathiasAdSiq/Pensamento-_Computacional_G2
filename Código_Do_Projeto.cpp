#include <Servo.h>

Servo dedo;

const byte servoPin = 9;
const byte switchPin = 2;

// Ajuste estes valores
const int POS_DENTRO = 10;
const int POS_BOTAO  = 90;

bool jaRodou = false;

void setup() {
  pinMode(switchPin, INPUT_PULLUP);

  dedo.attach(servoPin);
  dedo.write(POS_DENTRO);

  randomSeed(analogRead(A0));
}

void loop() {
  bool ligado = digitalRead(switchPin) == LOW;

  if (ligado && jaRodou == false) {
    jaRodou = true;

    executarPersonalidade();

    mover(POS_BOTAO, 6);
    delay(200);
    mover(POS_DENTRO, 6);
  }

  // Se o botão estiver desligado, fica SEMPRE dentro da caixa
  if (!ligado) {
    jaRodou = false;

    if (dedo.read() != POS_DENTRO) {
      mover(POS_DENTRO, 60);
    }

    dedo.write(POS_DENTRO);
  }
}

void mover(int destino, int vel) {
  int atual = dedo.read();

  if (atual < destino) {
    for (int p = atual; p <= destino; p++) {
      dedo.write(p);
      delay(vel);
    }
  } else {
    for (int p = atual; p >= destino; p--) {
      dedo.write(p);
      delay(vel);
    }
  }
}

void executarPersonalidade() {
  switch (random(6)) {
    case 0:
      delay(300);
      break;

    case 1:
      mover(60, 15);
      delay(400);
      break;

    case 2:
      mover(45, 15);
      delay(300);
      mover(75, 15);
      delay(300);
      break;

    case 3:
      mover(35, 8);
      delay(200);
      break;

    case 4:
      mover(70, 25);
      delay(400);
      mover(50, 25);
      delay(300);
      break;

    case 5:
      delay(random(700, 1500));
      break;
  }
}
