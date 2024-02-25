/* @file entry.c

	Implementation of all user-mode programs' entry point.
	SPDX-License-Identifier: WTFPL

*/

#include <common.h>
#include <stdio.h>
#include <platform.h>
#include <datasource.h>

int main(void)
{
	status_t status = STATUS_SUCCESS;
	open_device();
	uint8_t channels = get_channel_count();
	status = initialize_data_source_binding(channels);
	close_device();
	return 0;
}
