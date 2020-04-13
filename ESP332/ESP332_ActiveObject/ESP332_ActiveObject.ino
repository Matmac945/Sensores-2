//port

#include <Arduino.h>

QueueHandle_t aoBlinkQueue;
TimerHandle_t aoTimer;

typedef enum {
  evTimeout = 0U,
  evButton2,
  evButton1,
  AOBLINK_NO_MSG
} AOBLINK_EVENT_TYPE;

typedef struct
{
  AOBLINK_EVENT_TYPE evType;
  uint8_t evData;
} evAoBlink;

typedef enum
{
  Fast,
  FastLedOn,
  FastLedOff,
  Off,
  On,
  SlowWaitForLastTimeout,
  FastWaitForLastTimeout,
  Slow,
  SlowLedOn,
  SlowLedOff,
  NUM_STATES // number of states in the machine
} States;

typedef struct
{
  States stateVar;
  States stateVarSlow;
  States stateVarFast;
} stateVarsT;

stateVarsT stateVars;
stateVarsT stateVarsCopy;

void setup() {
  Serial.begin(9600);
  aoBlinkQueue = xQueueCreate(10, sizeof(evAoBlink));
  if (aoBlinkQueue != NULL)
  {
    //printf("aoBlink state machine created\r\n");
    xTaskCreate(aoBlink, "aoBlink", 1024 * 2, NULL, 2, NULL);
    xTaskCreate(serialTask, "serialTask", 1024 * 2, NULL, 1, NULL);
  }
  else
  {
    printf("aoBlinkQueue is not created\r\n");
  }
}

static void serialTask(void *pdata) {
  evAoBlink ev;
  char temp;
  BaseType_t xStatus;
  bool flag = 0;
  for (;;) {
    if (Serial.available() > 0) {
      temp =  Serial.read();
      if (temp == 'A') {
        ev.evType = evButton1;
        flag = 1;
      }
      else if (temp == 'S') {
        ev.evType = evButton2;
        flag = 1;
      }
      else {
        Serial.println("Dato invalido!");
      }
      if (flag == true) {
        xStatus = xQueueSendToBack(aoBlinkQueue, &ev, 0);
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

static void aoBlink(void *pdata) {
  BaseType_t xStatus;
  evAoBlink rxEvent;
  initStateMachine();
  for (;;) {
    xStatus = xQueueReceive(aoBlinkQueue, &rxEvent, portMAX_DELAY);
    if (xStatus == pdPASS)
    {
      //Serial.println(rxEvent.evType);
      aoStateMachine(rxEvent);
    }
  }
}

static void aoTimerCallback(TimerHandle_t xTimer)
{
  BaseType_t xStatus;
  evAoBlink ev;
  //printf("Sending timerEvent\r\n");
  ev.evType = evTimeout;
  xStatus = xQueueSendToBack(aoBlinkQueue, &ev, 0);
  //Serial.println("tick");
  if (xStatus != pdPASS)
  {
    Serial.println("aoTimerCallback couldn't send\r\n");
  }
}

static void initStateMachine() {
  BaseType_t xTimerCreatedStatus;

  //Create copy of statevar
  stateVarsCopy = stateVars;
  // Set state vars to default states
  stateVarsCopy.stateVar = Slow;          /* set init state of top state */
  stateVarsCopy.stateVarSlow = SlowLedOn; /* set init state of Slow */
  stateVarsCopy.stateVarFast = FastLedOn; /* set init state of Fast */
  aoTimer = xTimerCreate("aoTimer", pdMS_TO_TICKS(1000UL), pdTRUE, NULL, aoTimerCallback);
  if (aoTimer != NULL)
  {
    Serial.print("aoTimer created\r\n");
    xTimerCreatedStatus = xTimerStart(aoTimer, 0);
    if (xTimerCreatedStatus == pdPASS)
    {
      Serial.println("aoTimer started\r\n");
    }
  }
  stateVars = stateVarsCopy;
}

static void aoStateMachine(evAoBlink event) {

  int evConsumed = 0U;

  //Create copy of statevar
  stateVarsCopy = stateVars;

  switch (stateVars.stateVar)
  {
    case Slow:/* constant-expression */
      switch (stateVars.stateVarSlow)
      {
        case SlowLedOn:
          if (event.evType == evTimeout)
          {
            /* Transition from SlowLedOn to SlowLedOff */
            evConsumed = 1;

            /* OnEntry code of state SlowLedOff */
            //  gpio_set_level(GPIO_OUTPUT_IO_0, pdFALSE);
            Serial.println("LED OFF\r\n");
            /* adjust state variables  */
            stateVarsCopy.stateVarSlow = SlowLedOff;

          }
          else
          {
            /* Intentionally left blank */
          }
          break; /*end of event selection */
        case SlowLedOff:
          if (event.evType == evTimeout)
          {
            /* Transition from SlowLedOff to SlowLedOn */
            evConsumed = 1;

            /* OnEntry code of state SlowLedOn */
            //  gpio_set_level(GPIO_OUTPUT_IO_0, pdTRUE);
            Serial.println("LED ON\r\n");
            /* adjust state variables  */
            stateVarsCopy.stateVarSlow = SlowLedOn;
          }
          else
          {
            /* Intentionally left blank */
          }
          break; /*end of event selection */
        default:
          break; /* end switch Slow */
      }
      if (evConsumed == 0)
      {

        if (event.evType == evButton1)
        {
          /* Transition from Slow to SlowWaitForLastTimeout */
          evConsumed = 1;

          /* adjust state variables  */
          stateVarsCopy.stateVar = SlowWaitForLastTimeout;
        }
        else if (event.evType == evButton2)
        {
          /* Transition from Slow to Fast */
          evConsumed = 1;

          /* Action code for transition  */
          xTimerChangePeriod(aoTimer, pdMS_TO_TICKS(100UL), 0);
          //  gpio_set_level(GPIO_OUTPUT_IO_0, pdTRUE);
          Serial.println("LED ON\r\n");
          stateVarsCopy.stateVar = Fast;          /* Default in entry chain  */
          stateVarsCopy.stateVarFast = FastLedOn; /* Default in entry chain  */
        }
        else
        {
          /* Intentionally left blank */
        } /*end of event selection */
      }
      break; /* end of case Slow  */

    case Fast:
      switch (stateVars.stateVarFast)
      {
        case FastLedOn:
          if (event.evType == evTimeout)
          {
            /* Transition from FastLedOn to FastLedOff */
            evConsumed = 1;

            /* OnEntry code of state FastLedOff */
            //gpio_set_level(GPIO_OUTPUT_IO_0, pdFALSE);
            Serial.print("LED OFF\r\n");
            /* adjust state variables  */
            stateVarsCopy.stateVarFast = FastLedOff;
          }
          else
          {
            /* Intentionally left blank */
          }      /*end of event selection */
          break;
        case FastLedOff:
          if (event.evType == evTimeout)
          {
            /* Transition from FastLedOff to FastLedOn */
            evConsumed = 1;

            /* OnEntry code of state FastLedOn */
            //gpio_set_level(GPIO_OUTPUT_IO_0, pdTRUE);
            Serial.print("LED ON\r\n");


            /* adjust state variables  */
            stateVarsCopy.stateVarFast = FastLedOn;

          }
          else
          {
            /* Intentionally left blank */
          }      /*end of event selection */
          break;
        default:
          break;
      }
      /* Check if event was already processed  */
      if (evConsumed == 0)
      {

        if (event.evType == evButton1)
        {
          /* Transition from Fast to FastWaitForLastTimeout */
          evConsumed = 1;

          /* adjust state variables  */
          stateVarsCopy.stateVar = FastWaitForLastTimeout;
        }
        else if (event.evType == evButton2)
        {
          /* Transition from Fast to Slow */
          evConsumed = 1;

          /* Action code for transition  */
          xTimerChangePeriod(aoTimer, pdMS_TO_TICKS(1000UL), 0);
          //  gpio_set_level(GPIO_OUTPUT_IO_0, pdTRUE);
          Serial.println("LED ON\r\n");
          stateVarsCopy.stateVar = Slow;          /* Default in entry chain  */
          stateVarsCopy.stateVarSlow = SlowLedOn; /* Default in entry chain  */
        }
        else
        {
          /* Intentionally left blank */
        } /*end of event selection */
      }
      break; /* end of case Fast  */

    case Off:
      if (event.evType == evButton1)
      {
        /* Transition from Off to Slow */
        evConsumed = 1;

        /* OnEntry code of state Slow */
        xTimerChangePeriod(aoTimer, pdMS_TO_TICKS(1000UL), 0);
        //  gpio_set_level(GPIO_OUTPUT_IO_0, pdTRUE);
        Serial.println("LED ON\r\n");
        stateVarsCopy.stateVar = Slow;          /* Default in entry chain  */
        stateVarsCopy.stateVarSlow = SlowLedOn; /* Default in entry chain  */
      }
      else
      {
        /* Intentionally left blank */
      }      /*end of event selection */
      break; /* end of case Off  */

    case On:
      if (event.evType == evButton1)
      {
        /* Transition from On to Fast */
        evConsumed = 1;

        /* OnEntry code of state Fast */
        xTimerChangePeriod(aoTimer, pdMS_TO_TICKS(100UL), 0);
        //  gpio_set_level(GPIO_OUTPUT_IO_0, pdTRUE);
        Serial.println("LED ON\r\n");
        stateVarsCopy.stateVar = Fast;          /* Default in entry chain  */
        stateVarsCopy.stateVarFast = FastLedOn; /* Default in entry chain  */
      }
      else
      {
        /* Intentionally left blank */
      }      /*end of event selection */
      break; /* end of case On  */

    case SlowWaitForLastTimeout:
      if (event.evType == evTimeout)
      {
        /* Transition from SlowWaitForLastTimeout to Off */
        evConsumed = 1;

        /* OnEntry code of state Off */
        xTimerStop(aoTimer, 0);
        //  gpio_set_level(GPIO_OUTPUT_IO_0, pdFALSE);
        Serial.println("LED OFF\r\n");
        /* adjust state variables  */
        stateVarsCopy.stateVar = Off;
      }
      else
      {
        /* Intentionally left blank */
      }      /*end of event selection */
      break; /* end of case SlowWaitForLastTimeout  */

    case FastWaitForLastTimeout:
      if (event.evType == evTimeout)
      {
        /* Transition from FastWaitForLastTimeout to On */
        evConsumed = 1;

        /* OnEntry code of state On */
        xTimerStop(aoTimer, 0);
        //  gpio_set_level(GPIO_OUTPUT_IO_0, pdTRUE);
        Serial.println("LED ON\r\n");
        /* adjust state variables  */
        stateVarsCopy.stateVar = On;
      }
      else
      {
        /* Intentionally left blank */
      }      /*end of event selection */
      break; /* end of case FastWaitForLastTimeout  */

    default:
      break;/* end switch stateVar_root */
  }
  // Copy state variables back
  stateVars = stateVarsCopy;
}

void loop() {
}
