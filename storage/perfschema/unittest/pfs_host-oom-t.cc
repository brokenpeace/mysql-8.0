/* Copyright (c) 2011, 2017, Oracle and/or its affiliates. All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  51 Franklin Street, Suite 500, Boston, MA 02110-1335 USA */

#include <my_thread.h>
#include <pfs_buffer_container.h>
#include <pfs_global.h>
#include <pfs_host.h>
#include <pfs_instr.h>
#include <pfs_stat.h>
#include <string.h> /* memset */
#include <tap.h>

#include "stub_pfs_global.h"

extern struct PSI_bootstrap PFS_bootstrap;

static void test_oom()
{
  int rc;
  PFS_global_param param;
  PSI_thread_service_t *thread_service;
  PSI_thread_bootstrap *thread_boot;
  PSI_mutex_bootstrap *mutex_boot;
  PSI_rwlock_bootstrap *rwlock_boot;
  PSI_cond_bootstrap *cond_boot;
  PSI_file_bootstrap *file_boot;
  PSI_socket_bootstrap *socket_boot;
  PSI_table_bootstrap *table_boot;
  PSI_mdl_bootstrap *mdl_boot;
  PSI_idle_bootstrap *idle_boot;
  PSI_stage_bootstrap *stage_boot;
  PSI_statement_bootstrap *statement_boot;
  PSI_transaction_bootstrap *transaction_boot;
  PSI_memory_bootstrap *memory_boot;
  PSI_error_bootstrap *error_boot;
  PSI_data_lock_bootstrap *data_lock_boot;

  memset(& param, 0xFF, sizeof(param));
  param.m_enabled= true;
  param.m_mutex_class_sizing= 0;
  param.m_rwlock_class_sizing= 0;
  param.m_cond_class_sizing= 0;
  param.m_thread_class_sizing= 10;
  param.m_table_share_sizing= 0;
  param.m_file_class_sizing= 0;
  param.m_socket_class_sizing= 0;
  param.m_mutex_sizing= 0;
  param.m_rwlock_sizing= 0;
  param.m_cond_sizing= 0;
  param.m_thread_sizing= 1000;
  param.m_table_sizing= 0;
  param.m_file_sizing= 0;
  param.m_file_handle_sizing= 0;
  param.m_socket_sizing= 0;
  param.m_events_waits_history_sizing= 10;
  param.m_events_waits_history_long_sizing= 0;
  param.m_setup_actor_sizing= 0;
  param.m_setup_object_sizing= 0;
  param.m_user_sizing= 0;
  param.m_host_sizing= 1000;
  param.m_account_sizing= 0;
  param.m_stage_class_sizing= 50;
  param.m_events_stages_history_sizing= 0;
  param.m_events_stages_history_long_sizing= 0;
  param.m_statement_class_sizing= 50;
  param.m_events_statements_history_sizing= 0;
  param.m_events_statements_history_long_sizing= 0;
  param.m_events_transactions_history_sizing= 0;
  param.m_events_transactions_history_long_sizing= 0;
  param.m_digest_sizing= 0;
  param.m_session_connect_attrs_sizing= 0;
  param.m_program_sizing= 0;
  param.m_statement_stack_sizing= 0;
  param.m_memory_class_sizing= 10;
  param.m_metadata_lock_sizing= 0;
  param.m_max_digest_length= 0;
  param.m_max_sql_text_length= 0;
  param.m_error_sizing= 0;

  /* Setup */

  stub_alloc_always_fails= false;
  stub_alloc_fails_after_count= 1000;

  pre_initialize_performance_schema();
  rc= initialize_performance_schema(& param, & thread_boot,
                                    & mutex_boot, & rwlock_boot,
                                    & cond_boot, & file_boot, & socket_boot,
                                    & table_boot, & mdl_boot, & idle_boot,
                                    & stage_boot, & statement_boot, & transaction_boot,
                                    & memory_boot, & error_boot, & data_lock_boot);
  ok(rc == 0, "init ok");
  thread_service= (PSI_thread_service_t *)thread_boot->get_interface(PSI_THREAD_VERSION_1);

  PSI_thread_key thread_key_1;
  PSI_thread_info all_thread[]=
  {
    {&thread_key_1, "T-1", 0}
  };
  thread_service->register_thread("test", all_thread, 1);

  PSI_thread *thread_1= thread_service->new_thread(thread_key_1, NULL, 0);
  thread_service->set_thread(thread_1);

  /* Tests */

  int first_fail= 1;
  stub_alloc_fails_after_count= first_fail;
  thread_service->set_thread_account("", 0, "host1", 5);
  ok(global_host_container.m_lost == 1, "oom (host)");

  stub_alloc_fails_after_count= first_fail + 1;
  thread_service->set_thread_account("", 0, "host2", 5);
  ok(global_host_container.m_lost == 2, "oom (host waits)");

  stub_alloc_fails_after_count= first_fail + 2;
  thread_service->set_thread_account("", 0, "host3", 5);
  ok(global_host_container.m_lost == 3, "oom (host stages)");

  stub_alloc_fails_after_count= first_fail + 3;
  thread_service->set_thread_account("", 0, "host4", 5);
  ok(global_host_container.m_lost == 4, "oom (host statements)");

  stub_alloc_fails_after_count= first_fail + 4;
  thread_service->set_thread_account("", 0, "host5", 5);
  ok(global_host_container.m_lost == 5, "oom (host transactions)");

  stub_alloc_fails_after_count= first_fail + 5;
  thread_service->set_thread_account("", 0, "host6", 5);
  ok(global_host_container.m_lost == 6, "oom (host memory)");

  shutdown_performance_schema();
}

static void do_all_tests()
{
  test_oom();
}

int main(int, char **)
{
  plan(7);
  MY_INIT("pfs_host-oom-t");
  do_all_tests();
  return (exit_status());
}

