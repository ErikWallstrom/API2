#include <termbox.h>
#include <stdio.h>

#include "../../log.h"

int main(void)
{
	switch(tb_init())
	{
	case TB_EUNSUPPORTED_TERMINAL:
		log_error("Unsupported terminal");
		break;
	case TB_EFAILED_TO_OPEN_TTY:
		log_error("Failed to open TTY");
		break;
	case TB_EPIPE_TRAP_ERROR:
		log_error("Pipe trap error");
		break;
	}

	tb_select_input_mode(TB_INPUT_MOUSE | TB_INPUT_ESC);

	int x = 0;
	int y = 0;

	tb_change_cell(x, y, '@', TB_WHITE | TB_BOLD, TB_DEFAULT);
	tb_present();

	struct tb_event event;
	while(tb_poll_event(&event))
	{
		tb_change_cell(x, y, ' ', TB_DEFAULT, TB_DEFAULT);
		switch(event.type)
		{
		case TB_EVENT_KEY:
			switch(event.ch)
			{
			case 'w':
			case 'k':
				y--;
				break;
			case 's':
			case 'j':
				y++;
				break;
			case 'a':
			case 'h':
				x--;
				break;
			case 'd':
			case 'l':
				x++;
				break;
			case 'q':
				goto done;
				break;
			}
			break;
		case TB_EVENT_MOUSE:
			break;
		case TB_EVENT_RESIZE:
			break;
		}

		tb_set_clear_attributes(TB_DEFAULT, TB_GREEN | TB_BOLD);
		tb_clear();
		tb_change_cell(20, 9, 0x2580, TB_BLUE, TB_DEFAULT);
		tb_change_cell(21, 10, 0x2593, TB_GREEN, TB_DEFAULT);
		tb_change_cell(19, 10, 0x2592, TB_GREEN, TB_DEFAULT);
		tb_change_cell(20, 10, 0x2591, TB_GREEN, TB_DEFAULT);
		tb_change_cell(20, 11, 0x2591, TB_YELLOW, TB_DEFAULT);
		tb_change_cell(x, y, '@', TB_WHITE | TB_BOLD, TB_DEFAULT);
		tb_present();
	}
	
done:
	tb_shutdown();
}

