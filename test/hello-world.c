/* SPDX-License-Identifier: BSD-2 */
#ifndef EDK2_BUILD
#include <efi/efi.h>
#include <efi/efilib.h>
#else
#include <Uefi.h>
#include <Library/UefiLib.h>
#endif

EFI_STATUS EFIAPI
efi_main (
    EFI_HANDLE ImageHandle,
    EFI_SYSTEM_TABLE *SystemTable
    )
{
#ifndef EDK2_BUILD
    InitializeLib (ImageHandle, SystemTable);
#endif
    Print (L"hello world!\n");
    return EFI_SUCCESS;
}
