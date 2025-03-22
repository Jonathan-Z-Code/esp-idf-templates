Look inside the `lib` folder in order to get started on how to configure UART ports, log messages via ESP_LOG, timers, etc.

In terms of the timer implementation, I added a callback function when the alarm condition is met. I tried to hide the interrupt flag via API calls in order to protect the boolean from being accidentally modified.
