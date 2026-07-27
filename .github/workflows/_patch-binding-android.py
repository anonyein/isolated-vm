#!/usr/bin/env python3
"""Patch binding.gyp so isolated-vm can be cross-compiled for Android.

The stock binding.gyp links the addon against whatever
`process.config.target_defaults.libraries` reports on the *host* Node. That is
wrong for an Android cross build: we instead link directly against the prebuilt
Javet Node runtime shared library bundled in the repo under
`libs/<abi>/<so>`. That .so exports the full V8/Node C++ symbol set (verified
via ELF .dynsym: v8::*, node::*, plus the N-API C ABI), so it works as the
link target for isolated-vm's V8-embedding addon.

This script is idempotent: running it twice produces the same file.

Environment:
  IV_ANDROID_ABI  Android ABI dir under libs/ (e.g. arm64-v8a, x86_64)
  IV_ANDROID_SO   basename of the Javet .so to link against
"""

import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
GYP = os.path.join(ROOT, "binding.gyp")

ABI = os.environ.get("IV_ANDROID_ABI", "").strip()
SO = os.environ.get("IV_ANDROID_SO", "").strip()

if not ABI or not SO:
    sys.exit("IV_ANDROID_ABI and IV_ANDROID_SO must be set")

so_path = os.path.join("libs", ABI, SO)
abs_so = os.path.join(ROOT, "libs", ABI, SO)
if not os.path.isfile(abs_so):
    sys.exit(f"Javet .so not found: {abs_so}")

with open(GYP, "r", encoding="utf-8") as f:
    src = f.read()

if "IV_ANDROID_PATCH" in src:
    print("binding.gyp already patched; skipping")
    sys.exit(0)

# 1) Replace the host `process.config` libraries injection (line ~116-118) with
#    a direct link against the bundled Javet .so. Match the whole 'libraries'
#    block for the isolated_vm target regardless of exact whitespace.
host_libs_re = re.compile(
    r"'libraries':\s*\[\s*'<!@\(node -e[^\n]*\)'\s*,?\s*\]",
    re.S,
)

new_libraries = (
    "'libraries': [\n"
    "\t\t\t\t# IV_ANDROID_PATCH: link directly against the bundled Javet Node\n"
    "\t\t\t\t# runtime .so (exports the full V8/Node C++ + N-API symbols).\n"
    f"\t\t\t\t'<(module_root_dir)/{so_path.replace(os.sep, '/')}',\n"
    "\t\t\t]"
)

src, n = host_libs_re.subn(new_libraries, src)
if n != 1:
    sys.exit(f"failed to rewrite host libraries block (matched {n} times)")

# 2) Inject an Android branch into the isolated_vm target's `conditions`.
#    The existing conditions block is:
#        'conditions': [
#            [ 'OS != "win"', { ... nortti ... } ],
#        ],
#    We append an Android-specific entry that forces C++20 / RTTI / exceptions
#    and wires up include dirs + rpath. NDK clang already supplies libc++.
android_cond = (
    "\t\t\t\t[ 'OS == \"android\"', {\n"
    "\t\t\t\t\t'cflags_cc': [ '-std=c++20', '-frtti', '-fexceptions', '-fPIC' ],\n"
    "\t\t\t\t\t'cflags_cc!': [ '-fno-rtti', '-fno-exceptions' ],\n"
    "\t\t\t\t\t'cflags': [ '-fPIC' ],\n"
    "\t\t\t\t\t'defines': [ 'USE_CLOCK_THREAD_CPUTIME_ID', '__ANDROID__' ],\n"
    "\t\t\t\t\t'ldflags': [\n"
    "\t\t\t\t\t\t'-Wl,--no-undefined',\n"
    "\t\t\t\t\t\t\"-Wl,-rpath,'$$ORIGIN'\",\n"
    "\t\t\t\t\t],\n"
    "\t\t\t\t} ],\n"
)

# Anchor: the isolated_vm target's conditions block that contains the nortti
# exclusion. Insert the android entry right after that block opens.
anchor = "\t\t\t'conditions': [\n\t\t\t\t[ 'OS != \"win\"', {"
if anchor not in src:
    sys.exit("could not find isolated_vm conditions anchor")
src = src.replace(anchor, "\t\t\t'conditions': [\n" + android_cond + "\t\t\t\t[ 'OS != \"win\"', {", 1)

with open(GYP, "w", encoding="utf-8") as f:
    f.write(src)

print(f"patched binding.gyp: link -> {so_path}")
