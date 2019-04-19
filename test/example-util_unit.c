/* SPDX-License-Identifier: BSD-2 */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <tss2/tss2_tpm2_types.h>

#include <setjmp.h>
#include <cmocka.h>

#include "example/util.h"

static void
pcr0_is_set (void **state)
{
    BYTE pcr_selection [3] = { 0x1, };
    assert_true (is_pcr_selected (pcr_selection, 0));
}
static void
pcr17_is_set (void **state)
{
    BYTE pcr_selection [3] = { 0x0, 0x0, 0x2, };
    assert_true (is_pcr_selected (pcr_selection, 17));
}
static void
pcr10_not_set (void **state)
{
    BYTE pcr_selection [3] = { 0xff, 0xaa, 0xff, };
    assert_false (is_pcr_selected (pcr_selection, 10));
}
static void
pcr21_not_set (void **state)
{
    BYTE pcr_selection [3] = { 0xff, 0xff, 0xdf, };
    assert_false (is_pcr_selected (pcr_selection, 21));
}
static void
select_pcr14 (void **state)
{
    BYTE pcr_selection [3] = { 0, };
    select_pcr (pcr_selection, 14);
    assert_true (is_pcr_selected (pcr_selection, 14));
}
static void
deselect_pcr7 (void **state)
{
    BYTE pcr_selection [3] = { 0xff, 0xff, 0xff };
    deselect_pcr (pcr_selection, 7);
    assert_false (is_pcr_selected (pcr_selection, 7));
}
static void
deselect_pcr10 (void **state)
{
    BYTE pcr_selection [3] = { 0xff, 0xff, 0xff };
    deselect_pcr (pcr_selection, 10);
    assert_false (is_pcr_selected (pcr_selection, 10));
}
static void
deselect_pcr20 (void **state)
{
    BYTE pcr_selection [3] = { 0xff, 0xff, 0xff };
    deselect_pcr (pcr_selection, 20);
    assert_false (is_pcr_selected (pcr_selection, 20));
}
static void
count_alg_bitmap_0 (void **state)
{
    EFI_TCG2_EVENT_ALGORITHM_BITMAP bitmap = 0;
    assert_int_equal (count_algs_in_bitmap (bitmap), 0);
}
static void
count_alg_bitmap_17 (void **state)
{
    EFI_TCG2_EVENT_ALGORITHM_BITMAP bitmap = 0xf0f0f4f0;
    assert_int_equal (count_algs_in_bitmap (bitmap), 17);
}
static void
count_alg_bitmap_32 (void **state)
{
    EFI_TCG2_EVENT_ALGORITHM_BITMAP bitmap = 0xffffffff;
    assert_int_equal (count_algs_in_bitmap (bitmap), 32);
}
static void
select_all_pcrs_SHA1 (void **state)
{
    TPML_PCR_SELECTION selection = { 0, };
    EFI_TCG2_EVENT_ALGORITHM_BITMAP active_banks = EFI_TCG2_BOOT_HASH_ALG_SHA1;
    select_all_active_pcrs (active_banks, &selection);
    assert_int_equal (selection.count, 1);
    assert_int_equal (selection.pcrSelections [0].hash, TPM2_ALG_SHA1);
    assert_int_equal (selection.pcrSelections [0].pcrSelect [0], 0xff);
    assert_int_equal (selection.pcrSelections [0].pcrSelect [1], 0xff);
    assert_int_equal (selection.pcrSelections [0].pcrSelect [2], 0xff);
}
static void
select_all_pcrs_SHA256 (void **state)
{
    TPML_PCR_SELECTION selection = { 0, };
    EFI_TCG2_EVENT_ALGORITHM_BITMAP active_banks = EFI_TCG2_BOOT_HASH_ALG_SHA256;
    select_all_active_pcrs (active_banks, &selection);
    assert_int_equal (selection.count, 1);
    assert_int_equal (selection.pcrSelections [0].hash, TPM2_ALG_SHA256);
}
static void
select_all_pcrs_SHA384 (void **state)
{
    TPML_PCR_SELECTION selection = { 0, };
    EFI_TCG2_EVENT_ALGORITHM_BITMAP active_banks = EFI_TCG2_BOOT_HASH_ALG_SHA384;
    select_all_active_pcrs (active_banks, &selection);
    assert_int_equal (selection.count, 1);
    assert_int_equal (selection.pcrSelections [0].hash, TPM2_ALG_SHA384);
}
static void
select_all_pcrs_SHA512 (void **state)
{
    TPML_PCR_SELECTION selection = { 0, };
    EFI_TCG2_EVENT_ALGORITHM_BITMAP active_banks = EFI_TCG2_BOOT_HASH_ALG_SHA512;
    select_all_active_pcrs (active_banks, &selection);
    assert_int_equal (selection.count, 1);
    assert_int_equal (selection.pcrSelections [0].hash, TPM2_ALG_SHA512);
}
static void
select_all_pcrs_2_algs (void **state)
{
    TPML_PCR_SELECTION selection = { 0, };
    EFI_TCG2_EVENT_ALGORITHM_BITMAP active_banks =
        EFI_TCG2_BOOT_HASH_ALG_SHA1 | EFI_TCG2_BOOT_HASH_ALG_SHA512;
    select_all_active_pcrs (active_banks, &selection);
    assert_int_equal (selection.count, 2);
    assert_int_equal (selection.pcrSelections [0].hash, TPM2_ALG_SHA1);
    assert_int_equal (selection.pcrSelections [1].hash, TPM2_ALG_SHA512);
}
int
main(int argc, char* argv[])
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test (pcr0_is_set),
        cmocka_unit_test (pcr17_is_set),
        cmocka_unit_test (pcr10_not_set),
        cmocka_unit_test (pcr21_not_set),
        cmocka_unit_test (select_pcr14),
        cmocka_unit_test (deselect_pcr7),
        cmocka_unit_test (deselect_pcr10),
        cmocka_unit_test (deselect_pcr20),
        cmocka_unit_test (count_alg_bitmap_0),
        cmocka_unit_test (count_alg_bitmap_17),
        cmocka_unit_test (count_alg_bitmap_32),
        cmocka_unit_test (select_all_pcrs_SHA1),
        cmocka_unit_test (select_all_pcrs_SHA256),
        cmocka_unit_test (select_all_pcrs_SHA384),
        cmocka_unit_test (select_all_pcrs_SHA512),
        cmocka_unit_test (select_all_pcrs_2_algs),
    };
    return cmocka_run_group_tests (tests, NULL, NULL);
}
