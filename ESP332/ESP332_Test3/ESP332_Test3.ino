#define mainDELAY_LOOP_COUNT                ( 0xffffff)

const char *pcTextForTask1 = "Soy la tarea 1";
const char *pcTextForTask2 = "Soy la tarea 2";
const char *pcTextForTask3 = "Soy la tarea 3";
int stack = 8192;

void setup() {
  Serial.begin(115200);
  //Task 1
  xTaskCreate(vTask, "vTask1", stack, (void *)pcTextForTask1, 3, NULL);

  //Task 2
  xTaskCreate(vTask, "vTask2", stack, (void *)pcTextForTask3, 1, NULL);

  //Task 2
  xTaskCreate(vTask, "vTask3", stack, (void *)pcTextForTask2, 10, NULL);
}

void vTask(void *pvParameters) {
  char *pcTaskName;
  volatile uint32_t ul;

  pcTaskName = (char *)pvParameters;

  String taskMessage = "Task running on core ";
  taskMessage = taskMessage + xPortGetCoreID();
  Serial.println(taskMessage);
  for (;;) {
    Serial.println(pcTaskName);
    for ( ul = 0; ul < mainDELAY_LOOP_COUNT; ul++ )
    {
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("Loop");
  delay(1000);
}
