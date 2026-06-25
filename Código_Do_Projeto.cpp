// Sweep - Useless Box com Personalidade
// Baseado no exemplo original de BARRAGAN <http://barraganstudio.com>
// Modificado para adicionar comportamento "hesitante", "irritado" e "brincalhão" ao braço
// Verifica o botão continuamente, interrompendo o movimento se ele for solto
// CORRIGIDO: ao soltar o botão no meio de qualquer etapa, o braço agora volta
// para DENTRO da caixa (90°), em vez de continuar em direção ao botão (20°)

#include <Servo.h>

const int  buttonPin = 2;
int buttonState = 0;
int lastButtonState = LOW; // guarda o estado anterior do botão, pra detectar nova pressão

Servo myservo;  // create servo object to control a servo
                // a maximum of eight servo objects can be created
int pos;                // variable to store the servo position
long timeDelay;

const int posMin = 20;   // posição mínima já existente no código original (dedo no botão)
const int posMax = 90;   // posição máxima já existente no código original (dedo dentro da caixa)
const int posMeio = (posMin + posMax) / 2; // ponto médio do percurso (55)

int contadorPressoes = 0;  // conta quantas vezes o botão foi pressionado
const int maxIrritacao = 5; // a partir de quantas pressões ela fica "no limite"

// Lê o botão já aplicando a inversão (fiação lê ao contrário)
int lerBotao() {
  return !digitalRead(buttonPin);
}

// Verifica o botão "fatiando" o delay em pedacinhos de 5ms.
// Retorna false (e para tudo) se o botão for solto durante a espera.
bool esperarChecando(int duracaoMs) {
  for (int t = 0; t < duracaoMs; t += 5) {
    if (lerBotao() == LOW) {
      return false; // botão foi solto, abortar
    }
    delay(5);
  }
  return lerBotao() == HIGH;
}

// Move o servo até "destino", checando o botão a cada passo.
// Retorna false (e para o movimento) se o botão for solto no meio do caminho.
bool moverParaChecando(int destino, int velocidadeBase, int nivelIrritacao) {
  if (myservo.read() < destino) {
    for (pos = myservo.read(); pos <= destino; pos += 1) {
      if (lerBotao() == LOW) return false; // abortar na hora
      myservo.write(pos);
      timeDelay = velocidadeBase - nivelIrritacao;
      if (timeDelay < 5) timeDelay = 5;
      delay(timeDelay);
    }
  } else {
    for (pos = myservo.read(); pos >= destino; pos -= 1) {
      if (lerBotao() == LOW) return false; // abortar na hora
      myservo.write(pos);
      timeDelay = velocidadeBase - nivelIrritacao;
      if (timeDelay < 5) timeDelay = 5;
      delay(timeDelay);
    }
  }
  return true;
}

void setup()
{
  pinMode(buttonPin, INPUT);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object
  Serial.begin(9600);
  randomSeed(analogRead(A0)); // melhora a aleatoriedade do random()
}

void loop()
{
  buttonState = lerBotao();  // Read the button position (já invertido)
  Serial.println(buttonState);

  // Detecta uma NOVA pressão (transição de LOW para HIGH)
  if (buttonState == HIGH && lastButtonState == LOW) {
    contadorPressoes++;
    if (contadorPressoes > maxIrritacao) {
      contadorPressoes = maxIrritacao;
    }
  }
  // Se o botão for solto, ela "esfria" um pouco a irritação
  if (buttonState == LOW && lastButtonState == HIGH) {
    if (contadorPressoes > 0) {
      contadorPressoes--;
    }
  }

  lastButtonState = buttonState;

  int nivelIrritacao = contadorPressoes;

  if (buttonState == HIGH) {

    // --- ETAPA 1: vai da posição atual até o meio do caminho ---
    if (!moverParaChecando(posMeio, 25, nivelIrritacao)) goto desligarDireto;

    // --- ETAPA 2: pausa "pensativa" no meio do caminho ---
    int pausaPensativa = random(400, 900) - (nivelIrritacao * 80);
    if (pausaPensativa < 100) pausaPensativa = 100;
    if (!esperarChecando(pausaPensativa)) goto desligarDireto;

    // --- ETAPA 3: tremor de indecisão ---
    {
      int amplitudeTremor = 3 + nivelIrritacao;
      int velocidadeTremor = 60 - (nivelIrritacao * 8);
      if (velocidadeTremor < 15) velocidadeTremor = 15;

      for (int i = 0; i < 2; i++) {
        if (lerBotao() == LOW) goto desligarDireto;
        myservo.write(posMeio + amplitudeTremor);
        if (!esperarChecando(velocidadeTremor)) goto desligarDireto;
        myservo.write(posMeio - amplitudeTremor);
        if (!esperarChecando(velocidadeTremor)) goto desligarDireto;
      }
      myservo.write(posMeio);
      if (!esperarChecando(200)) goto desligarDireto;
    }

    // --- ETAPA 4: sorteia qual "personalidade" ela vai ter nessa rodada ---
    {
      int comportamento = random(0, 100);

      if (comportamento < 25 && nivelIrritacao < maxIrritacao) {
        // --- abre e fecha repetidamente ---
        int vezes = random(3, 10); // de 3 a 9 vezes
        for (int i = 0; i < vezes; i++) {
          if (!moverParaChecando(posMin, 12, nivelIrritacao)) goto desligarDireto;
          if (!esperarChecando(random(80, 200))) goto desligarDireto;
          if (!moverParaChecando(posMeio, 12, nivelIrritacao)) goto desligarDireto;
          if (!esperarChecando(random(80, 200))) goto desligarDireto;
        }
      }
      else if (comportamento < 50 && nivelIrritacao < maxIrritacao) {
        // --- finge desligar e volta rápido ---
        if (!moverParaChecando(posMin, 10, nivelIrritacao)) goto desligarDireto;
        if (!esperarChecando(random(150, 350))) goto desligarDireto;
        if (!moverParaChecando(posMax, 5, 0)) goto desligarDireto;
        if (!esperarChecando(random(200, 400))) goto desligarDireto;
      }
    }

    // --- ETAPA 5: "tentativa de desistência" ---
    {
      int chanceDesistir = 70 - (nivelIrritacao * 15);
      if (chanceDesistir < 5) chanceDesistir = 5;

      if (random(0, 100) < chanceDesistir) {
        int posAvanco = posMeio - 10;
        if (posAvanco < posMin) posAvanco = posMin;

        if (!moverParaChecando(posAvanco, 20, nivelIrritacao)) goto desligarDireto;
        if (!esperarChecando(random(200, 500))) goto desligarDireto;
        if (!moverParaChecando(posMeio, 15, nivelIrritacao)) goto desligarDireto;
        if (!esperarChecando(random(150, 400))) goto desligarDireto;
      }
    }

    // --- ETAPA 6: finalmente vai até a posição final e desliga o botão ---
    moverParaChecando(posMin, 20, nivelIrritacao);
    return;

    // --- Rótulo usado quando o botão é solto no meio de qualquer etapa ---
    desligarDireto:
      // O usuário soltou o botão: o braço deve voltar para DENTRO da caixa (90°),
      // não continuar em direção ao botão (20°).
      for (pos = myservo.read(); pos <= posMax; pos += 1) {
        myservo.write(pos);
        delay(10);
      }
      return;
  }
  else {
    // Botão solto: volta para a posição de "dentro da caixa" (90 graus)
    timeDelay = random(1, 4);
    for (pos = myservo.read(); pos <= posMax; pos += timeDelay) {
      myservo.write(pos);
      delay(15);
    }
  }
}
