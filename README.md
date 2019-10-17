# Load String Character-by-character in Shellcode

The idea here is to avoid having an explicit string in the shellcode itself,
but instead to insert code that copies characters one at a time into a local
buffer.

## Overview

There are four main components to the example: (1) a template assembly
language function that will be patched to set a buffer to the desired string,
one character at a time; (2) a program that creates the assembly language
patch to be inserted; (3) a script that puts the generated assembly into the
proper place in the template; and (4) a driver program that can be used to
demonstrate the operation of the code. Note that there are actually two
driver program binaries - one that shows the original behavior and another
that shows the modified behavior.

(1) The template assembly language function is just a compiled version of a C
source file, `loadstring-basis.c`. `gcc` is used to compile it to
assembly. This was simply copied to `loadstring-template.s` then a comment
was inserted to indicate where to place the shellcode. Like so:

```
    $ diff -u loadstring-basis.s loadstring-template.s
    --- loadstring-basis.s  2019-10-17 10:51:39.768364220 -0700
    +++ loadstring-template.s       2019-10-17 10:50:48.395927002 -0700
    @@ -49,6 +49,7 @@
            leaq    -32(%rbp), %rdx
            movq    -64(%rbp), %rax
            movq    %rdx, %rdi
    +# SHELLCODE HERE!
            call    *%rax
            .loc 1 10 0
            movq    -8(%rbp), %rcx
```

(2) The program `mkloadstring` takes a single string on the command line and
generates AT&T x86_64 assembly code to write that string to a buffer. It
affects just a single register. The register to use is configurable with `-r
reg` on the command line; it defaults to `%rdi` since that's most often used
for destination buffers.

For example:

```
    $ ./mkloadstring -p 'Howdy!'
            push    %rdi
            movb    $0x48, (%rdi)
            inc     %rdi
            movb    $0x6f, (%rdi)
            inc     %rdi
            movb    $0x77, (%rdi)
            inc     %rdi
            movb    $0x64, (%rdi)
            inc     %rdi
            movb    $0x79, (%rdi)
            inc     %rdi
            movb    $0x21, (%rdi)
            inc     %rdi
            movb    $0x00, (%rdi)
            pop     %rdi
```

This assemble code can then be inserted into existing assembly code template
to allow setting the string.

(3) `Makefile` will put those pieces together in order to generate goal
`loadstring-shellcode.s` by writing everything before the occurrence of the
string `SHELLCODE` in `loadstring-template.s` into `loadstring-shellcode.s`,
then the writing the output of `mkloadstring`, then writing the rest of
`loadstring-template.s`. The resulting `loadstring-shellcode.s` contains the
modified assembly language that will write a different string, one character
at a time, into the buffer at `%rdi`.

(4) The test driver source code is in `loadstring-driver.c`. When linked with
`loadstring-basis.s` in binary `loadstring-driver-basis`, it will simply
print whatever is passed on the command line. When linked with
`loadstring-shellcode.s` into binary `loadstring-driver`, it will print what
was passed to `mkloadstring` instead.

## Example run

```
    $ make
    gcc -o mkloadstring.o -Wall -Werror -g -c mkloadstring.c
    gcc -o mkloadstring -Wall -Werror -g mkloadstring.o
    gcc -o loadstring-basis.s -Wall -Werror -g -S -c loadstring-basis.c
    gcc -o loadstring-driver-basis -Wall -Werror -g loadstring-driver.c loadstring-basis.s
    cat loadstring-template.s | awk -- '/SHELLCODE/ { exit } { print  }' > loadstring-shellcode.s
    ./mkloadstring -p 'Load me, man!' >> loadstring-shellcode.s
    cat loadstring-template.s | awk -- '/SHELLCODE/ { found=1 } { if (found) print  }' >> loadstring-shellcode.s
    gcc -o loadstring-driver -Wall -Werror -g loadstring-driver.c loadstring-shellcode.s

    $ ./loadstring-driver-basis hello
    hello

    $ ./loadstring-driver hello
    Load me, man!
```

