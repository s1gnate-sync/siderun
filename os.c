#include "jslib.h"

void time_() {
	returnNumber(time(NULL));
}

void reboot_() {
	if (reboot(toInteger(1)) == -1) {
		printLastError("reboot");
		returnFalse();
	}

	returnUndefined();
}

void initOs() {
	regFunction(reboot);	
	regFunction(time);
}
