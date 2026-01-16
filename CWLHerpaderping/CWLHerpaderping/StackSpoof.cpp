#include "StackSpoof.h"
#include <stdio.h>
#include <intrin.h>  // For _AddressOfReturnAddress intrinsic

// Pattern for simple RET instruction (0xC3)
// This is a safe return point in legitimate code
const BYTE RET_PATTERN[] = { 0xC3 };

// Pattern for common function epilogue: add rsp, 0x28; ret
const BYTE EPILOGUE_PATTERN[] = { 0x48, 0x83, 0xC4, 0x28, 0xC3 };

PVOID FindReturnAddressGadget(const char* moduleName) {
    HMODULE hModule = GetModuleHandleA(moduleName);
    if (!hModule) {
        wprintf(L"[-] Failed to get module handle for %S\n", moduleName);
        return NULL;
    }

    MODULEINFO modInfo;
    if (!GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(modInfo))) {
        wprintf(L"[-] Failed to get module information for %S\n", moduleName);
        return NULL;
    }

    BYTE* base = (BYTE*)hModule;
    SIZE_T imageSize = modInfo.SizeOfImage;
    
    // Search for function epilogue pattern in .text section
    // This looks like a legitimate return from a function call
    for (SIZE_T i = 0; i < imageSize - sizeof(EPILOGUE_PATTERN); i++) {
        if (memcmp(base + i, EPILOGUE_PATTERN, sizeof(EPILOGUE_PATTERN)) == 0) {
            // Return address of the RET instruction
            PVOID gadget = (PVOID)(base + i + sizeof(EPILOGUE_PATTERN) - 1);
            wprintf(L"[+] Found gadget in %S at: %p\n", moduleName, gadget);
            return gadget;
        }
    }

    // Fallback: search for simple RET
    for (SIZE_T i = 0; i < imageSize - sizeof(RET_PATTERN); i++) {
        if (memcmp(base + i, RET_PATTERN, sizeof(RET_PATTERN)) == 0) {
            // Make sure it's in executable section
            MEMORY_BASIC_INFORMATION mbi;
            if (VirtualQuery(base + i, &mbi, sizeof(mbi))) {
                if (mbi.Protect == PAGE_EXECUTE_READ || 
                    mbi.Protect == PAGE_EXECUTE_READWRITE) {
                    PVOID gadget = (PVOID)(base + i);
                    wprintf(L"[+] Found simple RET gadget in %S at: %p\n", moduleName, gadget);
                    return gadget;
                }
            }
        }
    }

    wprintf(L"[-] Failed to find gadget in %S\n", moduleName);
    return NULL;
}

StackSpoofer::StackSpoofer(const char* trustedModule) 
    : isActive(FALSE), originalReturnAddress(NULL), fakeReturnAddress(NULL) {
    
    // Get the location of the return address on the stack
    returnAddressLocation = (PVOID*)_AddressOfReturnAddress();
    originalReturnAddress = *returnAddressLocation;
    
    // Find a legitimate gadget in trusted module
    fakeReturnAddress = FindReturnAddressGadget(trustedModule);
    
    if (!fakeReturnAddress) {
        wprintf(L"[!] Warning: Failed to find gadget, stack spoofing disabled\n");
    }
}

StackSpoofer::~StackSpoofer() {
    // Ensure stack is restored on destruction
    if (isActive) {
        Deactivate();
    }
}

void StackSpoofer::Activate() {
    if (!fakeReturnAddress) {
        return; // Spoofing not available
    }
    
    if (isActive) {
        wprintf(L"[!] Warning: Stack spoofing already active\n");
        return;
    }
    
    // Replace return address with fake one
    *returnAddressLocation = fakeReturnAddress;
    isActive = TRUE;
    
    wprintf(L"[*] Stack spoofed: %p -> %p\n", originalReturnAddress, fakeReturnAddress);
}

void StackSpoofer::Deactivate() {
    if (!isActive) {
        return;
    }
    
    // Restore original return address
    *returnAddressLocation = originalReturnAddress;
    isActive = FALSE;
    
    wprintf(L"[*] Stack restored: %p\n", originalReturnAddress);
}
