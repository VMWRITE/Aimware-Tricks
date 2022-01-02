#pragma once

__forceinline std::uintptr_t hide_thread()
{
	using NtSetInformationThreadFn = NTSTATUS( NTAPI* )
	(
		HANDLE ThreadHandle,
		ULONG  ThreadInformationClass,
		PVOID  ThreadInformation,
		ULONG  ThreadInformationLength
	);

	const ULONG hide_thread_from_debugger = 0x11;

	NtSetInformationThreadFn NtSetInformationThread = (NtSetInformationThreadFn)GetProcAddress
	(
		GetModuleHandleA("ntdll.dll"),
		"NtSetInformationThread"
	);

	NTSTATUS status = NtSetInformationThread
	(
		( HANDLE ) 0xFFFFFFFE,
		hide_thread_from_debugger,
		NULL,
		NULL
	);

	if( status != 0x0 )
	{
		std::printf("[ Aimware Antidebug ] NtSetInformationThread | 0x0\n\n");
		std::printf("[ Aimware Antidebug ] Starting execute syscall\n");

		const auto Wow64AllocMemory = VirtualAlloc
		(
			0x0,
			0x1000,
			MEM_COMMIT | MEM_RESERVE,
			PAGE_EXECUTE_READWRITE
		);

		__asm
		{
			mov edx, dword ptr fs:[0xC0]
			movups xmm0, xmmword ptr ds : [edx]
			mov eax, dword ptr ds : [Wow64AllocMemory]
			mov dword ptr ss : [ebp - 0x8] , eax
			movups xmmword ptr ds : [eax] , xmm0
		}

		__asm
		{
			push 0x0
			push 0x0
			push 0x11
			push 0xFFFFFFFE

			call $+5

			mov eax, 0xD

			call dword ptr ds : [ Wow64AllocMemory ]
		}

		return status;
	}

	std::printf("[ Aimware Antidebug ] NtSetInformationThread | 0x1\n");

	return status;
}

__forceinline std::uintptr_t check_debug_flags()
{
	using NtQueryInformationProcessFn = NTSTATUS( NTAPI* )
	(
		HANDLE           ProcessHandle,
		UINT ProcessInformationClass,
		PVOID            ProcessInformation,
		ULONG            ProcessInformationLength,
		PULONG           ReturnLength
	);

	const UINT debug_flags = 0x7;
	DWORD is_dbg = 0x0;

	NtQueryInformationProcessFn NtQueryInformationProcess = (NtQueryInformationProcessFn)GetProcAddress
	(
		GetModuleHandleA("ntdll.dll"), 
		"NtQueryInformationProcess" 
	);

	NTSTATUS status = NtQueryInformationProcess
	(
		( HANDLE )0xFFFFFFFF,
		debug_flags,
		&is_dbg,
		sizeof( DWORD ),
		NULL
	);

	if( status == 0x0 && is_dbg != 0x0 )
	{
		std::printf("[ Aimware Antidebug ] NtQueryInformationProcess | 0x1\n");
		return status;
	}

	std::printf("[ Aimware Antidebug ] NtQueryInformationProcess | 0x0\n\n");
	std::printf("[ Aimware Antidebug ] Starting execute syscall\n\n");

	const auto Wow64AllocMemory = VirtualAlloc
	(
		0x0,
		0x1000,
		MEM_COMMIT | MEM_RESERVE,
		PAGE_EXECUTE_READWRITE
	);

	__asm
	{
		mov edx, dword ptr fs : [0xC0]
		movups xmm0, xmmword ptr ds : [edx]
		mov eax, dword ptr ds : [Wow64AllocMemory]
		mov dword ptr ss : [esi + 0x8] , eax
		movups xmmword ptr ds : [eax] , xmm0
	}

	__asm
	{
		push 0x0
		push 0x4
		lea ecx, dword ptr ss : [is_dbg]
		push ecx
		push 0x7
		push 0xFFFFFFFF

		call $+5

		mov eax, 0x19

		call dword ptr ds : [Wow64AllocMemory]
	}

	if( is_dbg != 0x0 )
	{
		MessageBoxA(0, "NtQueryInformationProcess Syscall | Antidebug triggered", "Easton gay", 0);
	}
	else
	{
		std::printf("[ Aimware Antidebug ] NtQueryInformationProcess Syscall | 0x0\n");
	}
	
	return status;
}
