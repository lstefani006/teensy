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

/** \file */

/** \addtogroup isi_module
 * @{
 * \section gmac_usage Usage
 * - ISI_Init: initialize ISI with default parameters
 * - ISI_EnableInterrupt: enable one or more interrupts
 * - ISI_DisableInterrupt: disable one or more interrupts
 * - ISI_Enable: enable isi module
 * - ISI_Disable: disable isi module
 * - ISI_CodecPathFull: enable codec path
 * - ISI_SetFrame: set frame rate
 * - ISI_BytesForOnePixel: return number of byte for one pixel
 * - ISI_StatusRegister: return ISI status register
 * - ISI_Reset: make a software reset
 */
/**@}*/

#ifndef ISI_H
#define ISI_H

/*----------------------------------------------------------------------------
 *        Definition
 *----------------------------------------------------------------------------*/
#define YUV_INPUT          0
#define RGB_INPUT          1
#define GRAYSCALE_INPUT    2
 
#define ONE_BYTE_PER_PIXEL    1
#define TWO_BYTE_PER_PIXEL    2
#define THREE_BYTE_PER_PIXEL  3

#define YUV_INPUT    0
#define RGB_INPUT    1

/*----------------------------------------------------------------------------
 *        Types
 *----------------------------------------------------------------------------*/

/** Frame Buffer Descriptors */
typedef struct 
{
	/** Address of the Current FrameBuffer */
	uint32_t address;
	/** Address of the Control */
	uint32_t control;
	/** Address of the Next FrameBuffer */
	uint32_t next;
}isi_frame_buffer_desc_t;


/** ISI Matrix Color Space Conversion YCrCb to RGB */
typedef struct
{
	/** Color Space Conversion Matrix Coefficient C0*/
	uint8_t c0;
	/** Color Space Conversion Matrix Coefficient C1 */
	uint8_t c1;
	/** Color Space Conversion Matrix Coefficient C2 */
	uint8_t c2;
	/** Color Space Conversion Matrix Coefficient C3 */
	uint8_t c3;
   /** Color Space Conversion Red Chrominance Default Offset */
	uint8_t croff;
	/** Color Space Conversion Blue Chrominance Default Offset */
	uint8_t cboff;
	/** Color Space Conversion Luminance Default Offset */
	uint8_t yoff;
	 /** Color Space Conversion Matrix Coefficient C4 */
	uint16_t c4;
}isi_yuv2rgc_t;

/** ISI Matrix Color Space Conversion RGB to YCrCb */
typedef struct
{
	/** Color Space Conversion Matrix Coefficient C0*/
	uint8_t c0;
	/** Color Space Conversion Matrix Coefficient C1 */
	uint8_t c1;
	/** Color Space Conversion Matrix Coefficient C2 */
	uint8_t c2;
	/** Color Space Conversion Red Component Offset */
	uint8_t roff;
	/** Color Space Conversion Matrix Coefficient C3*/
	uint8_t c3;
	/** Color Space Conversion Matrix Coefficient C4 */
	uint8_t c4;
	/** Color Space Conversion Matrix Coefficient C5 */
	uint8_t c5;
	/** Color Space Conversion Green Component Offset */
	uint8_t goff;
	/** Color Space Conversion Matrix Coefficient C6*/
	uint8_t c6;
	/** Color Space Conversion Matrix Coefficient C7 */
	uint8_t c7;
	/** Color Space Conversion Matrix Coefficient C8 */
	uint8_t c8;
	/** Color Space Conversion Blue Component Offset */
	uint8_t boff;
}isi_rgc2yuv_t;

/*----------------------------------------------------------------------------
 *         Exported functions
 *----------------------------------------------------------------------------*/
extern void isi_enable(void);

extern void isi_disable(void);

extern void isi_dma_channel_enable(uint32_t channel);

extern void isi_dma_channel_disable(uint32_t channel);

extern void isi_dma_preview_channel_enabled(uint8_t enabled);

extern void isi_dma_codec_channel_enabled(uint8_t enabled);

extern void isi_enable_interrupt(uint32_t flag);

extern void isi_disable_interrupt(uint32_t flag);

extern void isi_codec_path_full(void);

extern void isi_codec_request(void);

extern void isi_codec_wait_dma_completed(void);

extern void isi_preview_wait_dma_completed(void);

extern void isi_set_framerate(uint32_t frame);

extern uint8_t isi_bytes_one_pixel(uint8_t bmpRgb);

extern void isi_reset(void);

extern uint32_t isi_get_status(void);

extern void isi_set_blank( uint8_t hBlank, uint8_t vBlank);

extern void isi_set_sensor_size( uint32_t hSize, uint32_t vSize);

extern void isi_rgb_pixel_mapping(uint32_t wRgbPixelMapping);

extern void isi_rgb_swap_mode(uint32_t swapMode);

extern void isi_ycrcb_format(uint32_t wYuvSwapMode);

extern void isi_set_grayscale_mode(uint32_t wPixelFormat);

extern void isi_set_input_stream(uint32_t wStreamMode);

extern void isi_set_preview_size(
	uint32_t hSize, 
	uint32_t vSize);

extern void isi_calc_scaler_factor( void );

extern void isi_set_dma_preview_path(
	uint32_t baseFrameBufDesc, 
	uint32_t dmaCtrl, 
	uint32_t frameBufferStartAddr);

extern void isi_set_dma_codec_path(
	uint32_t baseFrameBufDesc, 
	uint32_t dmaCtrl, 
	uint32_t frameBufferStartAddr);

extern void isi_set_matrix_yuv2rgb (isi_yuv2rgc_t* yuv2rgb);
extern void isi_set_matrix_rgb2yuv (isi_rgc2yuv_t* rgb2yuv);

#endif //#ifndef ISI_H

