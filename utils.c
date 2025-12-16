#include <time.h>

struct tm* 
timestamp() {
	time_t login_timestamp;
	time(&login_timestamp);
	struct tm *ts = localtime(&login_timestamp);
	return ts;
}
