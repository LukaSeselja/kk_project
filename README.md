# LLVM Passes - Compiler Construction Project

This directory contains the implementation of a **switch-lowering** transform
pass for LLVM, developed as part of the Compiler Construction course project at the Faculty of Mathematics, Belgrade

> This repository may also contain other passes implemented by teammates for
> the same course project, each in their own subdirectory. This README
> documents only the `LowerSwitch` pass.

## What the pass does

LLVM's `switch` instruction bundles an arbitrary number of value comparisons
and their jump targets into a single instruction. This pass (`-lower-switch`)
rewrites every `switch` in a function into an equivalent chain of blocks,
each performing a single `icmp eq` comparison followed by a conditional
branch.

## Building

The project is built on top of the LLVM source tree provided for the course
(downloaded and unpacked via the course's setup script into an
`llvm_project/` directory).

1. Copy this pass into the LLVM source tree:

   ```bash
   cp -r LowerSwitch llvm_project/llvm/lib/Transforms/LowerSwitch
   ```

2. Register the new subdirectory so it gets built, by adding the following
   line to the end of `llvm_project/llvm/lib/Transforms/CMakeLists.txt`:

   ```cmake
   add_subdirectory(LowerSwitch)
   ```

3. From `llvm_project/`, rebuild:

   ```bash
   ./make_llvm.sh
   ```

   This produces `lib/LLVMLowerSwitch.so` inside the `build/` directory.

## Running the pass

The pass is registered under the legacy PassManager with the name
`lower-switch`. Run it with `opt`, loading the built plugin:

```bash
cd llvm_project/build
./bin/opt -load lib/LLVMLowerSwitch.so -enable-new-pm=0 -lower-switch <input>.ll -S -o <output>.ll
```

## Tests

The `LowerSwitch/tests/` folder contains three LLVM IR test files:

- **`emptySwitch.ll`** — a switch with no case labels, only a default
  destination; the minimal edge case, verifying it collapses to a single
  unconditional branch.
- **`basicSwitch.ll`** — a standard switch with four cases and an explicit
  default; verifies the general chain-building logic.
- **`twoSwitches.ll`** — two independent switch statements in the same
  function; verifies that multiple switches are each found and
  lowered correctly without interfering with one another.

Run each test with:

```bash
cd llvm_project/build
./bin/opt -load lib/LLVMLowerSwitch.so -enable-new-pm=0 -lower-switch emptySwitch.ll -S -o emptySwitch.lowered.ll
```

and repeat for `basicSwitch.ll` and `twoSwitches.ll`. Inspect the output
`.ll` files — there should be no remaining `switch` instructions, replaced
by equivalent chains of `icmp`/`br` instructions.
