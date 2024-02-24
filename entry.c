/* @file entry.c

	Implementation of all user-mode programs' entry point.
	SPDX-License-Identifier: WTFPL

*/

#include <common.h>
#include <stdio.h>

#include <platform.h>
int main()
{
	open_device();
	get_channel_count();
	close_device();
	return 0;
}
