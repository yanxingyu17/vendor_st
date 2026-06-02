/****************************************************************************
 * boards/arm/stm32h7/nucleo-h7a3zi-q/src/stm32_progmem.c
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to you under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/mount.h>
#include <sys/param.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <debug.h>

#include <nuttx/progmem.h>
#include <nuttx/drivers/drivers.h>
#include <nuttx/fs/ioctl.h>
#include <nuttx/kmalloc.h>
#include <nuttx/mtd/mtd.h>

#include <stm32.h>
#include "nucleo-h7a3zi-q.h"
#include <stm32_flash.h>

#ifdef HAVE_PROGMEM_CHARDEV

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define PARTITION_LABEL_LEN         16
#define PROGMEM_DEVPATH_FMT         "/dev/progmem%d"
#define PROGMEM_DEVPATH_LEN         16

#if defined(CONFIG_STM32_PROGMEM_OTA_PARTITION) && !defined(CONFIG_MTD_PARTITION)
#  error "CONFIG_MTD_PARTITION is required when OTA partition is enabled"
#endif

/****************************************************************************
 * Private Types
 ****************************************************************************/

#if defined(CONFIG_STM32_PROGMEM_OTA_PARTITION)

struct ota_partition_s
{
  uint32_t    offset;
  uint32_t    size;
  const char *devpath;
};

#endif

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

#if defined(CONFIG_STM32_PROGMEM_OTA_PARTITION)
static struct mtd_dev_s *progmem_alloc_mtdpart(uint32_t mtd_offset,
                                               uint32_t mtd_size);
static int init_ota_partitions(void);
#endif

/****************************************************************************
 * Private Data
 ****************************************************************************/

static struct mtd_dev_s *g_progmem_mtd;

#if defined(CONFIG_STM32_PROGMEM_OTA_PARTITION)
static const struct ota_partition_s g_ota_partition_table[] =
{
  {
    .offset  = CONFIG_STM32_OTA_PRIMARY_SLOT_OFFSET,
    .size    = CONFIG_STM32_OTA_SLOT_SIZE,
    .devpath = CONFIG_STM32_OTA_PRIMARY_SLOT_DEVPATH
  },
  {
    .offset  = CONFIG_STM32_OTA_SECONDARY_SLOT_OFFSET,
    .size    = CONFIG_STM32_OTA_SLOT_SIZE,
    .devpath = CONFIG_STM32_OTA_SECONDARY_SLOT_DEVPATH
  },
  {
    .offset  = CONFIG_STM32_OTA_SCRATCH_OFFSET,
    .size    = CONFIG_STM32_OTA_SCRATCH_SIZE,
    .devpath = CONFIG_STM32_OTA_SCRATCH_DEVPATH
  }
};
#endif

/****************************************************************************
 * Private Functions
 ****************************************************************************/

#if defined(CONFIG_STM32_PROGMEM_OTA_PARTITION)

static struct mtd_dev_s *progmem_alloc_mtdpart(uint32_t mtd_offset,
                                               uint32_t mtd_size)
{
  uint32_t blocks;
  ssize_t startblock;

  ASSERT((mtd_offset % up_progmem_pagesize(0)) == 0);
  ASSERT((mtd_size % up_progmem_pagesize(0)) == 0);

  finfo("\tMTD offset = 0x%"PRIx32"\n", mtd_offset);
  finfo("\tMTD size = 0x%"PRIx32"\n", mtd_size);

  startblock = up_progmem_getpage(mtd_offset + up_progmem_getaddress(0));
  if (startblock < 0)
    {
      return NULL;
    }

  blocks = mtd_size / up_progmem_pagesize(0);

  return mtd_partition(g_progmem_mtd, startblock, blocks);
}

static int init_ota_partitions(void)
{
  int i;
  struct mtd_dev_s *mtd;
  int ret = 0;
  char path[PARTITION_LABEL_LEN + 1];

  for (i = 0; i < nitems(g_ota_partition_table); ++i)
    {
      const struct ota_partition_s *part = &g_ota_partition_table[i];
      mtd = progmem_alloc_mtdpart(part->offset, part->size);

      strlcpy(path, (char *)part->devpath, PARTITION_LABEL_LEN);

      finfo("INFO: [label]:   %s\n", path);
      finfo("INFO: [offset]:  0x%08" PRIx32 "\n", part->offset);
      finfo("INFO: [size]:    0x%08" PRIx32 "\n", part->size);

      if (!mtd)
        {
          ferr("ERROR: Failed to create MTD partition\n");
          ret = -1;
        }

      ret = register_mtddriver(path, mtd, 0777, NULL);
      if (ret < 0)
        {
          ferr("ERROR: Failed to register MTD @ %s\n", path);
          ret = -1;
        }
    }

  return ret;
}
#endif /* CONFIG_STM32_PROGMEM_OTA_PARTITION */

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int stm32_progmem_init(void)
{
  char devpath[PROGMEM_DEVPATH_LEN];
  int ret;

  g_progmem_mtd = progmem_initialize();
  if (g_progmem_mtd == NULL)
    {
      ferr("ERROR: Failed to get progmem flash MTD\n");
      return -EIO;
    }

  /* Always expose on-chip flash as MTD char device so generic test tools
   * (cmocka_driver_block, mtd_test) can access it without depending on
   * the OTA partition layout. CAUTION: writing this device will erase
   * the running firmware - re-flash after testing.
   */

  snprintf(devpath, sizeof(devpath), PROGMEM_DEVPATH_FMT,
           PROGMEM_MTD_MINOR);

  ret = register_mtddriver(devpath, g_progmem_mtd, 0777, NULL);
  if (ret < 0)
    {
      ferr("ERROR: register_mtddriver(%s) failed: %d\n", devpath, ret);
      return ret;
    }

  finfo("INFO: registered progmem MTD at %s\n", devpath);

#ifdef CONFIG_STM32_PROGMEM_OTA_PARTITION
  ret = init_ota_partitions();
  if (ret < 0)
    {
      ferr("ERROR: Failed to create OTA partition from MTD\n");
      return -EIO;
    }
#endif

  return OK;
}

#endif /* HAVE_PROGMEM_CHARDEV */
