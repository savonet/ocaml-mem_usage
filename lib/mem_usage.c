#include <caml/alloc.h>
#include <caml/fail.h>
#include <caml/memory.h>
#include <caml/threads.h>

#if defined(WIN32)
#include <pdh.h>
#include <psapi.h>
#include <tchar.h>
#include <windows.h>

CAMLprim value ocaml_mem_usage_mem_usage(value unit) {
  CAMLparam0();
  CAMLlocal1(ret);
  MEMORYSTATUSEX mem_info;
  PROCESS_MEMORY_COUNTERS_EX pmc;
  DWORDLONG total_virtual_memory;
  DWORDLONG total_used_virtual_memory;
  DWORDLONG total_physical_memory;
  DWORDLONG total_used_physical_memory;
  SIZE_T process_virtual_memory;
  SIZE_T process_physical_memory;

  caml_release_runtime_system();
  mem_info.dwLength = sizeof(MEMORYSTATUSEX);
  GlobalMemoryStatusEx(&mem_info);
  total_virtual_memory = mem_info.ullTotalPageFile;
  total_used_virtual_memory =
      mem_info.ullTotalPageFile - mem_info.ullAvailPageFile;
  total_physical_memory = mem_info.ullTotalPhys;
  total_used_physical_memory = mem_info.ullTotalPhys - mem_info.ullAvailPhys;

  GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS *)&pmc,
                       sizeof(pmc));
  process_virtual_memory = pmc.PrivateUsage;
  process_physical_memory = pmc.WorkingSetSize;
  caml_acquire_runtime_system();

  ret = caml_alloc_tuple(6);
  Store_field(ret, 0, Val_int(total_virtual_memory));
  Store_field(ret, 1, Val_int(total_physical_memory));
  Store_field(ret, 2, Val_int(total_used_virtual_memory));
  Store_field(ret, 3, Val_int(total_used_physical_memory));
  Store_field(ret, 4, Val_int(process_virtual_memory));
  Store_field(ret, 5, Val_int(process_physical_memory));
  CAMLreturn(ret);
}
#elif defined(__APPLE__)
#include <mach/mach.h>
#include <mach/mach_host.h>
#include <mach/mach_init.h>
#include <mach/mach_types.h>
#include <mach/vm_statistics.h>
#include <stdio.h>
#include <sys/mount.h>
#include <sys/sysctl.h>

CAMLprim value ocaml_mem_usage_mem_usage(value unit) {
  CAMLparam0();
  CAMLlocal1(ret);
  struct statfs stats;
  uint64_t total_virtual_memory, total_physical_memory,
      total_used_physical_memory, total_used_virtual_memory,
      process_physical_memory, process_virtual_memory;
  struct xsw_usage vmem_usage = {0};
  size_t size = sizeof(vmem_usage);
  struct task_basic_info t_info;
  mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
  vm_size_t page_size;
  mach_port_t mach_port;
  mach_msg_type_number_t count;
  vm_statistics64_data_t vm_stats;

  caml_release_runtime_system();
  if (statfs("/", &stats) != 0) {
    fprintf(stderr, "Error while getting free swap space.\n");
    total_virtual_memory = 0;
  } else {
    total_virtual_memory = (uint64_t)stats.f_bsize * stats.f_bfree;
  }

  if (sysctlbyname("vm.swapusage", &vmem_usage, &size, NULL, 0) != 0) {
    fprintf(stderr, "Error while getting swap usage.\n");
    total_used_virtual_memory = 0;
  } else {
    total_used_virtual_memory = vmem_usage.xsu_used;
  }

  if (task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&t_info,
                &t_info_count)) {
    fprintf(stderr,
            "Unable to get virtual memory currently used by the process.\n");
    process_physical_memory = 0;
    process_virtual_memory = 0;
  } else {
    process_physical_memory = t_info.resident_size;
    process_virtual_memory = t_info.virtual_size;
  }

  mach_port = mach_host_self();
  count = sizeof(vm_stats) / sizeof(natural_t);

  if (host_page_size(mach_port, &page_size) != KERN_SUCCESS) {
    fprintf(stderr, "Unable to get host page size.\n");
    total_physical_memory = 0;
    total_used_physical_memory = 0;
  } else {
    if (host_statistics64(mach_port, HOST_VM_INFO, (host_info64_t)&vm_stats,
                          &count) != KERN_SUCCESS) {
      fprintf(stderr, "Unable to get host stats.\n");
      total_physical_memory = 0;
      total_used_physical_memory = 0;
    } else {
      total_physical_memory = vm_stats.free_count * (int64_t)page_size;

      total_used_physical_memory =
          ((int64_t)vm_stats.active_count + (int64_t)vm_stats.inactive_count +
           (int64_t)vm_stats.wire_count) *
          (int64_t)page_size;
    }
  }

  caml_acquire_runtime_system();

  ret = caml_alloc_tuple(6);
  Store_field(ret, 0, Val_int(total_virtual_memory));
  Store_field(ret, 1, Val_int(total_physical_memory));
  Store_field(ret, 2, Val_int(total_used_virtual_memory));
  Store_field(ret, 3, Val_int(total_used_physical_memory));
  Store_field(ret, 4, Val_int(process_virtual_memory));
  Store_field(ret, 5, Val_int(process_physical_memory));
  CAMLreturn(ret);
}
#else
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sysinfo.h>
#include <sys/types.h>

CAMLprim value ocaml_mem_usage_mem_usage(value unit) {
  CAMLparam0();
  CAMLlocal1(ret);
  struct sysinfo memInfo;
  unsigned long total_virtual_memory, total_physical_memory,
      total_used_virtual_memory, total_used_physical_memory;
  int process_virtual_memory, process_physical_memory;
  FILE *file;
  char buffer[1024] = "";

  caml_release_runtime_system();
  sysinfo(&memInfo);
  total_physical_memory = memInfo.totalram;
  total_physical_memory *= memInfo.mem_unit;

  total_virtual_memory = memInfo.totalram;
  total_virtual_memory += memInfo.totalswap;
  total_virtual_memory *= memInfo.mem_unit;

  total_used_virtual_memory = memInfo.totalram - memInfo.freeram;
  total_used_virtual_memory += memInfo.totalswap - memInfo.freeswap;
  total_used_virtual_memory *= memInfo.mem_unit;

  total_used_physical_memory = memInfo.totalram - memInfo.freeram;
  total_used_physical_memory *= memInfo.mem_unit;

  file = fopen("/proc/self/status", "r");
  if (file) {
    while (fscanf(file, " %1023s", buffer) == 1) {
      if (strcmp(buffer, "VmSize:") == 0) {
        if (fscanf(file, " %lld", &process_virtual_memory) != 1)
          process_virtual_memory = 0;
        process_virtual_memory *= 1024;
        continue;
      }

      if (strcmp(buffer, "VmRSS:") == 0) {
        if (fscanf(file, " %lld", &process_physical_memory) != 1)
          process_physical_memory = 0;
        process_physical_memory *= 1024;
        continue;
      }
    }
    fclose(file);
  }
  caml_acquire_runtime_system();

  ret = caml_alloc_tuple(6);
  Store_field(ret, 0, Val_int(total_virtual_memory));
  Store_field(ret, 1, Val_int(total_physical_memory));
  Store_field(ret, 2, Val_int(total_used_virtual_memory));
  Store_field(ret, 3, Val_int(total_used_physical_memory));
  Store_field(ret, 4, Val_int(process_virtual_memory));
  Store_field(ret, 5, Val_int(process_physical_memory));
  CAMLreturn(ret);
}
#endif
