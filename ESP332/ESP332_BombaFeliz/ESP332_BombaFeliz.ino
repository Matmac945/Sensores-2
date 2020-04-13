#include <Arduino.h>

TimerHandle_t BombTimer;
QueueHandle_t BombQueue;

uint8_t times = 20;
uint8_t codePos = 0;
uint8_t code[5];
uint8_t R_code[] = {0, 2, 4, 9, 12};

bool pass = true;

typedef enum {
  evTimeout = 0,
  evButtonUP,
  evButtonDOWN,
  evARM
} BOMB_EVENT_TYPE;

typedef struct
{
  BOMB_EVENT_TYPE evType;
  uint8_t evData;
} evBomb;

typedef enum
{
  Config,
  Armed,
  Boom,
} States;

States stateVar;
States stateVarsCopy;

void setup() {
  Serial.begin(9600);
  BombQueue = xQueueCreate(10, sizeof(evBomb));
  if (BombQueue != NULL)
  {
    xTaskCreate(Bomb, "Bomb", 1024 * 2, NULL, 2, NULL);
    xTaskCreate(serialTask, "serialTask", 1024 * 2, NULL, 1, NULL);
  }
  else
  {
    printf("BombQueue is not created\r\n");
  }

}

static void Bomb(void *pdata) {
  BaseType_t xStatus;
  evBomb rxEvent;
  initStateMachine();
  for (;;) {
    xStatus = xQueueReceive(BombQueue, &rxEvent, portMAX_DELAY);
    if (xStatus == pdPASS)
    {
      BombStateMachine(rxEvent);
    }
  }
}

static void serialTask(void *pdata) {
  evBomb ev;
  char temp;
  BaseType_t xStatus;
  bool flag = 0;
  for (;;) {
    if (Serial.available() > 0) {
      temp =  Serial.read();
      if (temp == 'A') {
        ev.evType = evButtonUP;
        flag = 1;
      }
      else if (temp == 'S') {
        ev.evType = evButtonDOWN;
        flag = 1;
      }
      else if (temp == 'D') {
        ev.evType = evARM;
        flag = 1;
      }
      else {
        Serial.println("Dato invalido!");
      }
      if (flag == true) {
        xStatus = xQueueSendToBack(BombQueue, &ev, 0);
        if (xStatus != pdPASS)
        {
          Serial.print("Could not send to the queue.\r\n");
        }
        flag = 0;
      }
    }
    delay(50);
  }
}

static void BombTimerCallback(TimerHandle_t xTimer)
{
  BaseType_t xStatus;
  evBomb ev;
  ev.evType = evTimeout;
  xStatus = xQueueSendToBack(BombQueue, &ev, 0);
  if (xStatus != pdPASS)
  {
    Serial.print("BOMBA! no pudo explotar\r\n");
  }
}

static void initStateMachine() {
  stateVar = Config;
  Serial.println(times);
  BombTimer = xTimerCreate("BombTimer", pdMS_TO_TICKS(1000UL), pdTRUE, NULL, BombTimerCallback);
  if (BombTimer != NULL)
  {
    Serial.print("Temporizador de la bomba creado\r\n");
  }
  stateVar = stateVarsCopy;
}

static void BombStateMachine(evBomb event) {


  switch (stateVar)
  {
    case Config:

      if (event.evType == evButtonUP) {
        if (times <= 60) {
          times ++;
          Serial.print("Tiempo: ");
          Serial.print(times);
          Serial.println(" segundos");
        }
      } else if (event.evType == evButtonDOWN) {
        if (times >= 0) {
          times --;
          Serial.print("Tiempo: ");
          Serial.println(times);
          Serial.println(" segundos");
        }
      } else if (event.evType == evARM) {
        BaseType_t xTimerCreatedStatus;
        xTimerCreatedStatus = xTimerStart(BombTimer, 0);
        if (xTimerCreatedStatus == pdPASS) {
          Serial.println("Temporizador de la bomba activado\r\n");
          stateVar = Armed;
        }
      } else {
        //Vacio aproposito
      }
      break;

    case Armed:
      if (event.evType == evButtonUP) {
        if (codePos < 5) {
          code[codePos] = codePos * 3;
          codePos++;
        } else {
          Serial.println("Muchos digitos!");
        }
      }
      if (event.evType == evButtonDOWN) {
        if (codePos < 5) {
          code[codePos] = codePos * 2;
          codePos++;
        } else {
          Serial.println("Muchos digitos");
        }
      }
      if (event.evType == evARM) {
        for (int i = 0; i < 5; i++) {
          if (code[i] != R_code[i]) {
            pass = false;
          }
        }
        if (pass == true ) {
          Serial.println("Bomba desactivada!");
          codePos = 0 ;
          stateVar = Config;
        } else {
          Serial.println("Codigo incorrecto!");
          codePos = 0;
        }
      }
      if (event.evType == evTimeout) {
        times --;
        Serial.print("Tiempo restante: ");
        Serial.println(times);
      }
      if (times == 0) {
        Serial.println("Boom!!!!, el mundo se lleno de paz y felicidad! :D");
        stateVar = Boom;
      }
      break;
    case Boom:
      // final del programa!!
      break;
    default:
      break;
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
