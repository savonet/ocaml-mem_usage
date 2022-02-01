(*****************************************************************************

  Copyright 2022 Savonet team

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details, fully stated in the COPYING
  file at the root of the liquidsoap distribution.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

 *****************************************************************************)

type t = {
  total_virtual_memory : int;
  total_physical_memory : int;
  total_used_virtual_memory : int;
  total_used_physical_memory : int;
  process_virtual_memory : int;
  process_physical_memory : int;
}

external info : unit -> t = "ocaml_mem_usage_mem_usage"

let byte_units = ["B"; "kB"; "MB"; "GB"; "TB"; "PB"; "EB"; "ZB"; "YB"]
let bibyte_units = ["B"; "kiB"; "MiB"; "GiB"; "TiB"; "PiB"; "EiB"; "ZiB"; "YiB"]

let bit_units =
  ["b"; "kbit"; "Mbit"; "Gbit"; "Tbit"; "Pbit"; "Ebit"; "Zbit"; "Ybit"]

let bibit_units =
  ["b"; "kibit"; "Mibit"; "Gibit"; "Tibit"; "Pibit"; "Eibit"; "Zibit"; "Yibit"]

let prettify_bytes ?(float_printer = Printf.sprintf "%.02f") ?(signed = false)
    ?(bits = false) ?(binary = false) bytes =
  let units =
    match (bits, binary) with
      | true, true -> bibit_units
      | true, false -> bit_units
      | false, true -> bibyte_units
      | false, false -> byte_units
  in

  let prefix, bytes =
    if bytes < 0 then ("-", -bytes) else ((if signed then "+" else ""), bytes)
  in

  let exponent =
    Float.floor
      (if binary then log (float bytes) /. log 1024.
      else log10 (float bytes) /. 3.)
  in
  let unit_index =
    if List.length units - 1 < int_of_float exponent then List.length units - 1
    else int_of_float exponent
  in
  let bytes =
    float bytes /. Float.pow (if binary then 1024. else 1000.) exponent
  in
  Printf.sprintf "%s%s %s" prefix (float_printer bytes)
    (List.nth units unit_index)
