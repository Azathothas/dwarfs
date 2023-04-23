#!/usr/bin/env bash

declare -a repl
repl+=("#define FOLLY_EXPERIMENTAL_SYMBOLIZER_ELF_H_\n")
repl+=("#ifdef MUSL\n")
repl+=("#include <sys/types.h>\n")
repl+=("#include <sys/reg.h>\n")
repl+=("#define __ELF_NATIVE_CLASS __WORDSIZE\n")
repl+=("#endif")
sed -i "s|#define FOLLY_EXPERIMENTAL_SYMBOLIZER_ELF_H_|${repl[*]}|" ./folly/folly/experimental/symbolizer/Elf.h

sed -i "s|\(#if defined(__linux__) \&\& FOLLY_HAVE_ELF && FOLLY_HAVE_DWARF\)|\1 \&\& !defined(MUSL)|" \
  ./folly/folly/experimental/symbolizer/detail/Debug.h
