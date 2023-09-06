#include "stdafx.h"

#pragma diag_suppress 77
SYS_MODULE_INFO( hen_sprx_loader, 0, 1, 1);
SYS_MODULE_START( prx_entry );

#define SPRX_PATH "/dev_hdd0/tmp/RetroClient.sprx"

void hook_function_start_hook(uint32_t address, uint32_t newFunction, uint32_t functionStub)
{
	HookFunctionStart(address, *(uint32_t*)newFunction, *(uint32_t*)functionStub);
}
void hook_function_start_hook_2(uint32_t address, uint32_t functionStub, uint32_t newFunction)
{
	HookFunctionStart(address, *(uint32_t*)newFunction, *(uint32_t*)functionStub);
}

void hook_function_linked(uint32_t address, uint32_t newFunction, bool linked)
{
	HookFunction(address, *(uint32_t*)newFunction/*, linked*/);
}

int32_t write_process_hook(uint32_t address, void* data, size_t size)
{
	system_call_6(8, 0x7777, 0x32, (uint64_t)sys_process_getpid(), address, (uint64_t)data, size);
	return_to_user_prog(uint32_t);
}

int32_t read_process_hook(uint32_t address, void* data, size_t size)
{
	system_call_6(8, 0x7777, 0x31, (uint64_t)sys_process_getpid(), address, (uint64_t)data, size);
	return_to_user_prog(uint32_t);
}

sys_prx_id_t prx_id;
uint32_t base_address;
extern "C"
{
	int prx_entry(void)
	{
		prx_id = LoadSPRX(SPRX_PATH);
		base_address = GetModuleBaseAddress(prx_id); // get where the sprx is loaded in the game memory

		sys_ppu_thread_t loader_thread;
		sys_ppu_thread_create(&loader_thread, [](uint64_t arg)
		{
			sleep(3000);

			if (DrawYesNoMessageDialog("Do you want to load '"SPRX_PATH"'"))
			{
				// function address of the dex/cex sprx we need to patch
				uint32_t hookfunctionstart_address = PRX_TO_GAME_MEMORY(base_address, 0x4A0);
				uint32_t hookfunction_address = PRX_TO_GAME_MEMORY(base_address, 0x388);
				uint32_t writeprocess_address = PRX_TO_GAME_MEMORY(base_address, 0xD0);
				//uint32_t readprocess_address = PRX_TO_GAME_MEMORY(base_address, 0); not needed for the current example sprx

				// hook the functions to make them use hen syscalls
				HookFunction(hookfunctionstart_address, *(uint32_t*)hook_function_start_hook_2);
				HookFunction(hookfunction_address, *(uint32_t*)hook_function_linked);
				HookFunction(writeprocess_address, *(uint32_t*)write_process_hook);
				//HookFunction(readprocess_address, *(uint32_t*)hook_function_start_hook);

				sleep(500);

				// start the sprx
				StartSPRX(prx_id);
			}

			sys_ppu_thread_exit(0);
		}, 0, 100, 0x64, 0, "loader_thread");

		return SYS_PRX_START_OK;
	}
}
