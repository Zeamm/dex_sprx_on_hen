int32_t ps3_mapi_read_process_memory(uint64_t address, void *data, size_t size);
int32_t ps3_mapi_write_process_memory(uint64_t address, const void *data, size_t size);
void HookFunctionStart(uint32_t functionStartAddress, uint32_t newFunction, uint32_t functionStub);
void UnHookFunctionStart(uint32_t functionStartAddress, uint32_t functionStub);
int32_t HookFunction(uint32_t address, uint32_t function);
int32_t HookFunction(uint32_t address, uint32_t function, bool linked);
int32_t HookFunctionLinked(uint32_t address, uint32_t function);
int32_t BranchLinkedHook(uint32_t branchFrom, uint32_t branchTo);
int32_t BranchHook(uint32_t branchFrom, uint32_t branchTo);

sys_prx_id_t LoadSPRX(const char *path);
void StartSPRX(sys_prx_id_t prxID);
uint32_t GetModuleBaseAddress(sys_prx_id_t prx_id);
uint32_t PRX_TO_GAME_MEMORY(uint32_t base_address, uint32_t prx_address);

void sleep(uint64_t ms);

void YesNoDialogCallBack(int button_type, void *userdata);
bool DrawYesNoMessageDialog(const char *format, ...);
