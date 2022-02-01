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

val info : unit -> t

val prettify_bytes :
  ?float_printer:(float -> string) ->
  ?signed:bool ->
  ?bits:bool ->
  ?binary:bool ->
  int ->
  string
