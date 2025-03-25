# Welcome to esp-idf-templates! #
Look inside the `lib` folder in order to get started on how to configure UART ports, log messages via ESP_LOG, gpio pins, timers, etc.
Look inside the `test` folder to see unit tests i've created in order to test out the functionality of certain peripherals/features (uart, rtos, etc.)
In terms of the timer implementation, I added a callback function when the alarm condition is met. I tried to hide the interrupt flag via API calls in order to protect the boolean from being accidentally modified.
