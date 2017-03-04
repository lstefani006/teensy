/* ----------------------------------------------------------------------------
 *         SAM Software Package License
 * ----------------------------------------------------------------------------
 * Copyright (c) 2013, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

/**
 * \file
 *
 * Interface for memories configuration on board.
 *
 */

#ifndef BOARD_MEMORIES_H
#define BOARD_MEMORIES_H

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include <stdint.h>

/*----------------------------------------------------------------------------
 *        Functions
 *----------------------------------------------------------------------------*/

/**
 * \brief Configure the board console if any
 */
extern void board_cfg_console(void);

/**
 * \brief Restore all IOs to default state after power-on reset.
 */
extern void board_restore_pio_reset_state(void);

/**
 * \brief Save power by disabling most peripherals
 */
extern void board_save_misc_power(void);

/**
 * \brief Setup TLB for the board
 */
extern void board_setup_tlb(uint32_t *tlb);

/**
 * \brief Configures L2CC for the board
 */
extern void board_cfg_l2cc(void);

/**
 * \brief Configure the MATRIX for DDR
 */
extern void board_cfg_matrix_for_ddr(void);

/**
 * \brief Configure the MATRIX for NAND
 */
extern void board_cfg_matrix_for_nand(void);

/**
 * \brief Configures DDR for the board (calls board_cfg_matrix_for_ddr)
 */
extern void board_cfg_ddram(void);

/**
 * \brief Configures SMC for the board NAND flash (calls
 * board_cfg_matrix_for_nand).
 */
extern void board_cfg_nand_flash(void);

/**
 * \brief Configures a SD/MMC/e.MMC slot of the board
 * \param sdmmc_id  SDMMC peripheral ID (ID_SDMMCx).
 * \note The application shall have enabled the clocks assigned to this SDMMC
 * peripheral.
 * \return true if successful, false if the specified peripheral could not be
 * initialized.
 */
extern bool board_cfg_sdmmc(uint32_t sdmmc_id);

#endif  /* BOARD_MEMORIES_H */
