# Notepad2Text

This code is an update of a 1990s program for converting [Amstrad NC100 Notepad](https://en.wikipedia.org/wiki/Amstrad_NC100) word processor files to RTF written by Maksim Lin, then a student or staff member at Swindon University.

Maksim's code was written in C and targeted Windows machines. I have modified his code very slightly to allow it to be built on a Unix machine: my Raspberry Pi 500 running Linux. I have not tried to compile the updated code under any version of Windows because I'm not able to do so. I'm happy to consider pull requests that re-enable Windows usage if I have broken it. Please target the `develop` branch.

Being something of a code-formatting martinet, I have taken the opportunity to tidy up Maksim's code. I have also added optional plain text output (handy if, like me, you're pasting the result into web page or text editor) and enforced use of the Linux/macOS new line format: ie. `CRLF` to `LF`. It's not hard to hack back if you're a Windows user.

I have released my version, renamed *notepad2text* to align with [*word2text*](https://github.com/smittytone/Word2Text), under the MIT Licence. Maksim's original code contained no licensing info, just the code.

## Compilation

This is very straightforward:

```bash
gcc -o notepad2text np-rtf.c
```

You can now copy the build to any location in your `PATH`, eg.

```bash
sudo mv notepad2text /usr/local/bin
```

## Usage

Copy across a word file (eg. `.npd`) from the NC100 then run:

```bash
notepad2text test.npd
```

The output will be in same directory but named `.rtf`.

If you'd rather just get plain text, add the `--text` flag:

```bash
notepad2text --text test.npd
```

The output will be in same directory but named `.txt`.

Both types of output file use the Linux/macOS new line format.
