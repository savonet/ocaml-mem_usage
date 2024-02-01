(*
 * SPDX-FileCopyrightText: 2022 - 2024 Savonet team
 *
 * SPDX-License-Identifier: MIT
 *)

let () =
  let {
    Mem_usage.total_virtual_memory;
    total_physical_memory;
    total_used_virtual_memory;
    total_used_physical_memory;
    process_virtual_memory;
    process_physical_memory;
  } =
    Mem_usage.info ()
  in
  Printf.printf
    {|
Total virtual memory:    %s
Total physical memory:   %s
Used virtual memory:     %s
Used physical memory:    %s
Process virtual memory:  %s
Process physical memory: %s
|}
    (Mem_usage.prettify_bytes total_virtual_memory)
    (Mem_usage.prettify_bytes total_physical_memory)
    (Mem_usage.prettify_bytes total_used_virtual_memory)
    (Mem_usage.prettify_bytes total_used_physical_memory)
    (Mem_usage.prettify_bytes process_virtual_memory)
    (Mem_usage.prettify_bytes process_physical_memory)
