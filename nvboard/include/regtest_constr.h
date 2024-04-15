#include <nvboard.h>
#include "Vregtest.h"

void nvboard_bind_all_pins(Vregtest* top) {
	nvboard_bind_pin( &top->d, 4, SW3, SW2, SW1, SW0);
	nvboard_bind_pin( &top->clk, 1, BTNC);
	nvboard_bind_pin( &top->q, 4, LD3, LD2, LD1, LD0);
}
