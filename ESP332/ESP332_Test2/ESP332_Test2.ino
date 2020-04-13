const char *pcTextForTask1 = "Soy la tarea 1";
const char *pcTextForTask2 = "Soy la tarea 2";


void setup() {
  Serial.begin(115200);
  //Task 1
  xTaskCreate(vTask, "vTask1", 2048, (void *)pcTextForTask1, 1, NULL);

  //Task 2
  xTaskCreate(vTask, "vTask2", 2048, (void *)pcTextForTask2, 1, NULL);
}

void vTask(void *pvParameters) {
  char *pcTaskName;
  pcTaskName = (char *)pvParameters;

  for (;;) {
    Serial.println(pcTaskName);
    delay(1000);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
