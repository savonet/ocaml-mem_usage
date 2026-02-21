ocaml-mem_usage
============

![GitHub](https://img.shields.io/github/license/savonet/ocaml-mem_usage)
![GitHub Workflow Status](https://img.shields.io/github/actions/workflow/status/savonet/ocaml-mem_usage/.github/workflows/main.yml?branch=main)
![GitHub release (latest by date)](https://img.shields.io/github/v/release/savonet/ocaml-mem_usage)

A cross-platform OCaml module for reporting memory usage, with a focus on distinguishing **private memory** (allocated by the process itself) from **shared memory** (e.g., shared libraries, memory-mapped files).

Overview
========

The module reports both system-wide and per-process memory statistics:

| Field | Description |
|-------|-------------|
| `total_physical_memory` | Total RAM installed on the system |
| `total_virtual_memory` | Total virtual memory (RAM + swap) |
| `total_used_physical_memory` | RAM currently in use system-wide |
| `total_used_virtual_memory` | Virtual memory in use system-wide |
| `process_virtual_memory` | Virtual address space of the process |
| `process_physical_memory` | Resident set size (RSS) - pages in RAM |
| `process_private_memory` | Private pages unique to this process (USS) |
| `process_swapped_memory` | Process pages swapped to disk |

The key metric is `process_private_memory` which represents memory that would be freed if this process terminates, excluding shared libraries and other shared mappings.

Supported Platforms
===================

| Platform | APIs Used |
|----------|-----------|
| **Linux** | `sysinfo()`, `/proc/self/status`, `/proc/self/smaps_rollup` (4.14+) or `/proc/self/smaps` |
| **macOS** | `sysctl(hw.memsize)`, `host_statistics64()`, `mach_vm_region_recurse()` |
| **Windows** | `GlobalMemoryStatusEx()`, `GetProcessMemoryInfo()` with `PROCESS_MEMORY_COUNTERS_EX2` |
| **FreeBSD** | `sysinfo()` via libsysinfo |

Example
=======

```ocaml
let () =
  let info = Mem_usage.info () in
  Printf.printf "Process private memory: %s\n"
    (Mem_usage.prettify_bytes ~binary:true info.process_private_memory)
```

Output:
```
Process private memory: 1.25 MiB
```

Documentation:
=============

The [API documentation is available here](https://www.liquidsoap.info/ocaml-mem_usage/mem_usage/index.html)

Prerequisites:
==============

- ocaml
- dune
- findlib

See [dune-project](dune-project) file for versions.

Installation:
=============

The preferred installation method is via [opam](http://opam.ocaml.org/):

```
opam install mem_usage
```

This will install the latest release of `mem_usage`. If you wish to install the latest code from this repository, you can do:

```
opam install .
```

From within this repository.

Compilation:
============

```
dune build
```

Author:
=======

This author of this software may be contacted by electronic mail
at the following address: savonet-users@lists.sourceforge.net.
