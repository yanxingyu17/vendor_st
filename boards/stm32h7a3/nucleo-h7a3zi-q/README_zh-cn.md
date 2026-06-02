# NUCLEO-H7A3ZI-Q 开发板对 openvela 的支持

[ [English](README.md) | 简体中文 ]

## 简介

本目录为 **意法半导体 STMicroelectronics NUCLEO-H7A3ZI-Q**（板号 MB1363）
开发板提供 openvela 支持，基于 `dev-ai-contest-2026` 分支。

NUCLEO-H7A3ZI-Q 搭载 STM32H7A3ZIT6Q 主控（Cortex-M7 @ 280 MHz，2 MB
片上 Flash，1.4 MB SRAM，单 + 双精度 FPU），是 STM32H7A3 系列官方
Nucleo-144 评估板。目标场景为 AIoT 边缘节点、低功耗 HMI、工业控制器、
可穿戴主控（H7 系列里"低功耗 + 大片上存储"的细分点位，与 H750"高性能 +
外挂 OCTOSPI"路线刻意不同）。

开发板硬件细节、原理图、官方上手指南请参考 ST 官方文档：

- [NUCLEO-H7A3ZI-Q 产品页](https://www.st.com/en/evaluation-tools/nucleo-h7a3zi-q.html)
- [UM2407 — NUCLEO-H7A3ZI-Q 用户手册](https://www.st.com/resource/en/user_manual/um2407-stm32h7-nucleo144-boards-mb1363-stmicroelectronics.pdf)
- [RM0455 — STM32H7A3 参考手册](https://www.st.com/resource/en/reference_manual/rm0455-stm32h7a37b3-and-stm32h7b0-value-line-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)

> ⚠️ **分支依赖**
>
> 本板适配仅在 `open-vela/nuttx` 与 `open-vela/vendor_st` 的
> `dev-ai-contest-2026` 分支上可编译。`trunk` 或 `dev` 分支由于尚未
> 合入芯片层支持（nuttx in-tree 4 个 commits），无法编译。

> ⚠️ **混合架构（board-only vendor）**
>
> 参照 `vendor_espressif` 模式，本 vendor 仓只承担板级文件。STM32H7A3
> 芯片家族（RCC、headers、dispatcher 补丁、3 个真因 silent-killer 修复）
> 位于 nuttx in-tree 的 `arch/arm/src/stm32h7/`，因为 H7A3 是 nuttx 已有
> stm32h7 家族成员，与 H743/H753/H7B3 共享约 90%+ 代码。架构选型理由
> 详见仓库根的 [`STATUS.md`](../../../../STATUS.md)。

## 目录结构

```
vendor/st/boards/stm32h7a3/nucleo-h7a3zi-q/
├── Kconfig                 # 板级 Kconfig（仅 ROMFS 挂载点选项）
├── CMakeLists.txt          # CMake 工具链 glue
├── README.md / README_zh-cn.md / README.txt
├── include/board.h         # 时钟 / LED / GPIO 板级定义
├── src/                    # 板级 bring-up 源码（16 个 .c 文件）
│   ├── stm32_boot.c        # 早期 boot（TCM 启用、向量表）
│   ├── stm32_bringup.c     # 主 bring-up（外设挂入 /dev）
│   ├── stm32_appinitialize.c
│   ├── stm32_autoleds.c    # 3 个用户 LED（LD1=PB0/LD2=PE1/LD3=PB14）
│   ├── stm32_userleds.c    # /dev/userleds 设备节点
│   ├── stm32_buttons.c     # USER 按键 PC13 -> /dev/buttons
│   ├── stm32_gpio.c        # GPIO chardev
│   ├── stm32_adc.c         # ADC1 bring-up
│   ├── stm32_spi.c         # SPI3 bring-up
│   ├── stm32_pwm.c         # TIM1 PWM bring-up
│   ├── stm32_progmem.c     # Flash MTD（/dev/progmem0）
│   ├── stm32_reset.c       # 复位原因缓存
│   ├── stm32_uid.c         # 96-bit Unique ID 读取
│   └── stm32_usb.c         # USB OTG-HS-as-FS（PA11/PA12）
├── configs/                # 4 个 defconfig
│   ├── nsh/defconfig       # ~140 KB
│   ├── xts/defconfig       # ~400 KB
│   ├── m2/defconfig        # ~430 KB
│   └── m2-bsp/defconfig    # ~542 KB
└── scripts/
    ├── Make.defs           # 工具链 glue（armv7-m + flash.ld）
    ├── flash.ld            # 280 MHz @ VOS0 layout
    └── memory.ld           # 2 MB Flash + 1.4 MB SRAM 段定义
```

## 支持的外设

| 外设 | 驱动 | 设备节点 |
|------|------|----------|
| 3 个用户 LED（LD1/LD2/LD3） | `userleds` + `autoleds` | `/dev/userleds` |
| USER 按键（B1 蓝色，PC13） | `userbuttons` | `/dev/buttons` |
| USART3 — ST-Link Virtual COM | `stm32h7-serial` | `/dev/console`（ttyS0） |
| USART6（Arduino D1/D0） | `stm32h7-serial` | `/dev/ttyS1` |
| I2C1（Arduino D14/D15） | `stm32h7-i2c` | `/dev/i2c0` |
| I2C2 | `stm32h7-i2c` | `/dev/i2c1` |
| SPI3 | `stm32h7-spi` | `/dev/spi3` |
| ADC1（16-bit，5 通道 INP5/10/12/13/15） | `stm32h7-adc` | `/dev/adc0` |
| TIM1 PWM（CH1/CH1N） | `stm32h7-pwm` | `/dev/pwm0` |
| RTC（带 TAMP 备份寄存器缓存） | `stm32h7-rtc` | `/dev/rtc0` |
| IWDG / WWDG | `stm32h7-iwdg/wwdg` | `/dev/watchdog0` |
| TRNG | `stm32h7-rng` | `/dev/random`、`/dev/urandom` |
| 内部 Flash MTD（FLASH_OPT bank-2） | `stm32h7-progmem` | `/dev/progmem0` |
| 96-bit Unique ID | `stm32_uid` | sysinfo |
| 复位原因 | `boardctl(BOARDIOC_RESET_CAUSE)` | `procfs` |

> 本板硅片不带（不支持）：以太网 MAC、CRYP（硬件 AES）、HS USB PHY、
> LTDC LCD-TFT、SDMMC。H7A3 是 H7 系列里"无以太网、低功耗"的型号变体，
> 需要以太网请用 H743/H747/H753。

## GPIO 引脚映射（m2-bsp defconfig）

| 功能 | GPIO | 备注 |
|------|------|------|
| LD1（绿） | PB0  | Arduino D13 引脚冲突 ⚠ |
| LD2（蓝） | PE1  | （H7A3 与 H743 不同 — H743 是 PB7！） |
| LD3（红） | PB14 | |
| USER 按键（B1 蓝） | PC13 | |
| USART3 TX / RX（VCP） | PD8 / PD9 | ST-Link 虚拟串口（115200 8N1） |
| USART6 TX / RX | PG14 / PG9 | Arduino D1 / D0（如果焊上 morpho 排针） |
| I2C1 SCL / SDA | PB8 / PB9 | Arduino D15 / D14 |
| I2C2 SCL / SDA | PF1 / PF0 | SMBA 在 PF2 |
| SPI3 SCK / MISO / MOSI | PB3 / PB4 / PB5 | |
| ADC1 通道 | PA0/PA4/PC0/PC2/PC3 | INP16/INP18/INP10/INP12/INP13 |
| TIM1 PWM CH1 / CH1N | PE9 / PE8 | |
| HSE 时钟源 | 来自 ST-LINK MCU 8 MHz MCO（bypass 模式） | 板上无晶振 |

引脚分配遵循 ST 官方 UM2407 原理图，权威定义在 `include/board.h`
中的 `GPIO_*` 宏。

## 编译

openvela 仓库的 `build.sh` 脚本是入口：

```bash
cd openvela
./build.sh \
    $(pwd)/vendor/st/boards/stm32h7a3/nucleo-h7a3zi-q/configs/m2-bsp \
    -j$(nproc)
```

或者通过 `tools/configure.sh` 配合 menuconfig 调整：

```bash
cd openvela/nuttx
./tools/configure.sh -E ../vendor/st/boards/stm32h7a3/nucleo-h7a3zi-q/configs/m2-bsp
make -j$(nproc)
```

成功时输出末尾会出现：

```
LD: nuttx
CP: nuttx.hex
CP: nuttx.bin
arm-none-eabi-objcopy -O binary nuttx nuttx.bin
arm-none-eabi-objcopy -O ihex   nuttx nuttx.hex
```

`nuttx/` 目录下生成的产物：

| 文件 | 大小 | 用途 |
|------|----:|------|
| `nuttx`     | 约 6.6 MB | ELF（含调试符号），供 GDB 使用 |
| `nuttx.bin` | 约 541 KB | 平面镜像，OpenOCD / STM32CubeProgrammer 烧录 |
| `nuttx.hex` | 约 1.5 MB | Intel-HEX 格式（可选） |

镜像占用（m2-bsp）：`flash 25.80%（541 KB / 2 MB）`、
`sram 4.35%（45 KB / 1 MB）`。

### 可用 defconfig

| defconfig | 用途 | 估算大小 |
|-----------|------|----:|
| `nsh`     | 最小 NSH（仅 USART3 控制台） | 140 KB |
| `xts`     | NSH + xTS Bucket-A1 内核测试套件 | 400 KB |
| `m2`      | M2 演示：LED + I2C + SPI + PWM + RTC + Watchdog + ADC | 430 KB |
| `m2-bsp`  | M2 + xTS BSP cmocka_driver 测试 + RNG + progmem MTD | 542 KB |

## 烧录

板子板载的 ST-LINK V3E 同时提供 SWD 调试和 USB CDC 虚拟串口。请把
USB Micro-B 连到板上 **CN1（USB STLINK）** 接口。

### 方案 A — OpenOCD（推荐，Linux 友好）

```bash
sudo apt install openocd     # 0.12+ 支持 STM32H7Ax/Bx
openocd -f interface/stlink.cfg -f target/stm32h7x.cfg \
    -c "program nuttx/nuttx.bin 0x08000000 verify reset exit"
```

### 方案 B — STM32CubeProgrammer CLI

```bash
STM32_Programmer_CLI -c port=SWD mode=UR reset=HWrst freq=4000 \
                     -e all \
                     -w nuttx/nuttx.bin 0x08000000 -v
```

### SWD 连不上时的恢复（power-config-deadlock recovery）

如果板子进入 ST-LINK 报 `Unable to get core ID` / `No STM32 target found`
的状态，按 ST 官方 KB 49579 的 BOOT0 恢复流程操作：

1. 拔掉 USB 完全断电。
2. 用一根杜邦线把 `BT0`（CN11 第 7 脚）短接到 `+3V3`（CN11 第 6 脚）。
3. 上电；芯片走系统 bootloader（DFU），不再跑用户 flash。
4. STM32CubeProgrammer 执行 **Mass Erase** 清掉错误的 supply 配置。
5. 断电、撤掉杜邦线。
6. 重新上电；SWD 恢复，可以重新烧录健康固件。

参考：[How can I recover my STM32H7/STM32H7RS board after facing a power configuration deadlock](https://community.st.com/t5/stm32-mcus/how-can-i-recover-my-stm32h7-stm32h7rs-board-after-facing-a/ta-p/49579)（ST KB 49579）。

## 首次启动 & 快速验证

板子枚举两个 USB CDC 接口。USART3 NSH 控制台通常是
`/dev/ttyACM0`：

```bash
sudo usermod -a -G dialout $USER   # 一次性，需要重新登录
picocom -b 115200 /dev/ttyACM0
# 或: minicom -D /dev/ttyACM0 -b 115200 -8 -o
```

按板上黑色 **B2 RESET** 按键，应当看到 NuttX 启动，`ls /dev` 列出
所有外设：

```
nsh> uname -a
NuttX  0.0.0 <commit> <date> arm nucleo-h7a3zi-q
nsh> ls /dev
/dev:
 adc0       buttons    console    i2c0       i2c1
 progmem0   pwm0       random     rtc0       spi3
 ttyS0      urandom    userleds   watchdog0
```

各外设手动验证命令（全部在 `nsh>` 下执行）：

```
i2c bus                  # 列出 I2C 总线
i2c dev -b 0 0x03 0x77   # 扫描 I2C1 上的从设备
adc -n 5                 # 读 5 个 ADC1 采样
pwm -d 50 -f 1000        # TIM1 CH1 输出 50% duty / 1 kHz
rtc                      # 读 RTC 时间
watchdog -t 5000 -p 1000 /dev/watchdog0   # 5s 超时、1s 喂狗
cat /dev/random | xxd | head     # TRNG 输出
cat /proc/cpuinfo        # SYSCLK、FPU 信息
free                     # 内存布局
ostest                   # 内核回归测试套件
```

## 自定义配置

```bash
cd openvela/nuttx
make menuconfig          # 交互式调整
make savedefconfig
cp defconfig \
   ../vendor/st/boards/stm32h7a3/nucleo-h7a3zi-q/configs/<新配置名>/defconfig
```

新建配置变体（例如 `mybuild`）：把 `configs/` 下任一目录复制为
`configs/mybuild/`，然后把新路径传给 `build.sh` 或 `configure.sh`。

## 调试

- **串口日志** —— `syslog`/`printf` 输出到 USART3（PD8/PD9），通过
  ST-Link 虚拟串口在 115200 8N1 抓取。
- **SWD GDB** —— 用 OpenOCD 启动 GDB server：

  ```bash
  openocd -f interface/stlink.cfg -f target/stm32h7x.cfg
  # 另一个终端：
  arm-none-eabi-gdb nuttx/nuttx -ex 'target extended-remote :3333'
  ```

- **崩溃分析** —— assert/panic 触发时，H7 in-tree 代码现在能正常把
  寄存器 dump 打到串口（commit `87edc2fbf8` 修复了一个会让
  **所有** H7 板在 clock 配置完成前丢失 panic 文本的 silent-HardFault
  隐患）。

## 依赖的 nuttx in-tree commits

本板适配依赖 `open-vela/nuttx@dev-ai-contest-2026` 上的 4 个 commits：

| Commit | 标题 |
|--------|------|
| `016f253a0f` | `arch/stm32h7: add STM32H7A3 family + NUCLEO-H7A3ZI-Q board support` |
| `314c72c2e5` | `arch/stm32h7: fix PWR_CR3 program-once lock on H7A3` |
| `87edc2fbf8` | `arch/stm32h7: enable FPU before clock config to avoid silent HardFault`（**所有** H7 板受益） |
| `1f1ab45e3e` | `arch/stm32h7: fix HSE timeout calibration on H7A3 boot path` |

完整的架构选型理由和验证状态详见仓库根的
[`STATUS.md`](../../../../STATUS.md)。

## 已知限制

1. **最终 hybrid m2-bsp 固件已通过 build 验证，但还没在硬件上端到端
   跑通** —— 测试板在 PWR/FPU/HSE 真因调试期间累积了约 20+ 次烧录，
   进入 SWD 能连但用户代码跑飞的状态，匹配
   [ST 论坛 mvo 案例 `tid 898267`](https://community.st.com/t5/stm32-mcus-products/stm32h7a3lih6q-swd-permanently-unresponsive-after-20-flash/td-p/898267)。
   ST 已知悉，正在协调换板。三个 fix 本身已通过 brick 前的硬件结果
   单独验证（NSH banner、9 项 kernel xTS PASS、m2 demo 跑通），
   后续在新板上的端到端验证由 openspec change `open-h7a3-hybrid-hw-verify`
   跟踪。
2. **无以太网** —— 该型号硅片不带以太网 MAC，需要以太网请用
   H743/H747/H753。
3. **无 CRYP / 硬件 AES** —— 该型号硅片不带硬件加密加速器，软件
   AES（mbedTLS）可用。
4. **USB 仅 OTG-HS-as-FS** —— H7A3 有 OTG_HS 但只引出 FS PHY 的
   PA11/PA12（无 HS PHY 引脚）。`m2-bsp` defconfig 默认不开 USB，
   需要时手动 enable。
5. **无 SDMMC** —— 该型号硅片不带 SDMMC 外设，需要 SD 卡只能走
   SPI 模式驱动。
6. **TIM2 oneshot** 有已知 crash，已调查但尚未修复，由 openspec
   change `add-stm32h7a3-oneshot-real-fix`（BLOCKED-ON-HARDWARE）
   跟踪。

## 许可协议

本目录下所有文件均使用 Apache-2.0 协议（SPDX 标识符
`Apache-2.0`）；详见各文件头部声明。
