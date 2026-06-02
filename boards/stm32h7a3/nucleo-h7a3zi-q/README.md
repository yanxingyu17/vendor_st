# NUCLEO-H7A3ZI-Q Board Support for openvela

[ English | [简体中文](README_zh-cn.md) ]

## Introduction

This directory provides openvela support for the **STMicroelectronics
NUCLEO-H7A3ZI-Q** (MB1363) development board on the
`dev-ai-contest-2026` branch.

The NUCLEO-H7A3ZI-Q hosts a STM32H7A3ZIT6Q MCU (Cortex-M7 @ 280 MHz,
2 MB on-die Flash, 1.4 MB SRAM, single + double precision FPU). It is
the official Nucleo-144 evaluation board for the STM32H7A3 family —
target audience is AIoT edge nodes, low-power HMI, industrial
controllers, and wearable main MCUs (the "low-power, big-on-die-storage"
sweet spot of the H7 line, distinct from H750's "high-perf with external
OCTOSPI" positioning).

For board hardware details, schematics and the official getting-started
guide, see the upstream STMicroelectronics documentation:

- [NUCLEO-H7A3ZI-Q product page](https://www.st.com/en/evaluation-tools/nucleo-h7a3zi-q.html)
- [UM2407 — NUCLEO-H7A3ZI-Q user manual](https://www.st.com/resource/en/user_manual/um2407-stm32h7-nucleo144-boards-mb1363-stmicroelectronics.pdf)
- [RM0455 — STM32H7A3 reference manual](https://www.st.com/resource/en/reference_manual/rm0455-stm32h7a37b3-and-stm32h7b0-value-line-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)

> ⚠️ **Branch dependency**
>
> This board overlay only builds on the `dev-ai-contest-2026` branch of
> `open-vela/nuttx` and `open-vela/vendor_st`. Building it from `trunk`
> or `dev` will fail because the chip-side STM32H7A3 family support
> (4 commits in nuttx in-tree) is not yet upstream.

> ⚠️ **Hybrid architecture (board-only vendor)**
>
> Following the `vendor_espressif` pattern, this vendor only owns
> board-level files. The STM32H7A3 chip family (RCC, headers, dispatcher
> patches, three real-cause silent-killer fixes) lives in nuttx in-tree
> at `arch/arm/src/stm32h7/` because H7A3 is an existing nuttx stm32h7
> family member sharing ~90%+ code with H743/H753/H7B3. See
> [`STATUS.md`](../../../../STATUS.md) at repo root for the architecture
> rationale.

## Directory Structure

```
vendor/st/boards/stm32h7a3/nucleo-h7a3zi-q/
├── Kconfig                 # board-level Kconfig (ROMFS knobs only)
├── CMakeLists.txt          # CMake glue
├── README.md / README_zh-cn.md / README.txt
├── include/board.h         # clock/LED/GPIO board definitions
├── src/                    # board bring-up sources (16 .c files)
│   ├── stm32_boot.c        # early boot (TCM enable, vector table)
│   ├── stm32_bringup.c     # main bring-up (peripherals -> /dev nodes)
│   ├── stm32_appinitialize.c
│   ├── stm32_autoleds.c    # 3 user LEDs (LD1=PB0/LD2=PE1/LD3=PB14)
│   ├── stm32_userleds.c    # /dev/userleds device
│   ├── stm32_buttons.c     # USER button on PC13 -> /dev/buttons
│   ├── stm32_gpio.c        # GPIO chardev
│   ├── stm32_adc.c         # ADC1 bring-up
│   ├── stm32_spi.c         # SPI3 bring-up
│   ├── stm32_pwm.c         # TIM1 PWM bring-up
│   ├── stm32_progmem.c     # flash MTD (/dev/progmem0)
│   ├── stm32_reset.c       # reset cause cache
│   ├── stm32_uid.c         # 96-bit unique ID readback
│   └── stm32_usb.c         # USB OTG-HS as FS (PA11/PA12)
├── configs/                # 4 defconfigs
│   ├── nsh/defconfig       # ~140 KB
│   ├── xts/defconfig       # ~400 KB
│   ├── m2/defconfig        # ~430 KB
│   └── m2-bsp/defconfig    # ~542 KB
└── scripts/
    ├── Make.defs           # toolchain glue (armv7-m + flash.ld)
    ├── flash.ld            # 280 MHz @ VOS0 layout
    └── memory.ld           # 2 MB Flash + 1.4 MB SRAM regions
```

## Supported Peripherals

| Peripheral | Driver | Device node |
|-----------|--------|-------------|
| 3 user LEDs (LD1/LD2/LD3) | `userleds` + `autoleds` | `/dev/userleds` |
| USER button (B1, blue, PC13) | `userbuttons` | `/dev/buttons` |
| USART3 — ST-Link Virtual COM | `stm32h7-serial` | `/dev/console` (ttyS0) |
| USART6 (D1/D0 on Arduino) | `stm32h7-serial` | `/dev/ttyS1` |
| I2C1 (D14/D15 on Arduino) | `stm32h7-i2c` | `/dev/i2c0` |
| I2C2 | `stm32h7-i2c` | `/dev/i2c1` |
| SPI3 | `stm32h7-spi` | `/dev/spi3` |
| ADC1 (16-bit, 5 channels INP5/10/12/13/15) | `stm32h7-adc` | `/dev/adc0` |
| TIM1 PWM (CH1/CH1N) | `stm32h7-pwm` | `/dev/pwm0` |
| RTC (TAMP backup register cache) | `stm32h7-rtc` | `/dev/rtc0` |
| IWDG / WWDG | `stm32h7-iwdg/wwdg` | `/dev/watchdog0` |
| TRNG | `stm32h7-rng` | `/dev/random`, `/dev/urandom` |
| Internal Flash MTD (FLASH_OPT bank-2) | `stm32h7-progmem` | `/dev/progmem0` |
| 96-bit Unique ID | `stm32_uid` | sysinfo |
| Reset cause | `boardctl(BOARDIOC_RESET_CAUSE)` | `procfs` |

> Not supported on this board (silicon does not have it): Ethernet MAC,
> CRYP (hardware AES), HS USB PHY, LTDC LCD-TFT, SDMMC. The H7A3 is the
> "non-Ethernet, low-power" variant of the H7 line.

## GPIO Pin Map (m2-bsp defconfig)

| Function | GPIO | Notes |
|----------|------|-------|
| LD1 (Green) | PB0  | Arduino D13 conflict ⚠ |
| LD2 (Blue)  | PE1  | (H7A3 differs from H743's PB7!) |
| LD3 (Red)   | PB14 | |
| USER button (B1, blue) | PC13 | |
| USART3 TX / RX (VCP) | PD8 / PD9 | ST-Link Virtual COM (115200 8N1) |
| USART6 TX / RX | PG14 / PG9 | Arduino D1 / D0 (when soldered) |
| I2C1 SCL / SDA | PB8 / PB9 | Arduino D15 / D14 |
| I2C2 SCL / SDA | PF1 / PF0 | with SMBA on PF2 |
| SPI3 SCK / MISO / MOSI | PB3 / PB4 / PB5 | |
| ADC1 channels | PA0/PA4/PC0/PC2/PC3 | INP16/INP18/INP10/INP12/INP13 |
| TIM1 PWM CH1 / CH1N | PE9 / PE8 | |
| HSE source | from ST-LINK MCU 8 MHz MCO via bypass | no external crystal |

Pin assignments follow the official STMicroelectronics UM2407 schematic;
see `include/board.h` for the authoritative `GPIO_*` defines.

## Build

The openvela `build.sh` wrapper is the entry point:

```bash
cd openvela
./build.sh \
    $(pwd)/vendor/st/boards/stm32h7a3/nucleo-h7a3zi-q/configs/m2-bsp \
    -j$(nproc)
```

Or via `tools/configure.sh` for menuconfig customisation:

```bash
cd openvela/nuttx
./tools/configure.sh -E ../vendor/st/boards/stm32h7a3/nucleo-h7a3zi-q/configs/m2-bsp
make -j$(nproc)
```

Successful output ends with:

```
LD: nuttx
CP: nuttx.hex
CP: nuttx.bin
arm-none-eabi-objcopy -O binary nuttx nuttx.bin
arm-none-eabi-objcopy -O ihex   nuttx nuttx.hex
```

Artefacts produced under `nuttx/`:

| File | Size | Purpose |
|------|-----:|---------|
| `nuttx`     | ~6.6 MB | ELF (with debug-info), used by GDB |
| `nuttx.bin` | ~541 KB | flat image for OpenOCD / STM32CubeProgrammer |
| `nuttx.hex` | ~1.5 MB | Intel-HEX (optional) |

Image size summary (m2-bsp): `flash 25.80% (541 KB / 2 MB)`,
`sram 4.35% (45 KB / 1 MB)`.

### Available defconfigs

| defconfig | Use case | Approx. size |
|-----------|----------|----:|
| `nsh`     | Minimal NSH (USART3 console only) | 140 KB |
| `xts`     | NSH + xTS Bucket-A1 kernel test suite | 400 KB |
| `m2`      | M2 demo: LED + I2C + SPI + PWM + RTC + Watchdog + ADC | 430 KB |
| `m2-bsp`  | M2 + xTS BSP cmocka_driver tests + RNG + progmem MTD | 542 KB |

## Flash

The NUCLEO board exposes the on-board ST-LINK V3E as both an SWD probe
and a USB CDC virtual COM. Connect a USB-Micro-B cable to **CN1
(USB STLINK)** on the board.

### Option A — OpenOCD (recommended, Linux-friendly)

```bash
sudo apt install openocd     # 0.12+ has STM32H7Ax/Bx support
openocd -f interface/stlink.cfg -f target/stm32h7x.cfg \
    -c "program nuttx/nuttx.bin 0x08000000 verify reset exit"
```

### Option B — STM32CubeProgrammer CLI

```bash
STM32_Programmer_CLI -c port=SWD mode=UR reset=HWrst freq=4000 \
                     -e all \
                     -w nuttx/nuttx.bin 0x08000000 -v
```

### If SWD fails to connect (power-config-deadlock recovery)

If the board ever ends up in a state where ST-LINK reports
`Unable to get core ID` or `No STM32 target found`, follow the ST KB
recovery procedure (BOOT0 jumper to VDD):

1. Power off the board (unplug USB).
2. Use a jumper wire to short `BT0` (CN11 pin 7) to `+3V3` (CN11 pin 6).
3. Power on; the chip enters the system bootloader (DFU) instead of
   user flash.
4. Run STM32CubeProgrammer **Mass Erase** to clear bad supply config.
5. Power off, remove the jumper.
6. Power on; SWD is recoverable; reflash a healthy firmware.

Reference: ST KB article 49579,
[How can I recover my STM32H7/STM32H7RS board after facing a power configuration deadlock](https://community.st.com/t5/stm32-mcus/how-can-i-recover-my-stm32h7-stm32h7rs-board-after-facing-a/ta-p/49579).

## First Boot & Quick Tests

The board enumerates two USB CDC interfaces. The Virtual COM (USART3
NSH console) is usually `/dev/ttyACM0`:

```bash
sudo usermod -a -G dialout $USER   # one-time, requires re-login
picocom -b 115200 /dev/ttyACM0
# or: minicom -D /dev/ttyACM0 -b 115200 -8 -o
```

Press the on-board black **B2 RESET** button. You should see NuttX boot
and `ls /dev` list every peripheral:

```
nsh> uname -a
NuttX  0.0.0 <commit> <date> arm nucleo-h7a3zi-q
nsh> ls /dev
/dev:
 adc0       buttons    console    i2c0       i2c1
 progmem0   pwm0       random     rtc0       spi3
 ttyS0      urandom    userleds   watchdog0
```

Per-peripheral hand-test (all from `nsh>`):

```
i2c bus                  # list I2C buses
i2c dev -b 0 0x03 0x77   # scan I2C1 for slave devices
adc -n 5                 # read 5 ADC1 samples
pwm -d 50 -f 1000        # 50% duty @ 1 kHz on TIM1 CH1
rtc                      # read RTC time
watchdog -t 5000 -p 1000 /dev/watchdog0   # 5s timeout, 1s ping
cat /dev/random | xxd | head     # TRNG output
cat /proc/cpuinfo        # SYSCLK, FPU info
free                     # memory layout
ostest                   # kernel regression suite
```

## Customising the Configuration

```bash
cd openvela/nuttx
make menuconfig          # tweak interactively
make savedefconfig
cp defconfig \
   ../vendor/st/boards/stm32h7a3/nucleo-h7a3zi-q/configs/<your-config>/defconfig
```

To create an additional config variant, copy one of the four `configs/*/`
directories to `configs/<your-name>/` and pass the new path to
`build.sh` or `configure.sh`.

## Debugging

- **Serial logs** — `syslog`/`printf` go to USART3 (PD8/PD9), captured
  via the ST-Link Virtual COM at 115200 8N1.
- **GDB over SWD** — connect with OpenOCD:

  ```bash
  openocd -f interface/stlink.cfg -f target/stm32h7x.cfg
  # in another shell:
  arm-none-eabi-gdb nuttx/nuttx -ex 'target extended-remote :3333'
  ```

- **Crash analysis** — when an assert/panic fires, the H7 in-tree code
  now correctly prints a register dump on the console
  (commit `87edc2fbf8` fixes a latent silent-HardFault bug where panic
  text was lost on **all** H7 boards before clock setup completed).

## Companion nuttx in-tree commits (required)

This board overlay depends on four commits in `open-vela/nuttx@dev-ai-contest-2026`:

| Commit | Subject |
|--------|---------|
| `016f253a0f` | `arch/stm32h7: add STM32H7A3 family + NUCLEO-H7A3ZI-Q board support` |
| `314c72c2e5` | `arch/stm32h7: fix PWR_CR3 program-once lock on H7A3` |
| `87edc2fbf8` | `arch/stm32h7: enable FPU before clock config to avoid silent HardFault` (benefits **all** H7 boards) |
| `1f1ab45e3e` | `arch/stm32h7: fix HSE timeout calibration on H7A3 boot path` |

See [`STATUS.md`](../../../../STATUS.md) at repo root for the full
architecture rationale and verification status.

## Known Limitations

1. **Final hybrid m2-bsp firmware is build-verified, not yet
   hardware-verified end-to-end** on the boss's specific test board —
   the original test board accumulated silicon damage during ~20+ flash
   cycles of PWR/FPU/HSE root-cause debugging and entered an
   unresponsive state matching the [ST forum mvo case `tid 898267`](https://community.st.com/t5/stm32-mcus-products/stm32h7a3lih6q-swd-permanently-unresponsive-after-20-flash/td-p/898267).
   ST has been informed and asked for a replacement. The fixes
   themselves are individually validated against pre-brick hardware
   results (NSH banner, 9 kernel xTS PASS, m2 demo working). Tracked by
   openspec change `open-h7a3-hybrid-hw-verify`.
2. **No Ethernet** — silicon variant does not include the Ethernet MAC.
   For Ethernet-capable H7, use H743 / H747 / H753 instead.
3. **No CRYP / hardware AES** — silicon variant does not include the
   crypto accelerator. Software AES via mbedTLS works.
4. **USB OTG-HS-as-FS only** — the H7A3 has OTG_HS but with the FS PHY
   on PA11/PA12 only (no HS PHY pin). The `m2-bsp` defconfig keeps USB
   disabled; enable manually if needed.
5. **No SDMMC** — silicon variant does not include the SDMMC
   peripheral. SD card support requires SPI-mode driver if needed.
6. **TIM2 oneshot** has a known crash that was investigated but not
   yet fixed; tracked by openspec change `add-stm32h7a3-oneshot-real-fix`
   (BLOCKED-ON-HARDWARE).

## License

All files in this directory are licensed under Apache-2.0 (SPDX
identifier `Apache-2.0`); see individual files for full headers.
