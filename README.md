# Interactive LED Blink Threads Using Zephyr Shell

**Course:** Real-Time Embedded Systems (RTES)  
**Target Board:** Microchip PIC32CM GC00 Curiosity Pro (`pic32cm_gc00_cpro`)  
**Author:** Kanukuntla Harshith Reddy  
**Roll Number:** 2026MT01215  

---

## Overview
This project implements interactive multi-threaded LED control on the **PIC32CM GC00 Curiosity Pro** board using the **Zephyr RTOS** and its built-in **Zephyr Shell** subsystem. Two independent threads (`LED0` and `LED1`) control the onboard LEDs and can be dynamically started, suspended, and monitored via UART serial commands.

---

## Timing Calculation
Based on the last digit of roll number **2026MT01215** ($D = 5$):

| Parameter | Formula | Calculation | Value |
| :--- | :--- | :--- | :--- |
| **$T_{ON}$** | $(D + 1) \times 100\text{ ms}$ | $(5 + 1) \times 100$ | **600 ms** |
| **$T_{OFF}$** | Fixed | $500$ | **500 ms** |

---

## Custom Shell Commands
* **Shell Prompt:** `2026MT01215>`
* **Serial Configuration:** `115200 8-N-1`

| Command | Description |
| :--- | :--- |
| `start_thread <0\|1>` | Starts or resumes the blink thread for `LED0` (`0`) or `LED1` (`1`). |
| `suspend_thread <0\|1>` | Suspends the blink thread for `LED0` (`0`) or `LED1` (`1`) and turns the LED off. |
| `thread_status` | Displays the current execution state (`RUNNING` or `SUSPENDED`) of both LED threads. |

---

## Project Structure
```text
curiosity_shell_demo/
├── CMakeLists.txt                   # CMake build configuration
├── prj.conf                         # Zephyr Kconfig options (GPIO & Shell enabled)
├── src/
│   └── main.c                       # Thread definitions, GPIO logic, and shell command handlers
└── 2026mt01215_labassignment2.pdf   # Lab assignment report & verification screenshots
