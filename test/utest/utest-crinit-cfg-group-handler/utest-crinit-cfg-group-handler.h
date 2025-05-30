// SPDX-License-Identifier: MIT
/**
 * @file utest-crinit-cfg-group-handler.h
 * @brief Header declaring the unit tests for crinitCfgGroupHandler().
 */
#ifndef __UTEST_CFG_GROUP_HANDLER_H__
#define __UTEST_CFG_GROUP_HANDLER_H__

/**
 * Tests successful parsing of a numeric group ID.
 */
void crinitCfgGroupHandlerTestNumericSuccess(void **state);
/**
 * Tests successful parsing of a alphabetical group name (e.g. "disk") instead of an ID.
 */
void crinitCfgGroupHandlerTestAlphaInputSuccess(void **state);
/**
 * Tests successful parsing of two alphabetical group names (e.g. "disk floppy") instead of an ID.
 */
void crinitCfgGroupHandlerTestAlphaInputTwoGroupsSuccess(void **state);
/**
 * Tests successful parsing of three alphabetical group names (e.g. "nogroup disk floppy") instead of an ID.
 */
void crinitCfgGroupHandlerTestAlphaInputThreeGroupsSuccess(void **state);
/**
 * Tests successful parsing of two numeric group IDs.
 */
void crinitCfgGroupHandlerTestNumericMultipleGroupsSuccess(void **state);
/**
 * Tests unsuccessful parsing of a negative numeric group ID.
 */
void crinitCfgGroupHandlerTestNegativeInput(void **state);
/**
 * Tests detection of NULL pointer input.
 */
void crinitCfgGroupHandlerTestNullInput(void **state);
/**
 * Tests handling of empty value part.
 */
void crinitCfgGroupHandlerTestEmptyInput(void **state);
#endif /* __UTEST_CFG_GROUP_HANDLER_H__ */
