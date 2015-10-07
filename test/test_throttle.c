/**
 * @file test_ref.c
 * @brief
 * @author Travis Lane
 * @version 0.0.1
 * @date 2015-10-06
 */

#include <check.h>

#include "throttle.h"
#include "throttle_internal.h"

Suite *
suite_throttle_new()
{
  Suite *suite = suite_create("suite_throttle");
  return suite;
}

int
main()
{
  int failed;
  Suite *suite = suite_throttle_new();
  SRunner *runner = srunner_create(suite);
  srunner_run_all(runner, CK_NORMAL);
  failed = srunner_ntests_failed(runner);
  srunner_free(runner);
  return failed;
}
