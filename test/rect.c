#include "../rect.h"
#include "../log.h"

int main(void)
{
	double x = 10;
	double y = 10;

	struct Rect rect;
	rect_ctor(&rect, (struct Vector2d){x, y}, RECTREGPOINT_CENTER, 100, 50);

	log_info("Given position: %f, %f", x, y);
	log_info("Actual position: %f, %f", rect.pos.x, rect.pos.y);
	log_info(
		"rect_getpos: %f, %f", 
		rect_getpos(&rect, RECTREGPOINT_CENTER).x, 
		rect_getpos(&rect, RECTREGPOINT_CENTER).y
	);
}

