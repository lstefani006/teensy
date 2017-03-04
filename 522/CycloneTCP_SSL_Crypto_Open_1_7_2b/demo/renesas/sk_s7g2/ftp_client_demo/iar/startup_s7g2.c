//Dependencies
#include "bsp_irq_cfg.h"
#include "r7fs7g2x.h"

//Forward function declaration
void SystemInit(void);
void __iar_program_start(void);
int __low_level_init(void);

//Default handler
void Default_Handler(void);

//Cortex-M7 core handlers
void Reset_Handler(void);
void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void SVC_Handler(void);
void DebugMon_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

#pragma weak NMI_Handler = Default_Handler
#pragma weak HardFault_Handler = Default_Handler
#pragma weak MemManage_Handler = Default_Handler
#pragma weak BusFault_Handler = Default_Handler
#pragma weak UsageFault_Handler = Default_Handler
#pragma weak SVC_Handler = Default_Handler
#pragma weak DebugMon_Handler = Default_Handler
#pragma weak PendSV_Handler = Default_Handler
#pragma weak SysTick_Handler = Default_Handler

//Peripherals handlers
void PORT_IRQ0_IRQHandler(void);
void PORT_IRQ1_IRQHandler(void);
void PORT_IRQ2_IRQHandler(void);
void PORT_IRQ3_IRQHandler(void);
void PORT_IRQ4_IRQHandler(void);
void PORT_IRQ5_IRQHandler(void);
void PORT_IRQ6_IRQHandler(void);
void PORT_IRQ7_IRQHandler(void);
void PORT_IRQ8_IRQHandler(void);
void PORT_IRQ9_IRQHandler(void);
void PORT_IRQ10_IRQHandler(void);
void PORT_IRQ11_IRQHandler(void);
void PORT_IRQ12_IRQHandler(void);
void PORT_IRQ13_IRQHandler(void);
void PORT_IRQ14_IRQHandler(void);
void PORT_IRQ15_IRQHandler(void);
void DMAC0_INT_IRQHandler(void);
void DMAC1_INT_IRQHandler(void);
void DMAC2_INT_IRQHandler(void);
void DMAC3_INT_IRQHandler(void);
void DMAC4_INT_IRQHandler(void);
void DMAC5_INT_IRQHandler(void);
void DMAC6_INT_IRQHandler(void);
void DMAC7_INT_IRQHandler(void);
void DTC_COMPLETE_IRQHandler(void);
void ICU_SNZCANCEL_IRQHandler(void);
void FCU_FIFERR_IRQHandler(void);
void FCU_FRDYI_IRQHandler(void);
void LVD_LVD1_IRQHandler(void);
void LVD_LVD2_IRQHandler(void);
void MOSC_STOP_IRQHandler(void);
void SYSTEM_SNZREQ_IRQHandler(void);
void AGT0_AGTI_IRQHandler(void);
void AGT0_AGTCMAI_IRQHandler(void);
void AGT0_AGTCMBI_IRQHandler(void);
void AGT1_AGTI_IRQHandler(void);
void AGT1_AGTCMAI_IRQHandler(void);
void AGT1_AGTCMBI_IRQHandler(void);
void IWDT_NMIUNDF_IRQHandler(void);
void WDT_NMIUNDF_IRQHandler(void);
void RTC_ALM_IRQHandler(void);
void RTC_PRD_IRQHandler(void);
void RTC_CUP_IRQHandler(void);
void ADC120_ADI_IRQHandler(void);
void ADC120_GBADI_IRQHandler(void);
void ADC120_CMPAI_IRQHandler(void);
void ADC120_CMPBI_IRQHandler(void);
void ADC120_WCMPM_IRQHandler(void);
void ADC120_WCMPUM_IRQHandler(void);
void ADC121_ADI_IRQHandler(void);
void ADC121_GBADI_IRQHandler(void);
void ADC121_CMPAI_IRQHandler(void);
void ADC121_CMPBI_IRQHandler(void);
void ADC121_WCMPM_IRQHandler(void);
void ADC121_WCMPUM_IRQHandler(void);
void ACMP_HS0_IRQHandler(void);
void ACMP_HS1_IRQHandler(void);
void ACMP_HS2_IRQHandler(void);
void ACMP_HS3_IRQHandler(void);
void ACMP_HS4_IRQHandler(void);
void ACMP_HS5_IRQHandler(void);
void USBFS_D0FIFO_IRQHandler(void);
void USBFS_D1FIFO_IRQHandler(void);
void USBFS_USBI_IRQHandler(void);
void USBFS_USBR_IRQHandler(void);
void IIC0_RXI_IRQHandler(void);
void IIC0_TXI_IRQHandler(void);
void IIC0_TEI_IRQHandler(void);
void IIC0_EEI_IRQHandler(void);
void IIC0_WUI_IRQHandler(void);
void IIC1_RXI_IRQHandler(void);
void IIC1_TXI_IRQHandler(void);
void IIC1_TEI_IRQHandler(void);
void IIC1_EEI_IRQHandler(void);
void IIC2_RXI_IRQHandler(void);
void IIC2_TXI_IRQHandler(void);
void IIC2_TEI_IRQHandler(void);
void IIC2_EEI_IRQHandler(void);
void SSI0_SSITXI_IRQHandler(void);
void SSI0_SSIRXI_IRQHandler(void);
void SSI0_SSIF_IRQHandler(void);
void SSI1_SSIRT_IRQHandler(void);
void SSI1_SSIF_IRQHandler(void);
void SRC_IDEI_IRQHandler(void);
void SRC_ODFI_IRQHandler(void);
void SRC_OVFI_IRQHandler(void);
void SRC_UDFI_IRQHandler(void);
void SRC_CEFI_IRQHandler(void);
void PDC_PCDFI_IRQHandler(void);
void PDC_PCFEI_IRQHandler(void);
void PDC_PCERI_IRQHandler(void);
void CTSU_CTSUWR_IRQHandler(void);
void CTSU_CTSURD_IRQHandler(void);
void CTSU_CTSUFN_IRQHandler(void);
void KEY_INTKR_IRQHandler(void);
void DOC_DOPCI_IRQHandler(void);
void CAC_FERRI_IRQHandler(void);
void CAC_MENDI_IRQHandler(void);
void CAC_OVFI_IRQHandler(void);
void CAN0_ERS_IRQHandler(void);
void CAN0_RXF_IRQHandler(void);
void CAN0_TXF_IRQHandler(void);
void CAN0_RXM_IRQHandler(void);
void CAN0_TXM_IRQHandler(void);
void CAN1_ERS_IRQHandler(void);
void CAN1_RXF_IRQHandler(void);
void CAN1_TXF_IRQHandler(void);
void CAN1_RXM_IRQHandler(void);
void CAN1_TXM_IRQHandler(void);
void IOPORT_GROUP1_IRQHandler(void);
void IOPORT_GROUP2_IRQHandler(void);
void IOPORT_GROUP3_IRQHandler(void);
void IOPORT_GROUP4_IRQHandler(void);
void ELC_SWEVT0_IRQHandler(void);
void ELC_SWEVT1_IRQHandler(void);
void POEG_GROUP0_IRQHandler(void);
void POEG_GROUP1_IRQHandler(void);
void POEG_GROUP2_IRQHandler(void);
void POEG_GROUP3_IRQHandler(void);
void GPT0_CCMPA_IRQHandler(void);
void GPT0_CCMPB_IRQHandler(void);
void GPT0_CMPC_IRQHandler(void);
void GPT0_CMPD_IRQHandler(void);
void GPT0_CMPE_IRQHandler(void);
void GPT0_CMPF_IRQHandler(void);
void GPT0_OVF_IRQHandler(void);
void GPT0_UDF_IRQHandler(void);
void GPT0_ADTRGA_IRQHandler(void);
void GPT0_ADTRGB_IRQHandler(void);
void GPT1_CCMPA_IRQHandler(void);
void GPT1_CCMPB_IRQHandler(void);
void GPT1_CMPC_IRQHandler(void);
void GPT1_CMPD_IRQHandler(void);
void GPT1_CMPE_IRQHandler(void);
void GPT1_CMPF_IRQHandler(void);
void GPT1_OVF_IRQHandler(void);
void GPT1_UDF_IRQHandler(void);
void GPT1_ADTRGA_IRQHandler(void);
void GPT1_ADTRGB_IRQHandler(void);
void GPT2_CCMPA_IRQHandler(void);
void GPT2_CCMPB_IRQHandler(void);
void GPT2_CMPC_IRQHandler(void);
void GPT2_CMPD_IRQHandler(void);
void GPT2_CMPE_IRQHandler(void);
void GPT2_CMPF_IRQHandler(void);
void GPT2_OVF_IRQHandler(void);
void GPT2_UDF_IRQHandler(void);
void GPT2_ADTRGA_IRQHandler(void);
void GPT2_ADTRGB_IRQHandler(void);
void GPT3_CCMPA_IRQHandler(void);
void GPT3_CCMPB_IRQHandler(void);
void GPT3_CMPC_IRQHandler(void);
void GPT3_CMPD_IRQHandler(void);
void GPT3_CMPE_IRQHandler(void);
void GPT3_CMPF_IRQHandler(void);
void GPT3_OVF_IRQHandler(void);
void GPT3_UDF_IRQHandler(void);
void GPT3_ADTRGA_IRQHandler(void);
void GPT3_ADTRGB_IRQHandler(void);
void GPT4_CCMPA_IRQHandler(void);
void GPT4_CCMPB_IRQHandler(void);
void GPT4_CMPC_IRQHandler(void);
void GPT4_CMPD_IRQHandler(void);
void GPT4_CMPE_IRQHandler(void);
void GPT4_CMPF_IRQHandler(void);
void GPT4_OVF_IRQHandler(void);
void GPT4_UDF_IRQHandler(void);
void GPT4_ADTRGA_IRQHandler(void);
void GPT4_ADTRGB_IRQHandler(void);
void GPT5_CCMPA_IRQHandler(void);
void GPT5_CCMPB_IRQHandler(void);
void GPT5_CMPC_IRQHandler(void);
void GPT5_CMPD_IRQHandler(void);
void GPT5_CMPE_IRQHandler(void);
void GPT5_CMPF_IRQHandler(void);
void GPT5_OVF_IRQHandler(void);
void GPT5_UDF_IRQHandler(void);
void GPT5_ADTRGA_IRQHandler(void);
void GPT5_ADTRGB_IRQHandler(void);
void GPT6_CCMPA_IRQHandler(void);
void GPT6_CCMPB_IRQHandler(void);
void GPT6_CMPC_IRQHandler(void);
void GPT6_CMPD_IRQHandler(void);
void GPT6_CMPE_IRQHandler(void);
void GPT6_CMPF_IRQHandler(void);
void GPT6_OVF_IRQHandler(void);
void GPT6_UDF_IRQHandler(void);
void GPT6_ADTRGA_IRQHandler(void);
void GPT6_ADTRGB_IRQHandler(void);
void GPT7_CCMPA_IRQHandler(void);
void GPT7_CCMPB_IRQHandler(void);
void GPT7_CMPC_IRQHandler(void);
void GPT7_CMPD_IRQHandler(void);
void GPT7_CMPE_IRQHandler(void);
void GPT7_CMPF_IRQHandler(void);
void GPT7_OVF_IRQHandler(void);
void GPT7_UDF_IRQHandler(void);
void GPT7_ADTRGA_IRQHandler(void);
void GPT7_ADTRGB_IRQHandler(void);
void GPT8_CCMPA_IRQHandler(void);
void GPT8_CCMPB_IRQHandler(void);
void GPT8_CMPC_IRQHandler(void);
void GPT8_CMPD_IRQHandler(void);
void GPT8_CMPE_IRQHandler(void);
void GPT8_CMPF_IRQHandler(void);
void GPT8_OVF_IRQHandler(void);
void GPT8_UDF_IRQHandler(void);
void GPT9_CCMPA_IRQHandler(void);
void GPT9_CCMPB_IRQHandler(void);
void GPT9_CMPC_IRQHandler(void);
void GPT9_CMPD_IRQHandler(void);
void GPT9_CMPE_IRQHandler(void);
void GPT9_CMPF_IRQHandler(void);
void GPT9_OVF_IRQHandler(void);
void GPT9_UDF_IRQHandler(void);
void GPT10_CCMPA_IRQHandler(void);
void GPT10_CCMPB_IRQHandler(void);
void GPT10_CMPC_IRQHandler(void);
void GPT10_CMPD_IRQHandler(void);
void GPT10_CMPE_IRQHandler(void);
void GPT10_CMPF_IRQHandler(void);
void GPT10_OVF_IRQHandler(void);
void GPT10_UDF_IRQHandler(void);
void GPT11_CCMPA_IRQHandler(void);
void GPT11_CCMPB_IRQHandler(void);
void GPT11_CMPC_IRQHandler(void);
void GPT11_CMPD_IRQHandler(void);
void GPT11_CMPE_IRQHandler(void);
void GPT11_CMPF_IRQHandler(void);
void GPT11_OVF_IRQHandler(void);
void GPT11_UDF_IRQHandler(void);
void GPT12_CCMPA_IRQHandler(void);
void GPT12_CCMPB_IRQHandler(void);
void GPT12_CMPC_IRQHandler(void);
void GPT12_CMPD_IRQHandler(void);
void GPT12_CMPE_IRQHandler(void);
void GPT12_CMPF_IRQHandler(void);
void GPT12_OVF_IRQHandler(void);
void GPT12_UDF_IRQHandler(void);
void GPT13_CCMPA_IRQHandler(void);
void GPT13_CCMPB_IRQHandler(void);
void GPT13_CMPC_IRQHandler(void);
void GPT13_CMPD_IRQHandler(void);
void GPT13_CMPE_IRQHandler(void);
void GPT13_CMPF_IRQHandler(void);
void GPT13_OVF_IRQHandler(void);
void GPT13_UDF_IRQHandler(void);
void GPT_UVWEDGE_IRQHandler(void);
void ETHER_IPLS_IRQHandler(void);
void ETHER_MINT_IRQHandler(void);
void ETHER_PINT_IRQHandler(void);
void ETHER_EINT0_IRQHandler(void);
void ETHER_EINT1_IRQHandler(void);
void USBHS_D0FIFO_IRQHandler(void);
void USBHS_D1FIFO_IRQHandler(void);
void USBHS_USBIR_IRQHandler(void);
void SCI0_RXI_IRQHandler(void);
void SCI0_TXI_IRQHandler(void);
void SCI0_TEI_IRQHandler(void);
void SCI0_ERI_IRQHandler(void);
void SCI0_AM_IRQHandler(void);
void SCI0_RXI_OR_ERI_IRQHandler(void);
void SCI1_RXI_IRQHandler(void);
void SCI1_TXI_IRQHandler(void);
void SCI1_TEI_IRQHandler(void);
void SCI1_ERI_IRQHandler(void);
void SCI1_AM_IRQHandler(void);
void SCI2_RXI_IRQHandler(void);
void SCI2_TXI_IRQHandler(void);
void SCI2_TEI_IRQHandler(void);
void SCI2_ERI_IRQHandler(void);
void SCI2_AM_IRQHandler(void);
void SCI3_RXI_IRQHandler(void);
void SCI3_TXI_IRQHandler(void);
void SCI3_TEI_IRQHandler(void);
void SCI3_ERI_IRQHandler(void);
void SCI3_AM_IRQHandler(void);
void SCI4_RXI_IRQHandler(void);
void SCI4_TXI_IRQHandler(void);
void SCI4_TEI_IRQHandler(void);
void SCI4_ERI_IRQHandler(void);
void SCI4_AM_IRQHandler(void);
void SCI5_RXI_IRQHandler(void);
void SCI5_TXI_IRQHandler(void);
void SCI5_TEI_IRQHandler(void);
void SCI5_ERI_IRQHandler(void);
void SCI5_AM_IRQHandler(void);
void SCI6_RXI_IRQHandler(void);
void SCI6_TXI_IRQHandler(void);
void SCI6_TEI_IRQHandler(void);
void SCI6_ERI_IRQHandler(void);
void SCI6_AM_IRQHandler(void);
void SCI7_RXI_IRQHandler(void);
void SCI7_TXI_IRQHandler(void);
void SCI7_TEI_IRQHandler(void);
void SCI7_ERI_IRQHandler(void);
void SCI7_AM_IRQHandler(void);
void SCI8_RXI_IRQHandler(void);
void SCI8_TXI_IRQHandler(void);
void SCI8_TEI_IRQHandler(void);
void SCI8_ERI_IRQHandler(void);
void SCI8_AM_IRQHandler(void);
void SCI9_RXI_IRQHandler(void);
void SCI9_TXI_IRQHandler(void);
void SCI9_TEI_IRQHandler(void);
void SCI9_ERI_IRQHandler(void);
void SCI9_AM_IRQHandler(void);
void SPI0_SPRI_IRQHandler(void);
void SPI0_SPTI_IRQHandler(void);
void SPI0_SPII_IRQHandler(void);
void SPI0_SPEI_IRQHandler(void);
void SPI0_SPTEND_IRQHandler(void);
void SPI1_SPRI_IRQHandler(void);
void SPI1_SPTI_IRQHandler(void);
void SPI1_SPII_IRQHandler(void);
void SPI1_SPEI_IRQHandler(void);
void SPI1_SPTEND_IRQHandler(void);
void QSPI_INTR_IRQHandler(void);
void SDHI_MMC0_ACCS_IRQHandler(void);
void SDHI_MMC0_SDIO_IRQHandler(void);
void SDHI_MMC0_CARD_IRQHandler(void);
void SDHI_MMC0_ODMSDBREQ_IRQHandler(void);
void SDHI_MMC1_ACCS_IRQHandler(void);
void SDHI_MMC1_SDIO_IRQHandler(void);
void SDHI_MMC1_CARD_IRQHandler(void);
void SDHI_MMC1_ODMSDBREQ_IRQHandler(void);
void GLCDC_VPOS_IRQHandler(void);
void GLCDC_L1UNDF_IRQHandler(void);
void GLCDC_L2UNDF_IRQHandler(void);
void DRW_IRQ_IRQHandler(void);
void JPEG_JEDI_IRQHandler(void);
void JPEG_JDTI_IRQHandler(void);

#pragma weak PORT_IRQ0_IRQHandler = Default_Handler
#pragma weak PORT_IRQ1_IRQHandler = Default_Handler
#pragma weak PORT_IRQ2_IRQHandler = Default_Handler
#pragma weak PORT_IRQ3_IRQHandler = Default_Handler
#pragma weak PORT_IRQ4_IRQHandler = Default_Handler
#pragma weak PORT_IRQ5_IRQHandler = Default_Handler
#pragma weak PORT_IRQ6_IRQHandler = Default_Handler
#pragma weak PORT_IRQ7_IRQHandler = Default_Handler
#pragma weak PORT_IRQ8_IRQHandler = Default_Handler
#pragma weak PORT_IRQ9_IRQHandler = Default_Handler
#pragma weak PORT_IRQ10_IRQHandler = Default_Handler
#pragma weak PORT_IRQ11_IRQHandler = Default_Handler
#pragma weak PORT_IRQ12_IRQHandler = Default_Handler
#pragma weak PORT_IRQ13_IRQHandler = Default_Handler
#pragma weak PORT_IRQ14_IRQHandler = Default_Handler
#pragma weak PORT_IRQ15_IRQHandler = Default_Handler
#pragma weak DMAC0_INT_IRQHandler = Default_Handler
#pragma weak DMAC1_INT_IRQHandler = Default_Handler
#pragma weak DMAC2_INT_IRQHandler = Default_Handler
#pragma weak DMAC3_INT_IRQHandler = Default_Handler
#pragma weak DMAC4_INT_IRQHandler = Default_Handler
#pragma weak DMAC5_INT_IRQHandler = Default_Handler
#pragma weak DMAC6_INT_IRQHandler = Default_Handler
#pragma weak DMAC7_INT_IRQHandler = Default_Handler
#pragma weak DTC_COMPLETE_IRQHandler = Default_Handler
#pragma weak ICU_SNZCANCEL_IRQHandler = Default_Handler
#pragma weak FCU_FIFERR_IRQHandler = Default_Handler
#pragma weak FCU_FRDYI_IRQHandler = Default_Handler
#pragma weak LVD_LVD1_IRQHandler = Default_Handler
#pragma weak LVD_LVD2_IRQHandler = Default_Handler
#pragma weak MOSC_STOP_IRQHandler = Default_Handler
#pragma weak SYSTEM_SNZREQ_IRQHandler = Default_Handler
#pragma weak AGT0_AGTI_IRQHandler = Default_Handler
#pragma weak AGT0_AGTCMAI_IRQHandler = Default_Handler
#pragma weak AGT0_AGTCMBI_IRQHandler = Default_Handler
#pragma weak AGT1_AGTI_IRQHandler = Default_Handler
#pragma weak AGT1_AGTCMAI_IRQHandler = Default_Handler
#pragma weak AGT1_AGTCMBI_IRQHandler = Default_Handler
#pragma weak IWDT_NMIUNDF_IRQHandler = Default_Handler
#pragma weak WDT_NMIUNDF_IRQHandler = Default_Handler
#pragma weak RTC_ALM_IRQHandler = Default_Handler
#pragma weak RTC_PRD_IRQHandler = Default_Handler
#pragma weak RTC_CUP_IRQHandler = Default_Handler
#pragma weak ADC120_ADI_IRQHandler = Default_Handler
#pragma weak ADC120_GBADI_IRQHandler = Default_Handler
#pragma weak ADC120_CMPAI_IRQHandler = Default_Handler
#pragma weak ADC120_CMPBI_IRQHandler = Default_Handler
#pragma weak ADC120_WCMPM_IRQHandler = Default_Handler
#pragma weak ADC120_WCMPUM_IRQHandler = Default_Handler
#pragma weak ADC121_ADI_IRQHandler = Default_Handler
#pragma weak ADC121_GBADI_IRQHandler = Default_Handler
#pragma weak ADC121_CMPAI_IRQHandler = Default_Handler
#pragma weak ADC121_CMPBI_IRQHandler = Default_Handler
#pragma weak ADC121_WCMPM_IRQHandler = Default_Handler
#pragma weak ADC121_WCMPUM_IRQHandler = Default_Handler
#pragma weak ACMP_HS0_IRQHandler = Default_Handler
#pragma weak ACMP_HS1_IRQHandler = Default_Handler
#pragma weak ACMP_HS2_IRQHandler = Default_Handler
#pragma weak ACMP_HS3_IRQHandler = Default_Handler
#pragma weak ACMP_HS4_IRQHandler = Default_Handler
#pragma weak ACMP_HS5_IRQHandler = Default_Handler
#pragma weak USBFS_D0FIFO_IRQHandler = Default_Handler
#pragma weak USBFS_D1FIFO_IRQHandler = Default_Handler
#pragma weak USBFS_USBI_IRQHandler = Default_Handler
#pragma weak USBFS_USBR_IRQHandler = Default_Handler
#pragma weak IIC0_RXI_IRQHandler = Default_Handler
#pragma weak IIC0_TXI_IRQHandler = Default_Handler
#pragma weak IIC0_TEI_IRQHandler = Default_Handler
#pragma weak IIC0_EEI_IRQHandler = Default_Handler
#pragma weak IIC0_WUI_IRQHandler = Default_Handler
#pragma weak IIC1_RXI_IRQHandler = Default_Handler
#pragma weak IIC1_TXI_IRQHandler = Default_Handler
#pragma weak IIC1_TEI_IRQHandler = Default_Handler
#pragma weak IIC1_EEI_IRQHandler = Default_Handler
#pragma weak IIC2_RXI_IRQHandler = Default_Handler
#pragma weak IIC2_TXI_IRQHandler = Default_Handler
#pragma weak IIC2_TEI_IRQHandler = Default_Handler
#pragma weak IIC2_EEI_IRQHandler = Default_Handler
#pragma weak SSI0_SSITXI_IRQHandler = Default_Handler
#pragma weak SSI0_SSIRXI_IRQHandler = Default_Handler
#pragma weak SSI0_SSIF_IRQHandler = Default_Handler
#pragma weak SSI1_SSIRT_IRQHandler = Default_Handler
#pragma weak SSI1_SSIF_IRQHandler = Default_Handler
#pragma weak SRC_IDEI_IRQHandler = Default_Handler
#pragma weak SRC_ODFI_IRQHandler = Default_Handler
#pragma weak SRC_OVFI_IRQHandler = Default_Handler
#pragma weak SRC_UDFI_IRQHandler = Default_Handler
#pragma weak SRC_CEFI_IRQHandler = Default_Handler
#pragma weak PDC_PCDFI_IRQHandler = Default_Handler
#pragma weak PDC_PCFEI_IRQHandler = Default_Handler
#pragma weak PDC_PCERI_IRQHandler = Default_Handler
#pragma weak CTSU_CTSUWR_IRQHandler = Default_Handler
#pragma weak CTSU_CTSURD_IRQHandler = Default_Handler
#pragma weak CTSU_CTSUFN_IRQHandler = Default_Handler
#pragma weak KEY_INTKR_IRQHandler = Default_Handler
#pragma weak DOC_DOPCI_IRQHandler = Default_Handler
#pragma weak CAC_FERRI_IRQHandler = Default_Handler
#pragma weak CAC_MENDI_IRQHandler = Default_Handler
#pragma weak CAC_OVFI_IRQHandler = Default_Handler
#pragma weak CAN0_ERS_IRQHandler = Default_Handler
#pragma weak CAN0_RXF_IRQHandler = Default_Handler
#pragma weak CAN0_TXF_IRQHandler = Default_Handler
#pragma weak CAN0_RXM_IRQHandler = Default_Handler
#pragma weak CAN0_TXM_IRQHandler = Default_Handler
#pragma weak CAN1_ERS_IRQHandler = Default_Handler
#pragma weak CAN1_RXF_IRQHandler = Default_Handler
#pragma weak CAN1_TXF_IRQHandler = Default_Handler
#pragma weak CAN1_RXM_IRQHandler = Default_Handler
#pragma weak CAN1_TXM_IRQHandler = Default_Handler
#pragma weak IOPORT_GROUP1_IRQHandler = Default_Handler
#pragma weak IOPORT_GROUP2_IRQHandler = Default_Handler
#pragma weak IOPORT_GROUP3_IRQHandler = Default_Handler
#pragma weak IOPORT_GROUP4_IRQHandler = Default_Handler
#pragma weak ELC_SWEVT0_IRQHandler = Default_Handler
#pragma weak ELC_SWEVT1_IRQHandler = Default_Handler
#pragma weak POEG_GROUP0_IRQHandler = Default_Handler
#pragma weak POEG_GROUP1_IRQHandler = Default_Handler
#pragma weak POEG_GROUP2_IRQHandler = Default_Handler
#pragma weak POEG_GROUP3_IRQHandler = Default_Handler
#pragma weak GPT0_CCMPA_IRQHandler = Default_Handler
#pragma weak GPT0_CCMPB_IRQHandler = Default_Handler
#pragma weak GPT0_CMPC_IRQHandler = Default_Handler
#pragma weak GPT0_CMPD_IRQHandler = Default_Handler
#pragma weak GPT0_CMPE_IRQHandler = Default_Handler
#pragma weak GPT0_CMPF_IRQHandler = Default_Handler
#pragma weak GPT0_OVF_IRQHandler = Default_Handler
#pragma weak GPT0_UDF_IRQHandler = Default_Handler
#pragma weak GPT0_ADTRGA_IRQHandler = Default_Handler
#pragma weak GPT0_ADTRGB_IRQHandler = Default_Handler
#pragma weak GPT1_CCMPA_IRQHandler = Default_Handler
#pragma weak GPT1_CCMPB_IRQHandler = Default_Handler
#pragma weak GPT1_CMPC_IRQHandler = Default_Handler
#pragma weak GPT1_CMPD_IRQHandler = Default_Handler
#pragma weak GPT1_CMPE_IRQHandler = Default_Handler
#pragma weak GPT1_CMPF_IRQHandler = Default_Handler
#pragma weak GPT1_OVF_IRQHandler = Default_Handler
#pragma weak GPT1_UDF_IRQHandler = Default_Handler
#pragma weak GPT1_ADTRGA_IRQHandler = Default_Handler
#pragma weak GPT1_ADTRGB_IRQHandler = Default_Handler
#pragma weak GPT2_CCMPA_IRQHandler = Default_Handler
#pragma weak GPT2_CCMPB_IRQHandler = Default_Handler
#pragma weak GPT2_CMPC_IRQHandler = Default_Handler
#pragma weak GPT2_CMPD_IRQHandler = Default_Handler
#pragma weak GPT2_CMPE_IRQHandler = Default_Handler
#pragma weak GPT2_CMPF_IRQHandler = Default_Handler
#pragma weak GPT2_OVF_IRQHandler = Default_Handler
#pragma weak GPT2_UDF_IRQHandler = Default_Handler
#pragma weak GPT2_ADTRGA_IRQHandler = Default_Handler
#pragma weak GPT2_ADTRGB_IRQHandler = Default_Handler
#pragma weak GPT3_CCMPA_IRQHandler = Default_Handler
#pragma weak GPT3_CCMPB_IRQHandler = Default_Handler
#pragma weak GPT3_CMPC_IRQHandler = Default_Handler
#pragma weak GPT3_CMPD_IRQHandler = Default_Handler
#pragma weak GPT3_CMPE_IRQHandler = Default_Handler
#pragma weak GPT3_CMPF_IRQHandler = Default_Handler
#pragma weak GPT3_OVF_IRQHandler = Default_Handler
#pragma weak GPT3_UDF_IRQHandler = Default_Handler
#pragma weak GPT3_ADTRGA_IRQHandler = Default_Handler
#pragma weak GPT3_ADTRGB_IRQHandler = Default_Handler
#pragma weak GPT4_CCMPA_IRQHandler = Default_Handler
#pragma weak GPT4_CCMPB_IRQHandler = Default_Handler
#pragma weak GPT4_CMPC_IRQHandler = Default_Handler
#pragma weak GPT4_CMPD_IRQHandler = Default_Handler
#pragma weak GPT4_CMPE_IRQHandler = Default_Handler
#pragma weak GPT4_CMPF_IRQHandler = Default_Handler
#pragma weak GPT4_OVF_IRQHandler = Default_Handler
#pragma weak GPT4_UDF_IRQHandler = Default_Handler
#pragma weak GPT4_ADTRGA_IRQHandler = Default_Handler
#pragma weak GPT4_ADTRGB_IRQHandler = Default_Handler
#pragma weak GPT5_CCMPA_IRQHandler = Default_Handler
#pragma weak GPT5_CCMPB_IRQHandler = Default_Handler
#pragma weak GPT5_CMPC_IRQHandler = Default_Handler
#pragma weak GPT5_CMPD_IRQHandler = Default_Handler
#pragma weak GPT5_CMPE_IRQHandler = Default_Handler
#pragma weak GPT5_CMPF_IRQHandler = Default_Handler
#pragma weak GPT5_OVF_IRQHandler = Default_Handler
#pragma weak GPT5_UDF_IRQHandler = Default_Handler
#pragma weak GPT5_ADTRGA_IRQHandler = Default_Handler
#pragma weak GPT5_ADTRGB_IRQHandler = Default_Handler
#pragma weak GPT6_CCMPA_IRQHandler = Default_Handler
#pragma weak GPT6_CCMPB_IRQHandler = Default_Handler
#pragma weak GPT6_CMPC_IRQHandler = Default_Handler
#pragma weak GPT6_CMPD_IRQHandler = Default_Handler
#pragma weak GPT6_CMPE_IRQHandler = Default_Handler
#pragma weak GPT6_CMPF_IRQHandler = Default_Handler
#pragma weak GPT6_OVF_IRQHandler = Default_Handler
#pragma weak GPT6_UDF_IRQHandler = Default_Handler
#pragma weak GPT6_ADTRGA_IRQHandler = Default_Handler
#pragma weak GPT6_ADTRGB_IRQHandler = Default_Handler
#pragma weak GPT7_CCMPA_IRQHandler = Default_Handler
#pragma weak GPT7_CCMPB_IRQHandler = Default_Handler
#pragma weak GPT7_CMPC_IRQHandler = Default_Handler
#pragma weak GPT7_CMPD_IRQHandler = Default_Handler
#pragma weak GPT7_CMPE_IRQHandler = Default_Handler
#pragma weak GPT7_CMPF_IRQHandler = Default_Handler
#pragma weak GPT7_OVF_IRQHandler = Default_Handler
#pragma weak GPT7_UDF_IRQHandler = Default_Handler
#pragma weak GPT7_ADTRGA_IRQHandler = Default_Handler
#pragma weak GPT7_ADTRGB_IRQHandler = Default_Handler
#pragma weak GPT8_CCMPA_IRQHandler = Default_Handler
#pragma weak GPT8_CCMPB_IRQHandler = Default_Handler
#pragma weak GPT8_CMPC_IRQHandler = Default_Handler
#pragma weak GPT8_CMPD_IRQHandler = Default_Handler
#pragma weak GPT8_CMPE_IRQHandler = Default_Handler
#pragma weak GPT8_CMPF_IRQHandler = Default_Handler
#pragma weak GPT8_OVF_IRQHandler = Default_Handler
#pragma weak GPT8_UDF_IRQHandler = Default_Handler
#pragma weak GPT9_CCMPA_IRQHandler = Default_Handler
#pragma weak GPT9_CCMPB_IRQHandler = Default_Handler
#pragma weak GPT9_CMPC_IRQHandler = Default_Handler
#pragma weak GPT9_CMPD_IRQHandler = Default_Handler
#pragma weak GPT9_CMPE_IRQHandler = Default_Handler
#pragma weak GPT9_CMPF_IRQHandler = Default_Handler
#pragma weak GPT9_OVF_IRQHandler = Default_Handler
#pragma weak GPT9_UDF_IRQHandler = Default_Handler
#pragma weak GPT10_CCMPA_IRQHandler = Default_Handler
#pragma weak GPT10_CCMPB_IRQHandler = Default_Handler
#pragma weak GPT10_CMPC_IRQHandler = Default_Handler
#pragma weak GPT10_CMPD_IRQHandler = Default_Handler
#pragma weak GPT10_CMPE_IRQHandler = Default_Handler
#pragma weak GPT10_CMPF_IRQHandler = Default_Handler
#pragma weak GPT10_OVF_IRQHandler = Default_Handler
#pragma weak GPT10_UDF_IRQHandler = Default_Handler
#pragma weak GPT11_CCMPA_IRQHandler = Default_Handler
#pragma weak GPT11_CCMPB_IRQHandler = Default_Handler
#pragma weak GPT11_CMPC_IRQHandler = Default_Handler
#pragma weak GPT11_CMPD_IRQHandler = Default_Handler
#pragma weak GPT11_CMPE_IRQHandler = Default_Handler
#pragma weak GPT11_CMPF_IRQHandler = Default_Handler
#pragma weak GPT11_OVF_IRQHandler = Default_Handler
#pragma weak GPT11_UDF_IRQHandler = Default_Handler
#pragma weak GPT12_CCMPA_IRQHandler = Default_Handler
#pragma weak GPT12_CCMPB_IRQHandler = Default_Handler
#pragma weak GPT12_CMPC_IRQHandler = Default_Handler
#pragma weak GPT12_CMPD_IRQHandler = Default_Handler
#pragma weak GPT12_CMPE_IRQHandler = Default_Handler
#pragma weak GPT12_CMPF_IRQHandler = Default_Handler
#pragma weak GPT12_OVF_IRQHandler = Default_Handler
#pragma weak GPT12_UDF_IRQHandler = Default_Handler
#pragma weak GPT13_CCMPA_IRQHandler = Default_Handler
#pragma weak GPT13_CCMPB_IRQHandler = Default_Handler
#pragma weak GPT13_CMPC_IRQHandler = Default_Handler
#pragma weak GPT13_CMPD_IRQHandler = Default_Handler
#pragma weak GPT13_CMPE_IRQHandler = Default_Handler
#pragma weak GPT13_CMPF_IRQHandler = Default_Handler
#pragma weak GPT13_OVF_IRQHandler = Default_Handler
#pragma weak GPT13_UDF_IRQHandler = Default_Handler
#pragma weak GPT_UVWEDGE_IRQHandler = Default_Handler
#pragma weak ETHER_IPLS_IRQHandler = Default_Handler
#pragma weak ETHER_MINT_IRQHandler = Default_Handler
#pragma weak ETHER_PINT_IRQHandler = Default_Handler
#pragma weak ETHER_EINT0_IRQHandler = Default_Handler
#pragma weak ETHER_EINT1_IRQHandler = Default_Handler
#pragma weak USBHS_D0FIFO_IRQHandler = Default_Handler
#pragma weak USBHS_D1FIFO_IRQHandler = Default_Handler
#pragma weak USBHS_USBIR_IRQHandler = Default_Handler
#pragma weak SCI0_RXI_IRQHandler = Default_Handler
#pragma weak SCI0_TXI_IRQHandler = Default_Handler
#pragma weak SCI0_TEI_IRQHandler = Default_Handler
#pragma weak SCI0_ERI_IRQHandler = Default_Handler
#pragma weak SCI0_AM_IRQHandler = Default_Handler
#pragma weak SCI0_RXI_OR_ERI_IRQHandler = Default_Handler
#pragma weak SCI1_RXI_IRQHandler = Default_Handler
#pragma weak SCI1_TXI_IRQHandler = Default_Handler
#pragma weak SCI1_TEI_IRQHandler = Default_Handler
#pragma weak SCI1_ERI_IRQHandler = Default_Handler
#pragma weak SCI1_AM_IRQHandler = Default_Handler
#pragma weak SCI2_RXI_IRQHandler = Default_Handler
#pragma weak SCI2_TXI_IRQHandler = Default_Handler
#pragma weak SCI2_TEI_IRQHandler = Default_Handler
#pragma weak SCI2_ERI_IRQHandler = Default_Handler
#pragma weak SCI2_AM_IRQHandler = Default_Handler
#pragma weak SCI3_RXI_IRQHandler = Default_Handler
#pragma weak SCI3_TXI_IRQHandler = Default_Handler
#pragma weak SCI3_TEI_IRQHandler = Default_Handler
#pragma weak SCI3_ERI_IRQHandler = Default_Handler
#pragma weak SCI3_AM_IRQHandler = Default_Handler
#pragma weak SCI4_RXI_IRQHandler = Default_Handler
#pragma weak SCI4_TXI_IRQHandler = Default_Handler
#pragma weak SCI4_TEI_IRQHandler = Default_Handler
#pragma weak SCI4_ERI_IRQHandler = Default_Handler
#pragma weak SCI4_AM_IRQHandler = Default_Handler
#pragma weak SCI5_RXI_IRQHandler = Default_Handler
#pragma weak SCI5_TXI_IRQHandler = Default_Handler
#pragma weak SCI5_TEI_IRQHandler = Default_Handler
#pragma weak SCI5_ERI_IRQHandler = Default_Handler
#pragma weak SCI5_AM_IRQHandler = Default_Handler
#pragma weak SCI6_RXI_IRQHandler = Default_Handler
#pragma weak SCI6_TXI_IRQHandler = Default_Handler
#pragma weak SCI6_TEI_IRQHandler = Default_Handler
#pragma weak SCI6_ERI_IRQHandler = Default_Handler
#pragma weak SCI6_AM_IRQHandler = Default_Handler
#pragma weak SCI7_RXI_IRQHandler = Default_Handler
#pragma weak SCI7_TXI_IRQHandler = Default_Handler
#pragma weak SCI7_TEI_IRQHandler = Default_Handler
#pragma weak SCI7_ERI_IRQHandler = Default_Handler
#pragma weak SCI7_AM_IRQHandler = Default_Handler
#pragma weak SCI8_RXI_IRQHandler = Default_Handler
#pragma weak SCI8_TXI_IRQHandler = Default_Handler
#pragma weak SCI8_TEI_IRQHandler = Default_Handler
#pragma weak SCI8_ERI_IRQHandler = Default_Handler
#pragma weak SCI8_AM_IRQHandler = Default_Handler
#pragma weak SCI9_RXI_IRQHandler = Default_Handler
#pragma weak SCI9_TXI_IRQHandler = Default_Handler
#pragma weak SCI9_TEI_IRQHandler = Default_Handler
#pragma weak SCI9_ERI_IRQHandler = Default_Handler
#pragma weak SCI9_AM_IRQHandler = Default_Handler
#pragma weak SPI0_SPRI_IRQHandler = Default_Handler
#pragma weak SPI0_SPTI_IRQHandler = Default_Handler
#pragma weak SPI0_SPII_IRQHandler = Default_Handler
#pragma weak SPI0_SPEI_IRQHandler = Default_Handler
#pragma weak SPI0_SPTEND_IRQHandler = Default_Handler
#pragma weak SPI1_SPRI_IRQHandler = Default_Handler
#pragma weak SPI1_SPTI_IRQHandler = Default_Handler
#pragma weak SPI1_SPII_IRQHandler = Default_Handler
#pragma weak SPI1_SPEI_IRQHandler = Default_Handler
#pragma weak SPI1_SPTEND_IRQHandler = Default_Handler
#pragma weak QSPI_INTR_IRQHandler = Default_Handler
#pragma weak SDHI_MMC0_ACCS_IRQHandler = Default_Handler
#pragma weak SDHI_MMC0_SDIO_IRQHandler = Default_Handler
#pragma weak SDHI_MMC0_CARD_IRQHandler = Default_Handler
#pragma weak SDHI_MMC0_ODMSDBREQ_IRQHandler = Default_Handler
#pragma weak SDHI_MMC1_ACCS_IRQHandler = Default_Handler
#pragma weak SDHI_MMC1_SDIO_IRQHandler = Default_Handler
#pragma weak SDHI_MMC1_CARD_IRQHandler = Default_Handler
#pragma weak SDHI_MMC1_ODMSDBREQ_IRQHandler = Default_Handler
#pragma weak GLCDC_VPOS_IRQHandler = Default_Handler
#pragma weak GLCDC_L1UNDF_IRQHandler = Default_Handler
#pragma weak GLCDC_L2UNDF_IRQHandler = Default_Handler
#pragma weak DRW_IRQ_IRQHandler = Default_Handler
#pragma weak JPEG_JEDI_IRQHandler = Default_Handler
#pragma weak JPEG_JDTI_IRQHandler = Default_Handler

//Vector table
#pragma language = extended
#pragma segment = "CSTACK"
#pragma section = ".intvec"
#pragma location = ".intvec"
const uint32_t __vector_table[112] =
{
   (uint32_t) __sfe("CSTACK"),

   (uint32_t) Reset_Handler,
   (uint32_t) NMI_Handler,
   (uint32_t) HardFault_Handler,
   (uint32_t) MemManage_Handler,
   (uint32_t) BusFault_Handler,
   (uint32_t) UsageFault_Handler,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) 0,
   (uint32_t) SVC_Handler,
   (uint32_t) DebugMon_Handler,
   (uint32_t) 0,
   (uint32_t) PendSV_Handler,
   (uint32_t) SysTick_Handler,

#if (BSP_IRQ_CFG_PORT_IRQ0 != BSP_IRQ_DISABLED)
   (uint32_t) PORT_IRQ0_IRQHandler,
#endif
#if (BSP_IRQ_CFG_PORT_IRQ1 != BSP_IRQ_DISABLED)
   (uint32_t) PORT_IRQ1_IRQHandler,
#endif
#if (BSP_IRQ_CFG_PORT_IRQ2 != BSP_IRQ_DISABLED)
   (uint32_t) PORT_IRQ2_IRQHandler,
#endif
#if (BSP_IRQ_CFG_PORT_IRQ3 != BSP_IRQ_DISABLED)
   (uint32_t) PORT_IRQ3_IRQHandler,
#endif
#if (BSP_IRQ_CFG_PORT_IRQ4 != BSP_IRQ_DISABLED)
   (uint32_t) PORT_IRQ4_IRQHandler,
#endif
#if (BSP_IRQ_CFG_PORT_IRQ5 != BSP_IRQ_DISABLED)
   (uint32_t) PORT_IRQ5_IRQHandler,
#endif
#if (BSP_IRQ_CFG_PORT_IRQ6 != BSP_IRQ_DISABLED)
   (uint32_t) PORT_IRQ6_IRQHandler,
#endif
#if (BSP_IRQ_CFG_PORT_IRQ7 != BSP_IRQ_DISABLED)
   (uint32_t) PORT_IRQ7_IRQHandler,
#endif
#if (BSP_IRQ_CFG_PORT_IRQ8 != BSP_IRQ_DISABLED)
   (uint32_t) PORT_IRQ8_IRQHandler,
#endif
#if (BSP_IRQ_CFG_PORT_IRQ9 != BSP_IRQ_DISABLED)
   (uint32_t) PORT_IRQ9_IRQHandler,
#endif
#if (BSP_IRQ_CFG_PORT_IRQ10 != BSP_IRQ_DISABLED)
   (uint32_t) PORT_IRQ10_IRQHandler,
#endif
#if (BSP_IRQ_CFG_PORT_IRQ11 != BSP_IRQ_DISABLED)
   (uint32_t) PORT_IRQ11_IRQHandler,
#endif
#if (BSP_IRQ_CFG_PORT_IRQ12 != BSP_IRQ_DISABLED)
   (uint32_t) PORT_IRQ12_IRQHandler,
#endif
#if (BSP_IRQ_CFG_PORT_IRQ13 != BSP_IRQ_DISABLED)
   (uint32_t) PORT_IRQ13_IRQHandler,
#endif
#if (BSP_IRQ_CFG_PORT_IRQ14 != BSP_IRQ_DISABLED)
   (uint32_t) PORT_IRQ14_IRQHandler,
#endif
#if (BSP_IRQ_CFG_PORT_IRQ15 != BSP_IRQ_DISABLED)
   (uint32_t) PORT_IRQ15_IRQHandler,
#endif
#if (BSP_IRQ_CFG_DMAC0_INT != BSP_IRQ_DISABLED)
   (uint32_t) DMAC0_INT_IRQHandler,
#endif
#if (BSP_IRQ_CFG_DMAC1_INT != BSP_IRQ_DISABLED)
   (uint32_t) DMAC1_INT_IRQHandler,
#endif
#if (BSP_IRQ_CFG_DMAC2_INT != BSP_IRQ_DISABLED)
   (uint32_t) DMAC2_INT_IRQHandler,
#endif
#if (BSP_IRQ_CFG_DMAC3_INT != BSP_IRQ_DISABLED)
   (uint32_t) DMAC3_INT_IRQHandler,
#endif
#if (BSP_IRQ_CFG_DMAC4_INT != BSP_IRQ_DISABLED)
   (uint32_t) DMAC4_INT_IRQHandler,
#endif
#if (BSP_IRQ_CFG_DMAC5_INT != BSP_IRQ_DISABLED)
   (uint32_t) DMAC5_INT_IRQHandler,
#endif
#if (BSP_IRQ_CFG_DMAC6_INT != BSP_IRQ_DISABLED)
   (uint32_t) DMAC6_INT_IRQHandler,
#endif
#if (BSP_IRQ_CFG_DMAC7_INT != BSP_IRQ_DISABLED)
   (uint32_t) DMAC7_INT_IRQHandler,
#endif
#if (BSP_IRQ_CFG_DTC_COMPLETE != BSP_IRQ_DISABLED)
   (uint32_t) DTC_COMPLETE_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ICU_SNZCANCEL != BSP_IRQ_DISABLED)
   (uint32_t) ICU_SNZCANCEL_IRQHandler,
#endif
#if (BSP_IRQ_CFG_FCU_FIFERR != BSP_IRQ_DISABLED)
   (uint32_t) FCU_FIFERR_IRQHandler,
#endif
#if (BSP_IRQ_CFG_FCU_FRDYI != BSP_IRQ_DISABLED)
   (uint32_t) FCU_FRDYI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_LVD_LVD1 != BSP_IRQ_DISABLED)
   (uint32_t) LVD_LVD1_IRQHandler,
#endif
#if (BSP_IRQ_CFG_LVD_LVD2 != BSP_IRQ_DISABLED)
   (uint32_t) LVD_LVD2_IRQHandler,
#endif
#if (BSP_IRQ_CFG_MOSC_STOP != BSP_IRQ_DISABLED)
   (uint32_t) MOSC_STOP_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SYSTEM_SNZREQ != BSP_IRQ_DISABLED)
   (uint32_t) SYSTEM_SNZREQ_IRQHandler,
#endif
#if (BSP_IRQ_CFG_AGT0_AGTI != BSP_IRQ_DISABLED)
   (uint32_t) AGT0_AGTI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_AGT0_AGTCMAI != BSP_IRQ_DISABLED)
   (uint32_t) AGT0_AGTCMAI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_AGT0_AGTCMBI != BSP_IRQ_DISABLED)
   (uint32_t) AGT0_AGTCMBI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_AGT1_AGTI != BSP_IRQ_DISABLED)
   (uint32_t) AGT1_AGTI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_AGT1_AGTCMAI != BSP_IRQ_DISABLED)
   (uint32_t) AGT1_AGTCMAI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_AGT1_AGTCMBI != BSP_IRQ_DISABLED)
   (uint32_t) AGT1_AGTCMBI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_IWDT_NMIUNDF != BSP_IRQ_DISABLED)
   (uint32_t) IWDT_NMIUNDF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_WDT_NMIUNDF != BSP_IRQ_DISABLED)
   (uint32_t) WDT_NMIUNDF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_RTC_ALM != BSP_IRQ_DISABLED)
   (uint32_t) RTC_ALM_IRQHandler,
#endif
#if (BSP_IRQ_CFG_RTC_PRD != BSP_IRQ_DISABLED)
   (uint32_t) RTC_PRD_IRQHandler,
#endif
#if (BSP_IRQ_CFG_RTC_CUP != BSP_IRQ_DISABLED)
   (uint32_t) RTC_CUP_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ADC120_ADI != BSP_IRQ_DISABLED)
   (uint32_t) ADC120_ADI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ADC120_GBADI != BSP_IRQ_DISABLED)
   (uint32_t) ADC120_GBADI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ADC120_CMPAI != BSP_IRQ_DISABLED)
   (uint32_t) ADC120_CMPAI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ADC120_CMPBI != BSP_IRQ_DISABLED)
   (uint32_t) ADC120_CMPBI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ADC120_WCMPM != BSP_IRQ_DISABLED)
   (uint32_t) ADC120_WCMPM_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ADC120_WCMPUM != BSP_IRQ_DISABLED)
   (uint32_t) ADC120_WCMPUM_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ADC121_ADI != BSP_IRQ_DISABLED)
   (uint32_t) ADC121_ADI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ADC121_GBADI != BSP_IRQ_DISABLED)
   (uint32_t) ADC121_GBADI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ADC121_CMPAI != BSP_IRQ_DISABLED)
   (uint32_t) ADC121_CMPAI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ADC121_CMPBI != BSP_IRQ_DISABLED)
   (uint32_t) ADC121_CMPBI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ADC121_WCMPM != BSP_IRQ_DISABLED)
   (uint32_t) ADC121_WCMPM_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ADC121_WCMPUM != BSP_IRQ_DISABLED)
   (uint32_t) ADC121_WCMPUM_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ACMP_HS0 != BSP_IRQ_DISABLED)
   (uint32_t) ACMP_HS0_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ACMP_HS1 != BSP_IRQ_DISABLED)
   (uint32_t) ACMP_HS1_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ACMP_HS2 != BSP_IRQ_DISABLED)
   (uint32_t) ACMP_HS2_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ACMP_HS3 != BSP_IRQ_DISABLED)
   (uint32_t) ACMP_HS3_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ACMP_HS4 != BSP_IRQ_DISABLED)
   (uint32_t) ACMP_HS4_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ACMP_HS5 != BSP_IRQ_DISABLED)
   (uint32_t) ACMP_HS5_IRQHandler,
#endif
#if (BSP_IRQ_CFG_USBFS_D0FIFO != BSP_IRQ_DISABLED)
   (uint32_t) USBFS_D0FIFO_IRQHandler,
#endif
#if (BSP_IRQ_CFG_USBFS_D1FIFO != BSP_IRQ_DISABLED)
   (uint32_t) USBFS_D1FIFO_IRQHandler,
#endif
#if (BSP_IRQ_CFG_USBFS_USBI != BSP_IRQ_DISABLED)
   (uint32_t) USBFS_USBI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_USBFS_USBR != BSP_IRQ_DISABLED)
   (uint32_t) USBFS_USBR_IRQHandler,
#endif
#if (BSP_IRQ_CFG_IIC0_RXI != BSP_IRQ_DISABLED)
   (uint32_t) IIC0_RXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_IIC0_TXI != BSP_IRQ_DISABLED)
   (uint32_t) IIC0_TXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_IIC0_TEI != BSP_IRQ_DISABLED)
   (uint32_t) IIC0_TEI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_IIC0_EEI != BSP_IRQ_DISABLED)
   (uint32_t) IIC0_EEI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_IIC0_WUI != BSP_IRQ_DISABLED)
   (uint32_t) IIC0_WUI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_IIC1_RXI != BSP_IRQ_DISABLED)
   (uint32_t) IIC1_RXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_IIC1_TXI != BSP_IRQ_DISABLED)
   (uint32_t) IIC1_TXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_IIC1_TEI != BSP_IRQ_DISABLED)
   (uint32_t) IIC1_TEI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_IIC1_EEI != BSP_IRQ_DISABLED)
   (uint32_t) IIC1_EEI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_IIC2_RXI != BSP_IRQ_DISABLED)
   (uint32_t) IIC2_RXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_IIC2_TXI != BSP_IRQ_DISABLED)
   (uint32_t) IIC2_TXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_IIC2_TEI != BSP_IRQ_DISABLED)
   (uint32_t) IIC2_TEI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_IIC2_EEI != BSP_IRQ_DISABLED)
   (uint32_t) IIC2_EEI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SSI0_SSITXI != BSP_IRQ_DISABLED)
   (uint32_t) SSI0_SSITXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SSI0_SSIRXI != BSP_IRQ_DISABLED)
   (uint32_t) SSI0_SSIRXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SSI0_SSIF != BSP_IRQ_DISABLED)
   (uint32_t) SSI0_SSIF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SSI1_SSIRT != BSP_IRQ_DISABLED)
   (uint32_t) SSI1_SSIRT_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SSI1_SSIF != BSP_IRQ_DISABLED)
   (uint32_t) SSI1_SSIF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SRC_IDEI != BSP_IRQ_DISABLED)
   (uint32_t) SRC_IDEI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SRC_ODFI != BSP_IRQ_DISABLED)
   (uint32_t) SRC_ODFI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SRC_OVFI != BSP_IRQ_DISABLED)
   (uint32_t) SRC_OVFI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SRC_UDFI != BSP_IRQ_DISABLED)
   (uint32_t) SRC_UDFI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SRC_CEFI != BSP_IRQ_DISABLED)
   (uint32_t) SRC_CEFI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_PDC_PCDFI != BSP_IRQ_DISABLED)
   (uint32_t) PDC_PCDFI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_PDC_PCFEI != BSP_IRQ_DISABLED)
   (uint32_t) PDC_PCFEI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_PDC_PCERI != BSP_IRQ_DISABLED)
   (uint32_t) PDC_PCERI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_CTSU_CTSUWR != BSP_IRQ_DISABLED)
   (uint32_t) CTSU_CTSUWR_IRQHandler,
#endif
#if (BSP_IRQ_CFG_CTSU_CTSURD != BSP_IRQ_DISABLED)
   (uint32_t) CTSU_CTSURD_IRQHandler,
#endif
#if (BSP_IRQ_CFG_CTSU_CTSUFN != BSP_IRQ_DISABLED)
   (uint32_t) CTSU_CTSUFN_IRQHandler,
#endif
#if (BSP_IRQ_CFG_KEY_INTKR != BSP_IRQ_DISABLED)
   (uint32_t) KEY_INTKR_IRQHandler,
#endif
#if (BSP_IRQ_CFG_DOC_DOPCI != BSP_IRQ_DISABLED)
   (uint32_t) DOC_DOPCI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_CAC_FERRI != BSP_IRQ_DISABLED)
   (uint32_t) CAC_FERRI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_CAC_MENDI != BSP_IRQ_DISABLED)
   (uint32_t) CAC_MENDI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_CAC_OVFI != BSP_IRQ_DISABLED)
   (uint32_t) CAC_OVFI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_CAN0_ERS != BSP_IRQ_DISABLED)
   (uint32_t) CAN0_ERS_IRQHandler,
#endif
#if (BSP_IRQ_CFG_CAN0_RXF != BSP_IRQ_DISABLED)
   (uint32_t) CAN0_RXF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_CAN0_TXF != BSP_IRQ_DISABLED)
   (uint32_t) CAN0_TXF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_CAN0_RXM != BSP_IRQ_DISABLED)
   (uint32_t) CAN0_RXM_IRQHandler,
#endif
#if (BSP_IRQ_CFG_CAN0_TXM != BSP_IRQ_DISABLED)
   (uint32_t) CAN0_TXM_IRQHandler,
#endif
#if (BSP_IRQ_CFG_CAN1_ERS != BSP_IRQ_DISABLED)
   (uint32_t) CAN1_ERS_IRQHandler,
#endif
#if (BSP_IRQ_CFG_CAN1_RXF != BSP_IRQ_DISABLED)
   (uint32_t) CAN1_RXF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_CAN1_TXF != BSP_IRQ_DISABLED)
   (uint32_t) CAN1_TXF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_CAN1_RXM != BSP_IRQ_DISABLED)
   (uint32_t) CAN1_RXM_IRQHandler,
#endif
#if (BSP_IRQ_CFG_CAN1_TXM != BSP_IRQ_DISABLED)
   (uint32_t) CAN1_TXM_IRQHandler,
#endif
#if (BSP_IRQ_CFG_IOPORT_GROUP1 != BSP_IRQ_DISABLED)
   (uint32_t) IOPORT_GROUP1_IRQHandler,
#endif
#if (BSP_IRQ_CFG_IOPORT_GROUP2 != BSP_IRQ_DISABLED)
   (uint32_t) IOPORT_GROUP2_IRQHandler,
#endif
#if (BSP_IRQ_CFG_IOPORT_GROUP3 != BSP_IRQ_DISABLED)
   (uint32_t) IOPORT_GROUP3_IRQHandler,
#endif
#if (BSP_IRQ_CFG_IOPORT_GROUP4 != BSP_IRQ_DISABLED)
   (uint32_t) IOPORT_GROUP4_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ELC_SWEVT0 != BSP_IRQ_DISABLED)
   (uint32_t) ELC_SWEVT0_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ELC_SWEVT1 != BSP_IRQ_DISABLED)
   (uint32_t) ELC_SWEVT1_IRQHandler,
#endif
#if (BSP_IRQ_CFG_POEG_GROUP0 != BSP_IRQ_DISABLED)
   (uint32_t) POEG_GROUP0_IRQHandler,
#endif
#if (BSP_IRQ_CFG_POEG_GROUP1 != BSP_IRQ_DISABLED)
   (uint32_t) POEG_GROUP1_IRQHandler,
#endif
#if (BSP_IRQ_CFG_POEG_GROUP2 != BSP_IRQ_DISABLED)
   (uint32_t) POEG_GROUP2_IRQHandler,
#endif
#if (BSP_IRQ_CFG_POEG_GROUP3 != BSP_IRQ_DISABLED)
   (uint32_t) POEG_GROUP3_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT0_CCMPA != BSP_IRQ_DISABLED)
   (uint32_t) GPT0_CCMPA_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT0_CCMPB != BSP_IRQ_DISABLED)
   (uint32_t) GPT0_CCMPB_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT0_CMPC != BSP_IRQ_DISABLED)
   (uint32_t) GPT0_CMPC_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT0_CMPD != BSP_IRQ_DISABLED)
   (uint32_t) GPT0_CMPD_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT0_CMPE != BSP_IRQ_DISABLED)
   (uint32_t) GPT0_CMPE_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT0_CMPF != BSP_IRQ_DISABLED)
   (uint32_t) GPT0_CMPF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT0_OVF != BSP_IRQ_DISABLED)
   (uint32_t) GPT0_OVF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT0_UDF != BSP_IRQ_DISABLED)
   (uint32_t) GPT0_UDF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT0_ADTRGA != BSP_IRQ_DISABLED)
   (uint32_t) GPT0_ADTRGA_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT0_ADTRGB != BSP_IRQ_DISABLED)
   (uint32_t) GPT0_ADTRGB_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT1_CCMPA != BSP_IRQ_DISABLED)
   (uint32_t) GPT1_CCMPA_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT1_CCMPB != BSP_IRQ_DISABLED)
   (uint32_t) GPT1_CCMPB_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT1_CMPC != BSP_IRQ_DISABLED)
   (uint32_t) GPT1_CMPC_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT1_CMPD != BSP_IRQ_DISABLED)
   (uint32_t) GPT1_CMPD_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT1_CMPE != BSP_IRQ_DISABLED)
   (uint32_t) GPT1_CMPE_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT1_CMPF != BSP_IRQ_DISABLED)
   (uint32_t) GPT1_CMPF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT1_OVF != BSP_IRQ_DISABLED)
   (uint32_t) GPT1_OVF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT1_UDF != BSP_IRQ_DISABLED)
   (uint32_t) GPT1_UDF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT1_ADTRGA != BSP_IRQ_DISABLED)
   (uint32_t) GPT1_ADTRGA_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT1_ADTRGB != BSP_IRQ_DISABLED)
   (uint32_t) GPT1_ADTRGB_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT2_CCMPA != BSP_IRQ_DISABLED)
   (uint32_t) GPT2_CCMPA_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT2_CCMPB != BSP_IRQ_DISABLED)
   (uint32_t) GPT2_CCMPB_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT2_CMPC != BSP_IRQ_DISABLED)
   (uint32_t) GPT2_CMPC_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT2_CMPD != BSP_IRQ_DISABLED)
   (uint32_t) GPT2_CMPD_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT2_CMPE != BSP_IRQ_DISABLED)
   (uint32_t) GPT2_CMPE_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT2_CMPF != BSP_IRQ_DISABLED)
   (uint32_t) GPT2_CMPF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT2_OVF != BSP_IRQ_DISABLED)
   (uint32_t) GPT2_OVF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT2_UDF != BSP_IRQ_DISABLED)
   (uint32_t) GPT2_UDF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT2_ADTRGA != BSP_IRQ_DISABLED)
   (uint32_t) GPT2_ADTRGA_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT2_ADTRGB != BSP_IRQ_DISABLED)
   (uint32_t) GPT2_ADTRGB_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT3_CCMPA != BSP_IRQ_DISABLED)
   (uint32_t) GPT3_CCMPA_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT3_CCMPB != BSP_IRQ_DISABLED)
   (uint32_t) GPT3_CCMPB_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT3_CMPC != BSP_IRQ_DISABLED)
   (uint32_t) GPT3_CMPC_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT3_CMPD != BSP_IRQ_DISABLED)
   (uint32_t) GPT3_CMPD_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT3_CMPE != BSP_IRQ_DISABLED)
   (uint32_t) GPT3_CMPE_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT3_CMPF != BSP_IRQ_DISABLED)
   (uint32_t) GPT3_CMPF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT3_OVF != BSP_IRQ_DISABLED)
   (uint32_t) GPT3_OVF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT3_UDF != BSP_IRQ_DISABLED)
   (uint32_t) GPT3_UDF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT3_ADTRGA != BSP_IRQ_DISABLED)
   (uint32_t) GPT3_ADTRGA_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT3_ADTRGB != BSP_IRQ_DISABLED)
   (uint32_t) GPT3_ADTRGB_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT4_CCMPA != BSP_IRQ_DISABLED)
   (uint32_t) GPT4_CCMPA_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT4_CCMPB != BSP_IRQ_DISABLED)
   (uint32_t) GPT4_CCMPB_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT4_CMPC != BSP_IRQ_DISABLED)
   (uint32_t) GPT4_CMPC_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT4_CMPD != BSP_IRQ_DISABLED)
   (uint32_t) GPT4_CMPD_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT4_CMPE != BSP_IRQ_DISABLED)
   (uint32_t) GPT4_CMPE_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT4_CMPF != BSP_IRQ_DISABLED)
   (uint32_t) GPT4_CMPF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT4_OVF != BSP_IRQ_DISABLED)
   (uint32_t) GPT4_OVF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT4_UDF != BSP_IRQ_DISABLED)
   (uint32_t) GPT4_UDF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT4_ADTRGA != BSP_IRQ_DISABLED)
   (uint32_t) GPT4_ADTRGA_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT4_ADTRGB != BSP_IRQ_DISABLED)
   (uint32_t) GPT4_ADTRGB_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT5_CCMPA != BSP_IRQ_DISABLED)
   (uint32_t) GPT5_CCMPA_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT5_CCMPB != BSP_IRQ_DISABLED)
   (uint32_t) GPT5_CCMPB_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT5_CMPC != BSP_IRQ_DISABLED)
   (uint32_t) GPT5_CMPC_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT5_CMPD != BSP_IRQ_DISABLED)
   (uint32_t) GPT5_CMPD_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT5_CMPE != BSP_IRQ_DISABLED)
   (uint32_t) GPT5_CMPE_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT5_CMPF != BSP_IRQ_DISABLED)
   (uint32_t) GPT5_CMPF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT5_OVF != BSP_IRQ_DISABLED)
   (uint32_t) GPT5_OVF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT5_UDF != BSP_IRQ_DISABLED)
   (uint32_t) GPT5_UDF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT5_ADTRGA != BSP_IRQ_DISABLED)
   (uint32_t) GPT5_ADTRGA_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT5_ADTRGB != BSP_IRQ_DISABLED)
   (uint32_t) GPT5_ADTRGB_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT6_CCMPA != BSP_IRQ_DISABLED)
   (uint32_t) GPT6_CCMPA_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT6_CCMPB != BSP_IRQ_DISABLED)
   (uint32_t) GPT6_CCMPB_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT6_CMPC != BSP_IRQ_DISABLED)
   (uint32_t) GPT6_CMPC_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT6_CMPD != BSP_IRQ_DISABLED)
   (uint32_t) GPT6_CMPD_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT6_CMPE != BSP_IRQ_DISABLED)
   (uint32_t) GPT6_CMPE_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT6_CMPF != BSP_IRQ_DISABLED)
   (uint32_t) GPT6_CMPF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT6_OVF != BSP_IRQ_DISABLED)
   (uint32_t) GPT6_OVF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT6_UDF != BSP_IRQ_DISABLED)
   (uint32_t) GPT6_UDF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT6_ADTRGA != BSP_IRQ_DISABLED)
   (uint32_t) GPT6_ADTRGA_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT6_ADTRGB != BSP_IRQ_DISABLED)
   (uint32_t) GPT6_ADTRGB_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT7_CCMPA != BSP_IRQ_DISABLED)
   (uint32_t) GPT7_CCMPA_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT7_CCMPB != BSP_IRQ_DISABLED)
   (uint32_t) GPT7_CCMPB_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT7_CMPC != BSP_IRQ_DISABLED)
   (uint32_t) GPT7_CMPC_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT7_CMPD != BSP_IRQ_DISABLED)
   (uint32_t) GPT7_CMPD_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT7_CMPE != BSP_IRQ_DISABLED)
   (uint32_t) GPT7_CMPE_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT7_CMPF != BSP_IRQ_DISABLED)
   (uint32_t) GPT7_CMPF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT7_OVF != BSP_IRQ_DISABLED)
   (uint32_t) GPT7_OVF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT7_UDF != BSP_IRQ_DISABLED)
   (uint32_t) GPT7_UDF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT7_ADTRGA != BSP_IRQ_DISABLED)
   (uint32_t) GPT7_ADTRGA_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT7_ADTRGB != BSP_IRQ_DISABLED)
   (uint32_t) GPT7_ADTRGB_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT8_CCMPA != BSP_IRQ_DISABLED)
   (uint32_t) GPT8_CCMPA_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT8_CCMPB != BSP_IRQ_DISABLED)
   (uint32_t) GPT8_CCMPB_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT8_CMPC != BSP_IRQ_DISABLED)
   (uint32_t) GPT8_CMPC_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT8_CMPD != BSP_IRQ_DISABLED)
   (uint32_t) GPT8_CMPD_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT8_CMPE != BSP_IRQ_DISABLED)
   (uint32_t) GPT8_CMPE_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT8_CMPF != BSP_IRQ_DISABLED)
   (uint32_t) GPT8_CMPF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT8_OVF != BSP_IRQ_DISABLED)
   (uint32_t) GPT8_OVF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT8_UDF != BSP_IRQ_DISABLED)
   (uint32_t) GPT8_UDF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT9_CCMPA != BSP_IRQ_DISABLED)
   (uint32_t) GPT9_CCMPA_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT9_CCMPB != BSP_IRQ_DISABLED)
   (uint32_t) GPT9_CCMPB_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT9_CMPC != BSP_IRQ_DISABLED)
   (uint32_t) GPT9_CMPC_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT9_CMPD != BSP_IRQ_DISABLED)
   (uint32_t) GPT9_CMPD_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT9_CMPE != BSP_IRQ_DISABLED)
   (uint32_t) GPT9_CMPE_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT9_CMPF != BSP_IRQ_DISABLED)
   (uint32_t) GPT9_CMPF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT9_OVF != BSP_IRQ_DISABLED)
   (uint32_t) GPT9_OVF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT9_UDF != BSP_IRQ_DISABLED)
   (uint32_t) GPT9_UDF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT10_CCMPA != BSP_IRQ_DISABLED)
   (uint32_t) GPT10_CCMPA_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT10_CCMPB != BSP_IRQ_DISABLED)
   (uint32_t) GPT10_CCMPB_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT10_CMPC != BSP_IRQ_DISABLED)
   (uint32_t) GPT10_CMPC_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT10_CMPD != BSP_IRQ_DISABLED)
   (uint32_t) GPT10_CMPD_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT10_CMPE != BSP_IRQ_DISABLED)
   (uint32_t) GPT10_CMPE_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT10_CMPF != BSP_IRQ_DISABLED)
   (uint32_t) GPT10_CMPF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT10_OVF != BSP_IRQ_DISABLED)
   (uint32_t) GPT10_OVF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT10_UDF != BSP_IRQ_DISABLED)
   (uint32_t) GPT10_UDF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT11_CCMPA != BSP_IRQ_DISABLED)
   (uint32_t) GPT11_CCMPA_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT11_CCMPB != BSP_IRQ_DISABLED)
   (uint32_t) GPT11_CCMPB_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT11_CMPC != BSP_IRQ_DISABLED)
   (uint32_t) GPT11_CMPC_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT11_CMPD != BSP_IRQ_DISABLED)
   (uint32_t) GPT11_CMPD_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT11_CMPE != BSP_IRQ_DISABLED)
   (uint32_t) GPT11_CMPE_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT11_CMPF != BSP_IRQ_DISABLED)
   (uint32_t) GPT11_CMPF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT11_OVF != BSP_IRQ_DISABLED)
   (uint32_t) GPT11_OVF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT11_UDF != BSP_IRQ_DISABLED)
   (uint32_t) GPT11_UDF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT12_CCMPA != BSP_IRQ_DISABLED)
   (uint32_t) GPT12_CCMPA_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT12_CCMPB != BSP_IRQ_DISABLED)
   (uint32_t) GPT12_CCMPB_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT12_CMPC != BSP_IRQ_DISABLED)
   (uint32_t) GPT12_CMPC_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT12_CMPD != BSP_IRQ_DISABLED)
   (uint32_t) GPT12_CMPD_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT12_CMPE != BSP_IRQ_DISABLED)
   (uint32_t) GPT12_CMPE_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT12_CMPF != BSP_IRQ_DISABLED)
   (uint32_t) GPT12_CMPF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT12_OVF != BSP_IRQ_DISABLED)
   (uint32_t) GPT12_OVF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT12_UDF != BSP_IRQ_DISABLED)
   (uint32_t) GPT12_UDF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT13_CCMPA != BSP_IRQ_DISABLED)
   (uint32_t) GPT13_CCMPA_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT13_CCMPB != BSP_IRQ_DISABLED)
   (uint32_t) GPT13_CCMPB_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT13_CMPC != BSP_IRQ_DISABLED)
   (uint32_t) GPT13_CMPC_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT13_CMPD != BSP_IRQ_DISABLED)
   (uint32_t) GPT13_CMPD_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT13_CMPE != BSP_IRQ_DISABLED)
   (uint32_t) GPT13_CMPE_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT13_CMPF != BSP_IRQ_DISABLED)
   (uint32_t) GPT13_CMPF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT13_OVF != BSP_IRQ_DISABLED)
   (uint32_t) GPT13_OVF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT13_UDF != BSP_IRQ_DISABLED)
   (uint32_t) GPT13_UDF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GPT_UVWEDGE != BSP_IRQ_DISABLED)
   (uint32_t) GPT_UVWEDGE_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ETHER_IPLS != BSP_IRQ_DISABLED)
   (uint32_t) ETHER_IPLS_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ETHER_MINT != BSP_IRQ_DISABLED)
   (uint32_t) ETHER_MINT_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ETHER_PINT != BSP_IRQ_DISABLED)
   (uint32_t) ETHER_PINT_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ETHER_EINT0 != BSP_IRQ_DISABLED)
   (uint32_t) ETHER_EINT0_IRQHandler,
#endif
#if (BSP_IRQ_CFG_ETHER_EINT1 != BSP_IRQ_DISABLED)
   (uint32_t) ETHER_EINT1_IRQHandler,
#endif
#if (BSP_IRQ_CFG_USBHS_D0FIFO != BSP_IRQ_DISABLED)
   (uint32_t) USBHS_D0FIFO_IRQHandler,
#endif
#if (BSP_IRQ_CFG_USBHS_D1FIFO != BSP_IRQ_DISABLED)
   (uint32_t) USBHS_D1FIFO_IRQHandler,
#endif
#if (BSP_IRQ_CFG_USBHS_USBIR != BSP_IRQ_DISABLED)
   (uint32_t) USBHS_USBIR_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI0_RXI != BSP_IRQ_DISABLED)
   (uint32_t) SCI0_RXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI0_TXI != BSP_IRQ_DISABLED)
   (uint32_t) SCI0_TXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI0_TEI != BSP_IRQ_DISABLED)
   (uint32_t) SCI0_TEI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI0_ERI != BSP_IRQ_DISABLED)
   (uint32_t) SCI0_ERI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI0_AM != BSP_IRQ_DISABLED)
   (uint32_t) SCI0_AM_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI0_RXI_OR_ERI != BSP_IRQ_DISABLED)
   (uint32_t) SCI0_RXI_OR_ERI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI1_RXI != BSP_IRQ_DISABLED)
   (uint32_t) SCI1_RXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI1_TXI != BSP_IRQ_DISABLED)
   (uint32_t) SCI1_TXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI1_TEI != BSP_IRQ_DISABLED)
   (uint32_t) SCI1_TEI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI1_ERI != BSP_IRQ_DISABLED)
   (uint32_t) SCI1_ERI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI1_AM != BSP_IRQ_DISABLED)
   (uint32_t) SCI1_AM_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI2_RXI != BSP_IRQ_DISABLED)
   (uint32_t) SCI2_RXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI2_TXI != BSP_IRQ_DISABLED)
   (uint32_t) SCI2_TXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI2_TEI != BSP_IRQ_DISABLED)
   (uint32_t) SCI2_TEI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI2_ERI != BSP_IRQ_DISABLED)
   (uint32_t) SCI2_ERI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI2_AM != BSP_IRQ_DISABLED)
   (uint32_t) SCI2_AM_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI3_RXI != BSP_IRQ_DISABLED)
   (uint32_t) SCI3_RXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI3_TXI != BSP_IRQ_DISABLED)
   (uint32_t) SCI3_TXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI3_TEI != BSP_IRQ_DISABLED)
   (uint32_t) SCI3_TEI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI3_ERI != BSP_IRQ_DISABLED)
   (uint32_t) SCI3_ERI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI3_AM != BSP_IRQ_DISABLED)
   (uint32_t) SCI3_AM_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI4_RXI != BSP_IRQ_DISABLED)
   (uint32_t) SCI4_RXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI4_TXI != BSP_IRQ_DISABLED)
   (uint32_t) SCI4_TXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI4_TEI != BSP_IRQ_DISABLED)
   (uint32_t) SCI4_TEI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI4_ERI != BSP_IRQ_DISABLED)
   (uint32_t) SCI4_ERI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI4_AM != BSP_IRQ_DISABLED)
   (uint32_t) SCI4_AM_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI5_RXI != BSP_IRQ_DISABLED)
   (uint32_t) SCI5_RXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI5_TXI != BSP_IRQ_DISABLED)
   (uint32_t) SCI5_TXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI5_TEI != BSP_IRQ_DISABLED)
   (uint32_t) SCI5_TEI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI5_ERI != BSP_IRQ_DISABLED)
   (uint32_t) SCI5_ERI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI5_AM != BSP_IRQ_DISABLED)
   (uint32_t) SCI5_AM_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI6_RXI != BSP_IRQ_DISABLED)
   (uint32_t) SCI6_RXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI6_TXI != BSP_IRQ_DISABLED)
   (uint32_t) SCI6_TXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI6_TEI != BSP_IRQ_DISABLED)
   (uint32_t) SCI6_TEI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI6_ERI != BSP_IRQ_DISABLED)
   (uint32_t) SCI6_ERI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI6_AM != BSP_IRQ_DISABLED)
   (uint32_t) SCI6_AM_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI7_RXI != BSP_IRQ_DISABLED)
   (uint32_t) SCI7_RXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI7_TXI != BSP_IRQ_DISABLED)
   (uint32_t) SCI7_TXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI7_TEI != BSP_IRQ_DISABLED)
   (uint32_t) SCI7_TEI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI7_ERI != BSP_IRQ_DISABLED)
   (uint32_t) SCI7_ERI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI7_AM != BSP_IRQ_DISABLED)
   (uint32_t) SCI7_AM_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI8_RXI != BSP_IRQ_DISABLED)
   (uint32_t) SCI8_RXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI8_TXI != BSP_IRQ_DISABLED)
   (uint32_t) SCI8_TXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI8_TEI != BSP_IRQ_DISABLED)
   (uint32_t) SCI8_TEI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI8_ERI != BSP_IRQ_DISABLED)
   (uint32_t) SCI8_ERI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI8_AM != BSP_IRQ_DISABLED)
   (uint32_t) SCI8_AM_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI9_RXI != BSP_IRQ_DISABLED)
   (uint32_t) SCI9_RXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI9_TXI != BSP_IRQ_DISABLED)
   (uint32_t) SCI9_TXI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI9_TEI != BSP_IRQ_DISABLED)
   (uint32_t) SCI9_TEI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI9_ERI != BSP_IRQ_DISABLED)
   (uint32_t) SCI9_ERI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SCI9_AM != BSP_IRQ_DISABLED)
   (uint32_t) SCI9_AM_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SPI0_SPRI != BSP_IRQ_DISABLED)
   (uint32_t) SPI0_SPRI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SPI0_SPTI != BSP_IRQ_DISABLED)
   (uint32_t) SPI0_SPTI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SPI0_SPII != BSP_IRQ_DISABLED)
   (uint32_t) SPI0_SPII_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SPI0_SPEI != BSP_IRQ_DISABLED)
   (uint32_t) SPI0_SPEI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SPI0_SPTEND != BSP_IRQ_DISABLED)
   (uint32_t) SPI0_SPTEND_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SPI1_SPRI != BSP_IRQ_DISABLED)
   (uint32_t) SPI1_SPRI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SPI1_SPTI != BSP_IRQ_DISABLED)
   (uint32_t) SPI1_SPTI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SPI1_SPII != BSP_IRQ_DISABLED)
   (uint32_t) SPI1_SPII_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SPI1_SPEI != BSP_IRQ_DISABLED)
   (uint32_t) SPI1_SPEI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SPI1_SPTEND != BSP_IRQ_DISABLED)
   (uint32_t) SPI1_SPTEND_IRQHandler,
#endif
#if (BSP_IRQ_CFG_QSPI_INTR != BSP_IRQ_DISABLED)
   (uint32_t) QSPI_INTR_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SDHI_MMC0_ACCS != BSP_IRQ_DISABLED)
   (uint32_t) SDHI_MMC0_ACCS_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SDHI_MMC0_SDIO != BSP_IRQ_DISABLED)
   (uint32_t) SDHI_MMC0_SDIO_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SDHI_MMC0_CARD != BSP_IRQ_DISABLED)
   (uint32_t) SDHI_MMC0_CARD_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SDHI_MMC0_ODMSDBREQ != BSP_IRQ_DISABLED)
   (uint32_t) SDHI_MMC0_ODMSDBREQ_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SDHI_MMC1_ACCS != BSP_IRQ_DISABLED)
   (uint32_t) SDHI_MMC1_ACCS_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SDHI_MMC1_SDIO != BSP_IRQ_DISABLED)
   (uint32_t) SDHI_MMC1_SDIO_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SDHI_MMC1_CARD != BSP_IRQ_DISABLED)
   (uint32_t) SDHI_MMC1_CARD_IRQHandler,
#endif
#if (BSP_IRQ_CFG_SDHI_MMC1_ODMSDBREQ != BSP_IRQ_DISABLED)
   (uint32_t) SDHI_MMC1_ODMSDBREQ_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GLCDC_VPOS != BSP_IRQ_DISABLED)
   (uint32_t) GLCDC_VPOS_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GLCDC_L1UNDF != BSP_IRQ_DISABLED)
   (uint32_t) GLCDC_L1UNDF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_GLCDC_L2UNDF != BSP_IRQ_DISABLED)
   (uint32_t) GLCDC_L2UNDF_IRQHandler,
#endif
#if (BSP_IRQ_CFG_DRW_IRQ != BSP_IRQ_DISABLED)
   (uint32_t) DRW_IRQ_IRQHandler,
#endif
#if (BSP_IRQ_CFG_JPEG_JEDI != BSP_IRQ_DISABLED)
   (uint32_t) JPEG_JEDI_IRQHandler,
#endif
#if (BSP_IRQ_CFG_JPEG_JDTI != BSP_IRQ_DISABLED)
   (uint32_t) JPEG_JDTI_IRQHandler,
#endif
};

//Low-level initialization
int __low_level_init(void)
{
   //Point to the beginning of the vector table
   uint32_t *p = __section_begin(".intvec");
   //Set vector table offset
   SCB->VTOR = ((uint32_t) p & SCB_VTOR_TBLOFF_Msk);
   //If return 0, the data sections will not be initialized
   return 1;
}

//Reset handler
void Reset_Handler(void)
{
   //System initialization
   SystemInit();
   //Main entry point
   __iar_program_start();
}

//Default interrupt handler
void Default_Handler(void)
{
   while(1);
}
