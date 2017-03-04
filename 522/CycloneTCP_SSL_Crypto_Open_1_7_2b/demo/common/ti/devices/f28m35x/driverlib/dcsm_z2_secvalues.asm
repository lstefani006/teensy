;//###########################################################################
;// FILE:    dcsm_z2_sec_values.asm
;// TITLE:   F28M3Xx M3 Zone 2 Security Values
;// 
;// DESCRIPTION:
;//         This file is used to specify security values to
;//         program into the Zone 2 CSM/ECSL password locations and 
;//         GRABSECT/GRABRAM/FLASHEXEONLY locations in Flash at 0x27FFDC 
;//         - 0x27FFF3.       
;//
;//         NOTE: This file is NOT part of the driverlib library, but 
;//               must be added to any projects where Zone 2 DCSM
;//               security values must be programmed (i.e. projects which
;//               program code to Flash)
;//   
;//###########################################################################
;// $TI Release: F28M35x Support Library v206 $ 
;// $Release Date: Thu Mar  5 10:10:30 CST 2015 $ 
;// $Copyright:
;// Copyright (C) 2011-2015 Texas Instruments Incorporated - http://www.ti.com/
;//
;// Redistribution and use in source and binary forms, with or without 
;// modification, are permitted provided that the following conditions 
;// are met:
;// 
;//   Redistributions of source code must retain the above copyright 
;//   notice, this list of conditions and the following disclaimer.
;// 
;//   Redistributions in binary form must reproduce the above copyright
;//   notice, this list of conditions and the following disclaimer in the 
;//   documentation and/or other materials provided with the   
;//   distribution.
;// 
;//   Neither the name of Texas Instruments Incorporated nor the names of
;//   its contributors may be used to endorse or promote products derived
;//   from this software without specific prior written permission.
;// 
;// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
;// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
;// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
;// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
;// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
;// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
;// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
;// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
;// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
;// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
;// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;// $
;//###########################################################################

;//*****************************************************************************
;//! \addtogroup dcsm_api M3 DCSM API Drivers
;//! @{
;//*****************************************************************************

; The "z2secvalues" section contains the actual security values that will be
; linked and programmed into to the CSM password locations (PWL), ECSL password
; locatsion, GRABSECT, GRABRAM, and FLASHEXEONLY locations in flash.  
; All 0xFFFFFFFF's (erased) is the default value for the password locations (PWL).

; It is recommended that all values be left as 0xFFFFFFFF during code
; development.  Passwords of 0xFFFFFFFF do not activate code security and dummy 
; reads of the security registers isrequired to unlock the CSM.  
; When code development is complete, modify the passwords to activate the
; code security module.

      .sect ".z2secvalues"
      .global z2secvalues
z2secvalues:
      .word    0xFFFFFFFF        ;Flash Execute Only
      .word    0xFFFFFFFF        ;GRABRAM
      .word    0xFFFFFFFF        ;GRABSECT
      .word    0xFFFFFFFF        ;ECSLPSWD1    
      .word    0xFFFFFFFF        ;ECSLPSWD0
      .word    0xFFFFFFFF        ;CSMPSWD3
      .word    0xFFFFFFFF        ;CSMPSWD2
      .word    0xFFFFFFFF        ;CSMPSWD1 	  
      .word    0xFFFFFFFF        ;CSMPSWD0
    
;----------------------------------------------------------------------

; For code security operation, all addresses between 0x27FFD0 and
; 0x27FFDB cannot be used as program code or data.  These locations
; must be programmed to 0x00000000 when the code security password locations
; (PWL) are programmed.  If security is not a concern, then these addresses
; can be used for code or data.  

; The section "z2_csm_rsvd" can be used to program these locations to 0x00000000.

      .sect ".z2_csm_rsvd"
      .global z2_csm_rsvd
z2_csm_rsvd:
      .word 0x00000000 ;0x27FFD0
      .word 0x00000000 ;0x27FFD3 
      .word 0x00000000 ;0x27FFD8

;//*****************************************************************************
;// Close the Doxygen group.
;//! @}
;//*****************************************************************************
      


      


