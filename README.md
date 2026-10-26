# dimentio

Nonce getter/setter (fork of 0x7ff/dimentio) for checkm8-pwned devices:
it reads or sets the device's boot nonce (`com.apple.System.boot-nonce`
in NVRAM) and prints the derived nonce digest (`nonce_d`, up to 32
bytes). The digest is what goes into a saved blob's ApNonce when
restoring/downgrading with a tool like futurerestore.

The device must already expose kernel read/write: the tool tries, in
order, a tfp0 port, `/usr/lib/libkernrw.0.dylib`, `/usr/lib/libkrw.0.dylib`,
and `/dev/kmem` (see `dimentio_init` in `libdimentio.c`). In practice
this means: pwn the device first (e.g. with gaster), or run on a
jailbreak that already provides tfp0.

## Build

```sh
make        # iOS binary (iphoneos, arm64, iOS 10.0 min)
make macos  # macOS binary (arm64)
make clean  # remove the binary
```

`tfp0.plist` holds the entitlements for the iOS binary
(`get-task-allow`, `task_for_pid-allow`, `platform-application`,
`com.apple.private.kernel.get-kext-info`,
`com.apple.private.security.no-container`, and the
`AppleMobileApNonceUserClient` IOKit user-client class); it is used
when signing the iOS build, not by the Makefile.

## Usage

```sh
./dimentio                 # print the current nonce + nonce_d
./dimentio 0x1234567890abcdef   # set the nonce (exactly "0x" + 1..16 hex digits)
```

If setting via the IORegistry NVRAM route fails, the tool falls back
to patching the boot-nonce string directly in kernel memory, then
syncs NVRAM (a temporary key is set and deleted to force the sync).
If that sync step fails, it prints a hint to enter recovery mode
instead, so the nonce still persists.

## Notes

- If `make` fails on a current Xcode, check the `-mios-version-min=10.0`
  flag in the Makefile first: newer toolchains may no longer accept
  iOS 10.0 as a deployment target.
- The offset tables in `pfinder_init_offsets` are keyed on the kernel
  (xnu) version string; an unsupported newer xnu falls back to the
  oldest offsets, which will simply fail to find the kernel structures
  rather than patch the wrong ones.
