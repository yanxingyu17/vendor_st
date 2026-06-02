/****************************************************************************
 * boards/arm/stm32h7/nucleo-h7a3zi-q/src/stm32_reset.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <nuttx/arch.h>
#include <nuttx/board.h>
#include <sys/boardctl.h>

#include "arm_internal.h"
#include "hardware/stm32_rcc.h"

#ifdef CONFIG_BOARDCTL_RESET

/****************************************************************************
 * Public Functions
 ****************************************************************************/

/****************************************************************************
 * Name: board_reset
 *
 * Description:
 *   Reset board.  Support for this function is required by board-level
 *   logic if CONFIG_BOARDCTL_RESET is selected.
 *
 * Input Parameters:
 *   status - Status information provided with the reset event.  This
 *            meaning of this status information is board-specific.  If not
 *            used by a board, the value zero may be provided in calls to
 *            board_reset().
 *
 * Returned Value:
 *   If this function returns, then it was not possible to power-off the
 *   board due to some constraints.  The return value int this case is a
 *   board-specific reason for the failure to shutdown.
 *
 ****************************************************************************/

int board_reset(int status)
{
  up_systemreset();
  return 0;
}

#endif /* CONFIG_BOARDCTL_RESET */

#ifdef CONFIG_BOARDCTL_RESET_CAUSE

/* Cache decoded reset cause from first read of RCC_RSR. On STM32H7, an
 * internal reset source (IWDG/WWDG/SFT/LPWR) also pulls NRST low, so RSR
 * latches BOTH the real cause AND PINRSTF. After RMVF, the H7 silicon
 * re-samples NRST and re-asserts PINRSTF alone, masking the original
 * cause. We therefore decode RSR exactly once and return the cached
 * value on every subsequent call.
 */

static int g_reset_cause = -1;

static int stm32_decode_reset_cause(uint32_t rsr)
{
  if (rsr & RCC_RSR_IWDG1RSTF)
    {
      return BOARDIOC_RESETCAUSE_SYS_RWDT;
    }

  if (rsr & RCC_RSR_WWDG1RSTF)
    {
      return BOARDIOC_RESETCAUSE_CORE_MWDT;
    }

  if (rsr & RCC_RSR_LPWRRSTF)
    {
      return BOARDIOC_RESETCAUSE_LOWPOWER;
    }

  if (rsr & RCC_RSR_SFTRSTF)
    {
      return BOARDIOC_RESETCAUSE_CPU_SOFT;
    }

  if (rsr & RCC_RSR_PORRSTF)
    {
      return BOARDIOC_RESETCAUSE_SYS_CHIPPOR;
    }

  if (rsr & RCC_RSR_BORRSTF)
    {
      return BOARDIOC_RESETCAUSE_SYS_BOR;
    }

  if (rsr & RCC_RSR_PINRSTF)
    {
      return BOARDIOC_RESETCAUSE_PIN;
    }

  return BOARDIOC_RESETCAUSE_UNKOWN;
}

int board_reset_cause(struct boardioc_reset_cause_s *cause)
{
  if (g_reset_cause < 0)
    {
      uint32_t rsr = getreg32(STM32_RCC_RSR);

      g_reset_cause = stm32_decode_reset_cause(rsr);
      modifyreg32(STM32_RCC_RSR, 0, RCC_RSR_RMVF);
    }

  cause->cause = g_reset_cause;
  return 0;
}
#endif
