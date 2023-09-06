#include "stdafx.h"

int32_t ps3_mapi_read_process_memory(uint64_t address, void *data, size_t size)
{
	system_call_6(8, 0x7777, 0x31, (uint64_t)sys_process_getpid(), address, (uint64_t)data, size);
	return_to_user_prog(uint32_t);
}
int32_t ps3_mapi_write_process_memory(uint64_t address, const void *data, size_t size)
{
	system_call_6(8, 0x7777, 0x32, (uint64_t)sys_process_getpid(), address, (uint64_t)data, size);
	return_to_user_prog(uint32_t);
}

void HookFunctionStart(uint32_t functionStartAddress, uint32_t newFunction, uint32_t functionStub)
{
	uint32_t normalFunctionStub[8], hookFunctionStub[4];
	ps3_mapi_read_process_memory(functionStartAddress, normalFunctionStub, 0x10);
	normalFunctionStub[4] = 0x3D600000 + ((functionStartAddress + 0x10 >> 16) & 0xFFFF);
	normalFunctionStub[5] = 0x616B0000 + (functionStartAddress + 0x10 & 0xFFFF);
	normalFunctionStub[6] = 0x7D6903A6;
	normalFunctionStub[7] = 0x4E800420;
	ps3_mapi_write_process_memory(functionStub, normalFunctionStub, 0x20);
	hookFunctionStub[0] = 0x3D600000 + ((newFunction >> 16) & 0xFFFF);
	hookFunctionStub[1] = 0x616B0000 + (newFunction & 0xFFFF);
	hookFunctionStub[2] = 0x7D6903A6;
	hookFunctionStub[3] = 0x4E800420;
	ps3_mapi_write_process_memory(functionStartAddress, hookFunctionStub, 0x10);
}

void UnHookFunctionStart(uint32_t functionStartAddress, uint32_t functionStub)
{
	uint32_t normalFunctionStub[4];
	ps3_mapi_read_process_memory(functionStub, normalFunctionStub, 0x10);
	ps3_mapi_write_process_memory(functionStartAddress, normalFunctionStub, 0x10);
}

int32_t HookFunction(uint32_t address, uint32_t function)
{
	uint32_t opcode[4];
	opcode[0] = 0x3D600000 + ((function >> 16) & 0xFFFF);
	opcode[1] = 0x616B0000 + (function & 0xFFFF);
	opcode[2] = 0x7D6903A6;
	opcode[3] = 0x4E800420;
	return ps3_mapi_write_process_memory(address, &opcode, 0x10);
}
int32_t HookFunction(uint32_t address, uint32_t function, bool linked)
{
	uint32_t opcode[4];
	opcode[0] = 0x3D600000 + ((function >> 16) & 0xFFFF);
	opcode[1] = 0x616B0000 + (function & 0xFFFF);
	opcode[2] = 0x7D6903A6;
	opcode[3] = 0x4E800420 + linked ? 1 : 0;
	return ps3_mapi_write_process_memory(address, &opcode, 0x10);
}
int32_t HookFunctionLinked(uint32_t address, uint32_t function)
{
	uint32_t opcode[4];
	opcode[0] = 0x3D600000 + ((function >> 16) & 0xFFFF);
	opcode[1] = 0x616B0000 + (function & 0xFFFF);
	opcode[2] = 0x7D6903A6;
	opcode[3] = 0x4E800421;
	return ps3_mapi_write_process_memory(address, &opcode, 0x10);
}

int32_t BranchLinkedHook(uint32_t branchFrom, uint32_t branchTo)
{
	uint32_t branch;
	if (branchTo > branchFrom)
		branch = 0x48000001 + (branchTo - branchFrom);
	else
		branch = 0x4C000001 - (branchFrom - branchTo);
	return ps3_mapi_write_process_memory(branchFrom, &branch, 4);
}

int32_t BranchHook(uint32_t branchFrom, uint32_t branchTo)
{
	uint32_t branch;
	if (branchTo > branchFrom)
		branch = 0x48000000 + (branchTo - branchFrom);
	else
		branch = 0x4C000000 - (branchFrom - branchTo);
	return ps3_mapi_write_process_memory(branchFrom, &branch, 4);
}


sys_prx_id_t LoadSPRX(const char *path)
{
	return sys_prx_load_module(path, 0, NULL);
}

void StartSPRX(sys_prx_id_t prxID)
{
	int modres;
	sys_prx_start_module(prxID, 0, NULL, &modres, 0, NULL);
}

uint32_t GetModuleBaseAddress(sys_prx_id_t prx_id)
{
	sys_prx_module_info_t info;
	sys_prx_segment_info_t segments[10];
	char filename[SYS_PRX_MODULE_FILENAME_SIZE];
	info.size = sizeof(info);
	info.segments = segments;
	info.segments_num = sizeof(segments) / sizeof(sys_prx_segment_info_t);
	info.filename = filename;
	info.filename_size = sizeof(filename);
	sys_prx_get_module_info(prx_id, NULL, &info);

	return info.segments[0].base;
}

uint32_t PRX_TO_GAME_MEMORY(uint32_t base_address, uint32_t prx_address)
{
	return base_address + prx_address;
}

void sleep(uint64_t ms)
{
	sys_timer_usleep(ms * 1000);
}

bool yesno_dialog_result = false;
bool yesno_dialog_input = false;

void YesNoDialogCallBack(int button_type, void *userdata)
{
	switch (button_type)
	{
	case CELL_MSGDIALOG_BUTTON_YES:
		yesno_dialog_result = true;
		break;
	case CELL_MSGDIALOG_BUTTON_NO:
		yesno_dialog_result = false;
		break;
	}
	yesno_dialog_input = false;
}

bool DrawYesNoMessageDialog(const char *format, ...)
{
	va_list argptr;
	char text[CELL_MSGDIALOG_STRING_SIZE];
	va_start(argptr, format);
	_sys_vsprintf(text, format, argptr);
	va_end(argptr);
	cellMsgDialogOpen2(CELL_MSGDIALOG_TYPE_SE_TYPE_NORMAL | CELL_MSGDIALOG_TYPE_BUTTON_TYPE_YESNO | CELL_MSGDIALOG_TYPE_DISABLE_CANCEL_ON | CELL_MSGDIALOG_TYPE_DEFAULT_CURSOR_NO, text, YesNoDialogCallBack, NULL, NULL);
	yesno_dialog_input = true;
	while (yesno_dialog_input)
	{
		sleep(15);
		cellSysutilCheckCallback();
	}
	return yesno_dialog_result;
}
