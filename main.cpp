#include <iostream>
#include <Windows.h>

#include "antidebug.h"

int main()
{
	std::printf("\t\t\t\t[AIMWARE TRICKS] by c57 <3 (Easton Gay)\n\n\n");

	hide_thread();
	
	check_debug_flags();
	
	getchar();

	return 0;
}

