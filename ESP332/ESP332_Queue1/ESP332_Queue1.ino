#define mainDELAY_LOOP_COUNT                ( 0xffffff)

const char *pcTextForTask1 = "Soy la tarea 1";
const char *pcTextForTask2 = "Soy la tarea 2";
const char *pcTextForTask3 = "Soy la tarea 3";
int stack = 8192;

QueueHandle_t xQueue;


void setup() {
  Serial.begin(115200);

  /* The queue is created to hold a maximum of 5 values, each of which is
     large enough to hold a variable of type int32_t. */
  xQueue = xQueueCreate( 5, sizeof( int32_t ) );
  if ( xQueue != NULL )
  {
    /* Create the task that will read from the queue.  The task is created with
      priority 2, so above the priority of the sender tasks. */

    xTaskCreate( vReceiverTask, "Receiver", 2048, NULL, 2, NULL );

    /* Create two instances of the task that will send to the queue.  The task
      parameter is used to pass the value that the task will write to the queue,
      so one task will continuously write 100 to the queue while the other task
      will continuously write 200 to the queue.  Both tasks are created at
      priority 1. */
    xTaskCreate( vSenderTask, "Sender1", 2048, ( void * ) 100, 1, NULL );
    xTaskCreate( vSenderTask, "Sender2", 2048, ( void * ) 200, 1, NULL );
  }
  else
  {
    /* The queue could not be created. */
  }
}

void vReceiverTask(void *pvParameters) {
  /* Declare the variable that will hold the values received from the queue. */
  int32_t lReceivedValue;
  BaseType_t xStatus;
  const TickType_t xTicksToWait = pdMS_TO_TICKS( 100 );

  for (;;) {
    printf( "There are (%d) messages waiting\r\n", uxQueueMessagesWaiting( xQueue ));
    /* Receive data from the queue.

      The first parameter is the queue from which data is to be received.

      The second parameter is the buffer into which the received data will be
      placed.  In this case the buffer is simply the address of a variable that
      has the required size to hold the received data.

      The last parameter is the block time – the maximum amount of time that the
      task will remain in the Blocked state to wait for data to be available */

    printf("Receiver stack: %d\r\n", uxTaskGetStackHighWaterMark(NULL));

    xStatus = xQueueReceive( xQueue, &lReceivedValue, xTicksToWait );

    if ( xStatus == pdPASS )
    {
      /* Data was successfully received from the queue, print out the received
        value. */
      printf( "Received = %d\r\n", lReceivedValue );
    }
    else
    {
      /* Data was not received from the queue even after waiting for 100ms.
        This must be an error as the sending tasks are free running and will be
        continuously writing to the queue. */
      printf( "After 100ms blocking time, could not receive from the queue.\r\n" );
    }
  }
}

static void vSenderTask( void *pvParameters ) {
  int32_t lValueToSend;
  BaseType_t xStatus;

  lValueToSend = ( int32_t ) pvParameters;

  for (;;) {

    printf( "Sender(%d). stack: %d\r\n", lValueToSend, uxTaskGetStackHighWaterMark(NULL));
    xStatus = xQueueSendToBack( xQueue, &lValueToSend, 0 );

    if ( xStatus != pdPASS )
    {
      /* The send operation could not complete because the queue was full - */
      printf( "Could not send to the queue.\r\n" );
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
}
