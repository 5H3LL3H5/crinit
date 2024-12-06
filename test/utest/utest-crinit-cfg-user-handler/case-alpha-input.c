// SPDX-License-Identifier: MIT
/**
 * @file case-alpha-input.c
 * @brief Unit test for crinitCfgUserHandler(), handling of alphabetical input.
 */

#include "confhdl.h"
#include "common.h"
#include "unit_test.h"
#include "utest-crinit-cfg-user-handler.h"

#include <string.h>

void crinitCfgUserHandlerTestAlphaInput(void **state) {
    CRINIT_PARAM_UNUSED(state);

    crinitTask_t tgt;
    memset(&tgt, 0x00, sizeof(tgt));
    const char *val = "www-run";
    assert_int_equal(crinitCfgUserHandler(&tgt, val, CRINIT_CONFIG_TYPE_TASK), -1);
    assert_int_equal(tgt.user, 0);
}
