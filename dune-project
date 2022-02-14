(lang dune 2.0)
(version 0.0.1)
(name mem_usage)
(source (github savonet/ocaml-mem_usage))
(license GPL-2.0)
(authors "Romain Beauxis <toots@rastageeks.org>")
(maintainers "The Savonet Team <savonet-users@lists.sourceforge.net>")

(generate_opam_files true)

(package
 (name mem_usage)
 (synopsis "Cross-platform stats about memory usage")
 (depends dune)
)
