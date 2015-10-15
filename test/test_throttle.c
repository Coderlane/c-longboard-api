/*
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

START_TEST(test_throttle_set_get_request)
{
  int rc;
  float power;
  struct lb_throttle_t *throttle = lb_throttle_test_new();
  rc = lb_throttle_start(throttle);
  fail_if(rc != 0, "Failed to start throttle.");

  rc = lb_throttle_request_set(throttle, 0.0f);
  fail_if(rc != 0, "Failed to set throttle power ");
  rc = lb_throttle_request_get(throttle, &power);
  fail_if(rc != 0, "Failed to get throttle power.");
  fail_if(power != 0.0f, "Power was not the expected value.");

  rc = lb_throttle_request_set(throttle, 33.3f);
  fail_if(rc != 0, "Failed to set throttle power ");
  rc = lb_throttle_request_get(throttle, &power);
  fail_if(rc != 0, "Failed to get throttle power.");
  fail_if(power != 33.3f, "Power was not the expected value.");

  rc = lb_throttle_request_set(throttle, 100.0f);
  fail_if(rc != 0, "Failed to set throttle power ");
  rc = lb_throttle_request_get(throttle, &power);
  fail_if(rc != 0, "Failed to get throttle power.");
  fail_if(power != 100.0f, "Power was not the expected value.");

  lb_throttle_delete(throttle);
}
END_TEST

START_TEST(test_throttle_set_get_request_timed)
{
  int rc;
  float power;
  struct lb_throttle_t *throttle = lb_throttle_test_new();
  rc = lb_throttle_start(throttle);
  fail_if(rc != 0, "Failed to start throttle.");

  rc = lb_throttle_request_set(throttle, 0.0f);
  fail_if(rc != 0, "Failed to set throttle power ");
  rc = lb_throttle_request_get(throttle, &power);
  fail_if(rc != 0, "Failed to get throttle power.");
  fail_if(power != 0.0f, "Power was not the expected value.");

  rc = lb_throttle_current_get(throttle, &power);
  fail_if(rc != 0, "Failed to get throttle power. %d", rc);
  fail_if(power != 00.0f, "Power was not expected value. "
                          "Power: %f Expected: %f\n", power, 00.0f);

  rc = lb_throttle_request_set(throttle, 100.0f);
  fail_if(rc != 0, "Failed to set throttle power ");
  rc = lb_throttle_request_get(throttle, &power);
  fail_if(rc != 0, "Failed to get throttle power.");
  fail_if(power != 100.0f, "Power was not the expected value.");

  sleep(1);

  rc = lb_throttle_current_get(throttle, &power);
  fail_if(rc != 0, "Failed to get throttle power.");
  fail_if(power != 20.0f, "Power was not expected value. "
                          "Power: %f Expected: %f\n", power, 20.0f);

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

  TCase *case_ts = tcase_create("test_throttle_set");
  tcase_set_timeout(case_ts, 10);
  tcase_add_test(case_ts, test_throttle_set_get_request);
  tcase_add_test(case_ts, test_throttle_set_get_request_timed);

  suite_add_tcase(suite, case_tss);
  suite_add_tcase(suite, case_ts);
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
