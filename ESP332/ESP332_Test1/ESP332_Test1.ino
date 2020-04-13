void setup() {
  Serial.begin(115200);
  //Task 1
  xTaskCreate(vTask1, "vTask1", 2048, NULL, 1, NULL);

  //Task 2
  xTaskCreate(vTask2, "vTask2", 2048, NULL, 1, NULL);
}

void vTask(void *pvParameters) {
  for (;;) {
    Serial.println("Soy la tarea 1");
    delay(1000);
  }
}
void vTask2(void *pvParameters) {
  for (;;) {
    Serial.println("Soy la tarea 2");
    delay(1000);
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
