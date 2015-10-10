/**
 * @file test_ref.c
 * @brief
 * @author Travis Lane
 * @version 0.0.1
 * @date 2015-10-06
 */

#include <check.h>
#include <unistd.h>

#include "throttle.h"
#include "throttle_internal.h"

START_TEST(test_throttle_std_start_stop)
{
  int rc;
  struct lb_throttle_t *throttle = lb_throttle_test_new();

  rc = lb_throttle_start(throttle);
  fail_if(rc != 0, "Failed to start throttle.");

  rc = lb_throttle_stop(throttle);
  fail_if(rc != 0, "Failed to stop throttle.");

  lb_throttle_delete(throttle);
}
END_TEST

START_TEST(test_throttle_double_start)
{
  int rc;
  struct lb_throttle_t *throttle = lb_throttle_test_new();

  rc = lb_throttle_start(throttle);
  fail_if(rc != 0, "Failed to start throttle.");

  rc = lb_throttle_start(throttle);
  fail_if(rc == 0, "Started throttle when it should have failed.");

  rc = lb_throttle_stop(throttle);
  fail_if(rc != 0, "Failed to start throttle.");

  lb_throttle_delete(throttle);
}
END_TEST

START_TEST(test_throttle_early_stop)
{
  int rc;
  struct lb_throttle_t *throttle = lb_throttle_test_new();
  rc = lb_throttle_stop(throttle);
  fail_if(rc == 0, "Stopped throttle when it should have failed.");

  lb_throttle_delete(throttle);
}
END_TEST

Suite *
suite_throttle_new()
{
  Suite *suite = suite_create("suite_throttle");

  TCase *case_tss = tcase_create("test_throttle_start_stop");
  tcase_add_test(case_tss, test_throttle_std_start_stop);
  tcase_add_test(case_tss, test_throttle_double_start);
  tcase_add_test(case_tss, test_throttle_early_stop);

  suite_add_tcase(suite, case_tss);
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
