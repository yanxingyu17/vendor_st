# vendor_st — Project Status

> **Last updated**: 2026-06-02 (commit `ccdbf2c` on `dev-ai-contest-2026`)

## Architecture

`vendor_st` follows the **board-only vendor pattern** (same as
`vendor_espressif`). All STM32H7A3 chip-family code (RCC, headers,
Kconfig, peripheral drivers, dispatchers) lives in **nuttx in-tree**
under `arch/arm/src/stm32h7/`. This repo only owns NUCLEO board BSPs.

```
                                 +-----------------------------+
                                 |  nuttx in-tree              |
   defconfig                     |  arch/arm/src/stm32h7/      |
   ARCH_CHIP_STM32H7A3ZI=y  -->  |   stm32h7a3xx_rcc.c         |
                                 |   hardware/stm32h7a3xx_*.h  |
                                 |   (shared H7 drivers)       |
                                 +-----------------------------+
                                              ^
                                              | (in-tree chip family)
                                              |
   defconfig                     +-----------------------------+
   ARCH_BOARD_CUSTOM=y       --> |  vendor_st (this repo)      |
   ARCH_BOARD_CUSTOM_DIR=...     |  boards/stm32h7a3/          |
                                 |    nucleo-h7a3zi-q/         |
                                 |      src/ include/          |
                                 |      scripts/ configs/      |
                                 +-----------------------------+
```

## Hardware Verification Status — IMPORTANT

| Item | Status |
|------|--------|
| Build verification (`m2-bsp` produces ARM ELF) | ✅ DONE |
| Build verification (`nucleo-h743zi:nsh` sentinel, no regression) | ✅ DONE |
| Hardware verification of final hybrid `m2-bsp` firmware | 🔴 **BLOCKED** |
| Hardware verification of three real-cause fixes (PWR/FPU/HSE) | 🟡 PARTIAL — fixes individually validated pre-brick; final hybrid wiring NOT yet run on board |
| 6 cmocka driver tests (T1/T3/T4/T5/T6/T7) | 🔴 BLOCKED — see openspec `verify-stm32h7a3-round3-fixes` |
| TIM2 oneshot real-cause investigation | 🔴 BLOCKED — see openspec `add-stm32h7a3-oneshot-real-fix` |

### Why Hardware Is Blocked

The original NUCLEO-H7A3ZI-Q test board went through ~20+ flash cycles
during PWR/FPU/HSE root-cause debugging and ended in an unresponsive
state matching the ST forum mvo case
([community.st.com tid 898267](https://community.st.com/t5/stm32-mcus-products/stm32h7a3lih6q-swd-permanently-unresponsive-after-20-flash/td-p/898267)).
SWD is recoverable (BOOT0+Mass Erase works), but custom firmware no
longer reaches NSH — PC drifts into unmapped flash regions after
Reset_Handler, suggestive of accumulated silicon damage similar to mvo's
1-2 Ω SWD pin short observation.

ST has been informed and asked for a replacement board. Hardware
verification will resume once a healthy board is available.

## Companion Commits in nuttx in-tree

The following nuttx in-tree commits are required for this vendor repo to
build and run correctly. They are NOT in this repo:

| Commit | Subject | Status |
|--------|---------|--------|
| `016f253a0f` | `arch/stm32h7: add STM32H7A3 family + NUCLEO-H7A3ZI-Q board support` | committed in nuttx fork |
| `314c72c2e5` | `arch/stm32h7: fix PWR_CR3 program-once lock on H7A3` | committed |
| `87edc2fbf8` | `arch/stm32h7: enable FPU before clock config to avoid silent HardFault` | committed (generic H7 latent fix) |
| `1f1ab45e3e` | `arch/stm32h7: fix HSE timeout calibration on H7A3 boot path` | committed |

## How to Build

See `docs/zh-cn/quickstart/development_board/NUCLEO-H7A3ZI-Q.md` in the
nuttx tree for the canonical Chinese-language quickstart. Short version:

```bash
cd nuttx
./tools/configure.sh -E ../vendor/st/boards/stm32h7a3/nucleo-h7a3zi-q/configs/m2-bsp
make -j$(nproc)
```

Available configs: `nsh`, `xts`, `m2`, `m2-bsp`.

## OpenSpec Changes Tracking This Work

Inside the nuttx tree at `openspec/changes/`:

- `add-stm32h7a3-board-support` — main change, includes Implementation Result
  block explaining the hybrid architecture pivot
- `verify-stm32h7a3-round3-fixes` — BLOCKED-ON-HARDWARE
- `add-stm32h7a3-oneshot-real-fix` — BLOCKED-ON-HARDWARE
- `spike-h743-bin-on-h7a3-board` — earlier spike, archivable
