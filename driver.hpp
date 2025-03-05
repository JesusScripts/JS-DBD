#pragma once

#include <Windows.h>
#include <TlHelp32.h>
#include <cstdint>
#include <vector>
#include "crypt.h"

#pragma comment(lib, "Ntdll.lib")

uintptr_t virtualaddy;
uintptr_t cr3;

#define CODE_RW CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8500, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define CODE_BA CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8601, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define CODE_C3 CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8902, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define CODE_MOUSE CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8803, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define CODE_PROT CTL_CODE(FILE_DEVICE_UNKNOWN, 0x8704, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define CODE_SECURITY 0x12340678

typedef struct _rw {
    INT32 security;
    INT32 process_id;
    ULONGLONG address;
    ULONGLONG buffer;
    ULONGLONG size;
    BOOLEAN write;
} rw, * prw;

typedef struct sBaseAddress {
    INT32 security;
    INT32 process_id;
    ULONGLONG* address;
} ba, * pba;

typedef struct _movemouse {
    long x;
    long y;
    unsigned short button_flags;
} movemouse, * MouseMovementStruct;

typedef struct _MEMORY_OPERATION_DATA {
    uint32_t pid;
    ULONGLONG* cr3;
} MEMORY_OPERATION_DATA, * PMEMORY_OPERATION_DATA;

#include <winternl.h>

extern "C" NTSTATUS NTAPI NtCreateFile(
    PHANDLE FileHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PIO_STATUS_BLOCK IoStatusBlock,
    PLARGE_INTEGER AllocationSize,
    ULONG FileAttributes,
    ULONG ShareAccess,
    ULONG CreateDisposition,
    ULONG CreateOptions,
    PVOID EaBuffer,
    ULONG EaLength
);

typedef NTSTATUS(NTAPI* NtCreateFile_t)(
    PHANDLE FileHandle,
    ACCESS_MASK DesiredAccess,
    POBJECT_ATTRIBUTES ObjectAttributes,
    PIO_STATUS_BLOCK IoStatusBlock,
    PLARGE_INTEGER AllocationSize,
    ULONG FileAttributes,
    ULONG ShareAccess,
    ULONG CreateDisposition,
    ULONG CreateOptions,
    PVOID EaBuffer,
    ULONG EaLength
    );

typedef struct _TARGET_PROCESS {
    int ProcessId;
} TARGET_PROCESS, * PTARGET_PROCESS;

namespace mem {
    HANDLE driver_handle;
    INT32 process_id;

    bool is_loaded() {
        UNICODE_STRING DeviceName;
        OBJECT_ATTRIBUTES ObjAttr;
        IO_STATUS_BLOCK IoStatusBlock;
        NTSTATUS status;

        RtlInitUnicodeString(&DeviceName, L"\\Device\\RabnaGirAs");

        InitializeObjectAttributes(&ObjAttr, &DeviceName, OBJ_CASE_INSENSITIVE, NULL, NULL);

        NtCreateFile_t NtCreateFile = (NtCreateFile_t)GetProcAddress(GetModuleHandleW(skCrypt(L"ntdll.dll")), skCrypt("NtCreateFile"));

        if (NtCreateFile == NULL) {
            return false;
        }

        status = NtCreateFile(&driver_handle, GENERIC_READ | GENERIC_WRITE, &ObjAttr, &IoStatusBlock, NULL,
            FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN, 0, NULL, 0);

        if (!NT_SUCCESS(status) || (driver_handle == INVALID_HANDLE_VALUE)) {
            return false;
        }

        return true;
    }

    bool init() {
        UNICODE_STRING DeviceName;
        OBJECT_ATTRIBUTES ObjAttr;
        IO_STATUS_BLOCK IoStatusBlock;
        NTSTATUS status;

        RtlInitUnicodeString(&DeviceName, L"\\Device\\RabnaGirAs");

        InitializeObjectAttributes(&ObjAttr, &DeviceName, OBJ_CASE_INSENSITIVE, NULL, NULL);

        NtCreateFile_t NtCreateFile = (NtCreateFile_t)GetProcAddress(GetModuleHandleW(skCrypt(L"ntdll.dll")), skCrypt("NtCreateFile"));

        if (NtCreateFile == NULL) {
            return false;
        }

        status = NtCreateFile(&driver_handle, GENERIC_READ | GENERIC_WRITE, &ObjAttr, &IoStatusBlock, NULL,
            FILE_ATTRIBUTE_NORMAL, FILE_SHARE_READ | FILE_SHARE_WRITE, FILE_OPEN, 0, NULL, 0);

        if (!NT_SUCCESS(status) || (driver_handle == INVALID_HANDLE_VALUE)) {
            return false;
        }

        TARGET_PROCESS targetProcess;
        targetProcess.ProcessId = GetCurrentProcessId();

        DWORD bytesReturned;
        BOOL result = DeviceIoControl(
            driver_handle,
            CODE_PROT,
            &targetProcess,
            sizeof(TARGET_PROCESS),
            NULL,
            0,
            &bytesReturned,
            NULL
        );

        return true;
    }

    void read_physical(uintptr_t address, PVOID buffer, DWORD size) {
        _rw arguments = { 0 };

        arguments.security = CODE_SECURITY;
        arguments.address = (ULONGLONG)address;
        arguments.buffer = (ULONGLONG)buffer;
        arguments.size = size;
        arguments.process_id = process_id;
        arguments.write = FALSE;

        DeviceIoControl(driver_handle, CODE_RW, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
    }

    void write_physical(PVOID address, PVOID buffer, DWORD size) {
        _rw arguments = { 0 };

        arguments.security = CODE_SECURITY;
        arguments.address = (ULONGLONG)address;
        arguments.buffer = (ULONGLONG)buffer;
        arguments.size = size;
        arguments.process_id = process_id;
        arguments.write = TRUE;

        DeviceIoControl(driver_handle, CODE_RW, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);
    }

    void move_mouse(long x, long y, unsigned short button) {
        movemouse req = { 0 };
        req.x = x;
        req.y = y;
        req.button_flags = button;
        DWORD bytesReturned;
        DeviceIoControl(
            driver_handle,
            CODE_MOUSE,     // IOCTL code
            &req,                   // Input buffer
            sizeof(req),            // Size of input buffer
            &req,                   // Output buffer
            sizeof(req),            // Size of output buffer
            &bytesReturned,         // Number of bytes returned
            nullptr                 // OVERLAPPED structure, NULL for synchronous operation
        );
    }

    uintptr_t base_address() {
        uintptr_t image_address = { NULL };
        sBaseAddress arguments = { NULL };

        arguments.security = CODE_SECURITY;
        arguments.process_id = process_id;
        arguments.address = (ULONGLONG*)&image_address;

        DeviceIoControl(driver_handle, CODE_BA, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);

        return image_address;
    }

    uintptr_t fetch_cr3() {
        _MEMORY_OPERATION_DATA arguments = { 0 };

        arguments.pid = process_id;
        DeviceIoControl(driver_handle, CODE_C3, &arguments, sizeof(arguments), nullptr, NULL, NULL, NULL);

        return true;
    }

    INT32 find_process(LPCTSTR process_name) {
        PROCESSENTRY32 pt;
        HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        pt.dwSize = sizeof(PROCESSENTRY32);
        if (Process32First(hsnap, &pt)) {
            do {
                if (!lstrcmpi(pt.szExeFile, process_name)) {
                    CloseHandle(hsnap);
                    process_id = pt.th32ProcessID;
                    return pt.th32ProcessID;
                }
            } while (Process32Next(hsnap, &pt));
        }
        CloseHandle(hsnap);

        return { NULL };
    }
}

template <typename T>
T read2(uintptr_t address) {
    T buffer{ };
    mem::read_physical(address, &buffer, sizeof(T));
    return buffer;
}

template <typename T>
T read(uint64_t address) {
    T buffer{ };
    mem::read_physical(address, &buffer, sizeof(T));
    return buffer;
}


template<typename T>
bool read_array(uintptr_t address, T out[], size_t len) {
    for (size_t i = 0; i < len; ++i) {
        out[i] = read<T>(address + i * sizeof(T));
    }
    return true;
}

template <typename T>
T write(uint64_t address, T buffer) {
    mem::write_physical((PVOID)address, &buffer, sizeof(T));
    return buffer;
}

bool is_valid(const uint64_t address) {
    if (address <= 0x400000 || address == 0xCCCCCCCCCCCCCCCC || reinterpret_cast<void*>(address) == nullptr || address > 0x7FFFFFFFFFFFFFFF) {
        return false;
    }
    return true;
}

template <typename T>
std::vector<T> batch_read(const std::vector<uint64_t>& addresses) {
    size_t num_addresses = addresses.size();
    std::vector<T> results(num_addresses);
    std::vector<uint8_t> buffer(num_addresses * sizeof(T));

    for (size_t i = 0; i < num_addresses; ++i) {
        mem::read_physical((PVOID)addresses[i], buffer.data() + i * sizeof(T), sizeof(T));
    }

    for (size_t i = 0; i < num_addresses; ++i) {
        results[i] = *reinterpret_cast<T*>(buffer.data() + i * sizeof(T));
    }

    return results;
}

template <typename T>
void write_bit(uint64_t address, uint8_t bitPosition, T value) {
    uint8_t byteAtOffset = read<uint8_t>(address);

    if (value) {
        byteAtOffset |= (1 << bitPosition);
    }
    else {
        byteAtOffset &= ~(1 << bitPosition);
    }
    write<uint8_t>(address, byteAtOffset);
}
