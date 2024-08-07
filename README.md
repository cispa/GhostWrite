# GhostWrite

![Logo of GhostWrite](ghostwrite.svg)

GhostWrite is a security vulnerability that affects the T-Head XuanTie C910, one of the fastest RISC-V chips to date.
GhostWrite breaks all security isolations on affected RISC-V hardware.
Refer to the [GhostWrite website](https://ghostwriteattack.com) for details and the research paper.

## Cite

Please find the most up to date citation directly at the top of [our website](https://ghostwriteattack.com).

## Details & Building

At it's core, GhostWrite exploits a single ill-behaving store instruction on the C910, which illegally uses a physical instead of a virtual address as target.
This PoC uses the `vse128.v`, though other encodings are available as well.
The instruction is embedded in it's assembled form into the code since the instruction is not (yet) supported by compilers.
Instructions preceding the vulnerable instruction set up the vector extension and set up registers correctly.

The PoC tests if GhostWrite works on the machine by resolving the physical address of a virtual one and then writing to that physical address.
Afterwards the caches need to be flushed so that the new value is visible in virtual memory.

### Building

The following should work on the machine itself:
``` sh
gcc -march="rv64gvzve64x" ghostwrite.c -o ghostwrite
```

The Makefile is configured to produce a statically linked binary (using a cross-compiler).
Simply execute `make`.

#### Nix-based building environment

We further ship a Nix flakes based building environment.
Install [Nix](https://nixos.org/download/) on your system and [enable flakes](https://nixos.wiki/wiki/Flakes).
Then just get a development shell and build the PoC:

``` sh
nix develop
make
```
