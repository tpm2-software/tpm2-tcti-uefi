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

    EFI_STATUS Status = SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
    if (EFI_ERROR(Status))
    {
    return Status;
    }

    Status = SystemTable->ConOut->OutputString(SystemTable->ConOut, L"\r\nHello World!\r\n");
    if (EFI_ERROR(Status))
    {
    return Status;
    }

    Print (L"hello world!\n");
    return EFI_SUCCESS;
}
