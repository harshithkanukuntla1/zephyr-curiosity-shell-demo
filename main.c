#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/shell/shell.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Roll number 2026mt01215 -> last digit D = 5 */
#define D 5
#define T_ON_MS  ((D + 1) * 100)   /* 600 ms */
#define T_OFF_MS 500               /* 500 ms */

#define NUM_LEDS     2
#define STACK_SIZE   1024
#define LED_PRIORITY 5

static const struct gpio_dt_spec leds[NUM_LEDS] = {
	GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios),
	GPIO_DT_SPEC_GET(DT_ALIAS(led1), gpios),
};

K_THREAD_STACK_ARRAY_DEFINE(led_stacks, NUM_LEDS, STACK_SIZE);
static struct k_thread led_threads[NUM_LEDS];

static bool running[NUM_LEDS];
static bool started[NUM_LEDS];

/* Blink loop runs inside the thread, not in the shell handler */
static void led_entry(void *p1, void *p2, void *p3)
{
	int idx = (int)(intptr_t)p1;

	ARG_UNUSED(p2);
	ARG_UNUSED(p3);

	while (1) {
		gpio_pin_set_dt(&leds[idx], 1);   /* LED ON  */
		k_msleep(T_ON_MS);
		gpio_pin_set_dt(&leds[idx], 0);   /* LED OFF */
		k_msleep(T_OFF_MS);
	}
}

static int parse_index(const struct shell *sh, const char *arg)
{
	if (strcmp(arg, "0") == 0) {
		return 0;
	}
	if (strcmp(arg, "1") == 0) {
		return 1;
	}
	shell_error(sh, "Invalid argument '%s'. Use 0 or 1.", arg);
	return -1;
}

static int cmd_start_thread(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);

	int idx = parse_index(sh, argv[1]);

	if (idx < 0) {
		return -EINVAL;
	}
	if (running[idx]) {
		shell_print(sh, "LED%d thread is already RUNNING", idx);
		return 0;
	}

	if (!started[idx]) {
		k_thread_start(&led_threads[idx]);    /* first start */
		started[idx] = true;
	} else {
		k_thread_resume(&led_threads[idx]);   /* resume after suspend */
	}
	running[idx] = true;
	shell_print(sh, "LED%d thread RUNNING", idx);
	return 0;
}

static int cmd_suspend_thread(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);

	int idx = parse_index(sh, argv[1]);

	if (idx < 0) {
		return -EINVAL;
	}
	if (!running[idx]) {
		shell_print(sh, "LED%d thread is already SUSPENDED", idx);
		return 0;
	}

	k_thread_suspend(&led_threads[idx]);
	gpio_pin_set_dt(&leds[idx], 0);           /* make sure LED is OFF */
	running[idx] = false;
	shell_print(sh, "LED%d thread SUSPENDED", idx);
	return 0;
}

static int cmd_thread_status(const struct shell *sh, size_t argc, char **argv)
{
	ARG_UNUSED(argc);
	ARG_UNUSED(argv);

	for (int i = 0; i < NUM_LEDS; i++) {
		shell_print(sh, "LED%d thread: %s", i,
			    running[i] ? "RUNNING" : "SUSPENDED");
	}
	return 0;
}

SHELL_CMD_ARG_REGISTER(start_thread, NULL,
		       "Resume LED blink thread: start_thread <0|1>",
		       cmd_start_thread, 2, 0);
SHELL_CMD_ARG_REGISTER(suspend_thread, NULL,
		       "Suspend LED blink thread: suspend_thread <0|1>",
		       cmd_suspend_thread, 2, 0);
SHELL_CMD_REGISTER(thread_status, NULL,
		   "Show whether the LED0 and LED1 threads are RUNNING or SUSPENDED",
		   cmd_thread_status);

int main(void)
{
	for (int i = 0; i < NUM_LEDS; i++) {
		if (!gpio_is_ready_dt(&leds[i])) {
			return 0;
		}
		if (gpio_pin_configure_dt(&leds[i], GPIO_OUTPUT_INACTIVE) < 0) {
			return 0;
		}
	}

	/* Threads are created but not started (K_FOREVER) -> initially SUSPENDED */
	for (int i = 0; i < NUM_LEDS; i++) {
		k_thread_create(&led_threads[i], led_stacks[i],
				K_THREAD_STACK_SIZEOF(led_stacks[i]),
				led_entry, (void *)(intptr_t)i, NULL, NULL,
				LED_PRIORITY, 0, K_FOREVER);
	}

	return 0;
}