#include <stdio.h>
#include <stdlib.h>

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

xTaskHandle task_a_handle;
xTaskHandle task_b_handle;
xTaskHandle task_c_handle;

SemaphoreHandle_t resource_a;
SemaphoreHandle_t resource_b;
SemaphoreHandle_t resource_c;
void the_task(void *pvParameters)
{
	while (1)
	{
		xSemaphoreTake(resource_c, portMAX_DELAY);
		xSemaphoreTake(resource_a, portMAX_DELAY);
		...
		xSemaphoreTake(resource_b, portMAX_DELAY);
		xSemaphoreGive(resource_c, portMAX_DELAY)
		...
		if (...)
		{
			...
			xSemaphoreTake(resource_c, portMAX_DELAY);
			xSemaphoreGive(resource_a);
			...
			xSemaphoreTake(resource_a, portMAX_DELAY);
			xSemaphoreGive(resource_c,portMAX_DELAY);
			...
		}
		xSemaphoreGive(resource_b);
		...
		xSemaphoreGive(resource_a);
		...
	}
}

int main(int argc, char **argv)
{
    resource_a = xSemaphoreCreateMutex();
    resource_b = xSemaphoreCreateMutex();
    xTaskCreate(the_task, "Task 1", configMINIMAL_STACK_SIZE, NULL, 1, &task_a_handle);
    xTaskCreate(the_task, "Task 2", configMINIMAL_STACK_SIZE, NULL, 1, &task_b_handle);
    xTaskCreate(the_task, "Task 3", configMINIMAL_STACK_SIZE, NULL, 1, &task_c_handle);

    vTaskStartScheduler();
    for( ;; );
}

