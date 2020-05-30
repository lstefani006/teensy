#include "systick.h"

#include "lcd.hpp"
#include "oledfont.hpp"
//#include "bmp.h"

#define ST7789_SLPOUT 0x11
#define ST7789_NORON 0x13
#define ST7789_MADCTL 0x36 // Memory data access control
#define TFT_MAD_RGB 0x08
#define ST7789_COLMOD 0x3A
#define ST7789_PORCTRL 0xB2   // Porch control
#define ST7789_GCTRL 0xB7     // Gate control
#define ST7789_VCOMS 0xBB     // VCOMS setting
#define ST7789_LCMCTRL 0xC0   // LCM control
#define ST7789_VDVVRHEN 0xC2  // VDV and VRH command enable
#define ST7789_VRHS 0xC3      // VRH set
#define ST7789_VDVSET 0xC4    // VDV setting
#define ST7789_FRCTR2 0xC6    // FR Control 2
#define ST7789_PWCTRL1 0xD0   // Power control 1
#define ST7789_PVGAMCTRL 0xE0 // Positive voltage gamma control
#define ST7789_NVGAMCTRL 0xE1 // Negative voltage gamma control
#define ST7789_INVON 0x21
#define ST7789_CASET 0x2A
#define ST7789_RASET 0x2B
#define ST7789_RAMWR 0x2C
#define ST7789_DISPOFF 0x28
#define ST7789_DISPON 0x29
#define TFT_MAD_COLOR_ORDER TFT_MAD_RGB
#define TFT_MAD_MY 0x80
#define TFT_MAD_MX 0x40
#define TFT_MAD_MV 0x20
#define TFT_MAD_ML 0x10

u16 LCD_BACK_COLOR = BLACK;
static u16 S_colstart = 52;
static u16 S_rowstart = 40;
static u16 S_init_height = 240;
static u16 S_init_width = 135;
static u16 S_width = 135;
static u16 S_height = 240;
static u8 S_rotation = 0;

void LCD_Writ_Bus(u8 dat)
{
#if SPI0_CFG == 1
    OLED_CS_Clr();

    while (RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_TBE))
        ;
    spi_i2s_data_transmit(SPI0, dat);
    while (RESET == spi_i2s_flag_get(SPI0, SPI_FLAG_RBNE))
        ;
    spi_i2s_data_receive(SPI0);

    OLED_CS_Set();
#elif SPI0_CFG == 2
    spi_dma_enable(SPI0, SPI_DMA_TRANSMIT);
#else
    u8 i;
    OLED_CS_Clr();
    for (i = 0; i < 8; i++)
    {
        OLED_SCLK_Clr();
        if (dat & 0x80)
            OLED_SDIN_Set();
        else
            OLED_SDIN_Clr();
        OLED_SCLK_Set();
        dat <<= 1;
    }
    OLED_CS_Set();
#endif
}

void LCD_WR_DATA8(u8 dat)
{
    OLED_DC_Set();
    LCD_Writ_Bus(dat);
}

void LCD_WR_DATA(u16 dat)
{
    OLED_DC_Set();
    LCD_Writ_Bus(dat >> 8);
    LCD_Writ_Bus(dat);
}

void LCD_WR_REG(u8 dat)
{
    OLED_DC_Clr();
    LCD_Writ_Bus(dat);
}

void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2)
{
    LCD_WR_REG(0x2a);
    LCD_WR_DATA(x1 + S_colstart);
    LCD_WR_DATA(x2 + S_colstart);
    LCD_WR_REG(0x2b);
    LCD_WR_DATA(y1 + S_rowstart);
    LCD_WR_DATA(y2 + S_rowstart);
    LCD_WR_REG(0x2c);
}

#if SPI0_CFG == 2
/*!
    \brief      configure the DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_parameter_struct dma_init_struct;

    /* SPI0 transmit dma config:DMA0,DMA_CH2 */
    dma_deinit(DMA0, DMA_CH2);
    dma_struct_para_init(&dma_init_struct);

    dma_init_struct.periph_addr = (uint32_t)&SPI_DATA(SPI0);
    dma_init_struct.memory_addr = (uint32_t)image;
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_LOW;
    dma_init_struct.number = FRAME_SIZE;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init(DMA0, DMA_CH2, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH2);
    dma_memory_to_memory_disable(DMA0, DMA_CH2);
}
#endif

#if SPI0_CFG == 1
/*!
    \brief      configure the SPI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_config(void)
{
    spi_parameter_struct spi_init_struct;
    /* deinitilize SPI and the parameters */
    OLED_CS_Set();
    spi_struct_para_init(&spi_init_struct);

    /* SPI0 parameter config */
    spi_init_struct.trans_mode = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode = SPI_MASTER;
    spi_init_struct.frame_size = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE;
    spi_init_struct.nss = SPI_NSS_SOFT;
    spi_init_struct.prescale = SPI_PSC_8;
    spi_init_struct.endian = SPI_ENDIAN_MSB;
    spi_init(SPI0, &spi_init_struct);

    spi_crc_polynomial_set(SPI0, 7);
    spi_enable(SPI0);
}
#endif

void LCD_setRotation(uint8_t m)
{
    S_rotation = m % 4;
    LCD_WR_REG(0x36);
    switch (S_rotation)
    {
    case 0:
        S_colstart = 52;
        S_rowstart = 40;
        S_width = S_init_width;
        S_height = S_init_height;
        LCD_WR_DATA8(TFT_MAD_COLOR_ORDER);
        break;
    case 1:
        S_colstart = 40;
        S_rowstart = 53;
        S_width = S_init_height;
        S_height = S_init_width;
        LCD_WR_DATA8(TFT_MAD_MX | TFT_MAD_MV | TFT_MAD_COLOR_ORDER);
        break;
    case 2:
        S_colstart = 52;
        S_rowstart = 40;
        S_width = S_init_width;
        S_height = S_init_height;
        LCD_WR_DATA8(TFT_MAD_MX | TFT_MAD_MY | TFT_MAD_COLOR_ORDER);
        break;
    case 3:
        S_colstart = 40;
        S_rowstart = 52;
        S_width = S_init_height;
        S_height = S_init_width;
        LCD_WR_DATA8(TFT_MAD_MV | TFT_MAD_MY | TFT_MAD_COLOR_ORDER);
        break;
    }
}

void LCD_Init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);

#if SPI0_CFG == 1
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_SPI0);
    /* SPI0 GPIO config: NSS/PA4, SCK/PA5, MOSI/PA7 */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

    // dovrebbe essere il gpio per accendere il display
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    gpio_bit_set(GPIOB, GPIO_PIN_10);

    spi_config();

#elif SPI0_CFG == 2
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_periph_clock_enable(RCU_SPI0);

    /* SPI0 GPIO config: NSS/PA4, SCK/PA5, MOSI/PA7 */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_7);
    /* SPI0 GPIO config: MISO/PA6 */
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_6);

    dma_config();

    dma_channel_enable(DMA0, DMA_CH2);
#elif SPI0_CFG == 3
    gpio_init(GPIOA, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

    gpio_bit_reset(GPIOA, GPIO_PIN_5 | GPIO_PIN_7);
    gpio_bit_reset(GPIOB, GPIO_PIN_2);
#endif

    gpio_init(GPIOB, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_0 | GPIO_PIN_1);
    gpio_bit_reset(GPIOB, GPIO_PIN_0 | GPIO_PIN_1);

    OLED_RST_Clr();
    delay_1ms(200);
    OLED_RST_Set();
    delay_1ms(20);
    OLED_BLK_Set();

    LCD_WR_REG(ST7789_SLPOUT); // Sleep out
    delay_1ms(120);

    LCD_WR_REG(ST7789_NORON); // Normal display mode on

    //------------------------------display and color format setting--------------------------------//
    LCD_WR_REG(ST7789_MADCTL);
    //LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(TFT_MAD_RGB);

    // JLX240 display datasheet
    LCD_WR_REG(0xB6);
    LCD_WR_DATA8(0x0A);
    LCD_WR_DATA8(0x82);

    LCD_WR_REG(ST7789_COLMOD);
    LCD_WR_DATA8(0x55);
    delay_1ms(10);

    //--------------------------------ST7789V Frame rate setting----------------------------------//
    LCD_WR_REG(ST7789_PORCTRL);
    LCD_WR_DATA8(0x0c);
    LCD_WR_DATA8(0x0c);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x33);
    LCD_WR_DATA8(0x33);

    LCD_WR_REG(ST7789_GCTRL); // Voltages: VGH / VGL
    LCD_WR_DATA8(0x35);

    //---------------------------------ST7789V Power setting--------------------------------------//
    LCD_WR_REG(ST7789_VCOMS);
    LCD_WR_DATA8(0x28); // JLX240 display datasheet

    LCD_WR_REG(ST7789_LCMCTRL);
    LCD_WR_DATA8(0x0C);

    LCD_WR_REG(ST7789_VDVVRHEN);
    LCD_WR_DATA8(0x01);
    LCD_WR_DATA8(0xFF);

    LCD_WR_REG(ST7789_VRHS); // voltage VRHS
    LCD_WR_DATA8(0x10);

    LCD_WR_REG(ST7789_VDVSET);
    LCD_WR_DATA8(0x20);

    LCD_WR_REG(ST7789_FRCTR2);
    LCD_WR_DATA8(0x0f);

    LCD_WR_REG(ST7789_PWCTRL1);
    LCD_WR_DATA8(0xa4);
    LCD_WR_DATA8(0xa1);

    //--------------------------------ST7789V gamma setting---------------------------------------//
    LCD_WR_REG(ST7789_PVGAMCTRL);
    LCD_WR_DATA8(0xd0);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x02);
    LCD_WR_DATA8(0x07);
    LCD_WR_DATA8(0x0a);
    LCD_WR_DATA8(0x28);
    LCD_WR_DATA8(0x32);
    LCD_WR_DATA8(0x44);
    LCD_WR_DATA8(0x42);
    LCD_WR_DATA8(0x06);
    LCD_WR_DATA8(0x0e);
    LCD_WR_DATA8(0x12);
    LCD_WR_DATA8(0x14);
    LCD_WR_DATA8(0x17);

    LCD_WR_REG(ST7789_NVGAMCTRL);
    LCD_WR_DATA8(0xd0);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x02);
    LCD_WR_DATA8(0x07);
    LCD_WR_DATA8(0x0a);
    LCD_WR_DATA8(0x28);
    LCD_WR_DATA8(0x31);
    LCD_WR_DATA8(0x54);
    LCD_WR_DATA8(0x47);
    LCD_WR_DATA8(0x0e);
    LCD_WR_DATA8(0x1c);
    LCD_WR_DATA8(0x17);
    LCD_WR_DATA8(0x1b);
    LCD_WR_DATA8(0x1e);

    LCD_WR_REG(ST7789_INVON);

    LCD_WR_REG(ST7789_CASET); // Column address set
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0xE5); // 239

    LCD_WR_REG(ST7789_RASET); // Row address set
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x00);
    LCD_WR_DATA8(0x01);
    LCD_WR_DATA8(0x3F); // 319

    delay_1ms(120);

    LCD_setRotation(0);

    LCD_WR_REG(ST7789_DISPON); //Display on
    delay_1ms(120);
}

void LCD_Clear(u16 Color)
{
    u16 i, j;
    LCD_Address_Set(0, 0, S_width - 1, S_height - 1);
    for (i = 0; i < S_width; i++)
    {
        for (j = 0; j < S_height; j++)
        {
            LCD_WR_DATA(Color);
        }
    }
}

void LCD_DrawPoint(u16 x, u16 y, u16 color)
{
    LCD_Address_Set(x, y, x, y); //设置光标位置
    LCD_WR_DATA(color);
}

void LCD_DrawPoint_big(u16 x, u16 y, u16 color)
{
    LCD_Fill(x - 1, y - 1, x + 1, y + 1, color);
}

void LCD_Fill(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color)
{
    u16 i, j;
    LCD_Address_Set(xsta, ysta, xend, yend); //设置光标位置
    for (i = ysta; i <= yend; i++)
    {
        for (j = xsta; j <= xend; j++)
            LCD_WR_DATA(color); //设置光标位置
    }
}

void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    u16 t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x2 - x1;
    delta_y = y2 - y1;
    uRow = x1;
    uCol = y1;
    if (delta_x > 0)
        incx = 1;
    else if (delta_x == 0)
        incx = 0;
    else
    {
        incx = -1;
        delta_x = -delta_x;
    }
    if (delta_y > 0)
        incy = 1;
    else if (delta_y == 0)
        incy = 0;
    else
    {
        incy = -1;
        delta_y = -delta_x;
    }
    if (delta_x > delta_y)
        distance = delta_x;
    else
        distance = delta_y;
    for (t = 0; t < u16(distance + 1); t++)
    {
        LCD_DrawPoint(uRow, uCol, color);
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance)
        {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance)
        {
            yerr -= distance;
            uCol += incy;
        }
    }
}

void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 color)
{
    LCD_DrawLine(x1, y1, x2, y1, color);
    LCD_DrawLine(x1, y1, x1, y2, color);
    LCD_DrawLine(x1, y2, x2, y2, color);
    LCD_DrawLine(x2, y1, x2, y2, color);
}

void LCD_DrawCircle(u16 x0, u16 y0, u8 r, u16 color)
{
    int a, b;
    // int di;
    a = 0;
    b = r;
    while (a <= b)
    {
        LCD_DrawPoint(x0 - b, y0 - a, color); //3
        LCD_DrawPoint(x0 + b, y0 - a, color); //0
        LCD_DrawPoint(x0 - a, y0 + b, color); //1
        LCD_DrawPoint(x0 - a, y0 - b, color); //2
        LCD_DrawPoint(x0 + b, y0 + a, color); //4
        LCD_DrawPoint(x0 + a, y0 - b, color); //5
        LCD_DrawPoint(x0 + a, y0 + b, color); //6
        LCD_DrawPoint(x0 - b, y0 + a, color); //7
        a++;
        if ((a * a + b * b) > (r * r))
        {
            b--;
        }
    }
}

void LCD_ShowChar(u16 x, u16 y, u8 num, u8 mode, u16 color)
{
    u8 temp;
    u8 pos, t;
    u16 x0 = x;
    if (x > S_width - 16 || y > S_height - 16)
        return;
    num = num - ' ';
    LCD_Address_Set(x, y, x + 8 - 1, y + 16 - 1);
    if (!mode)
    {
        for (pos = 0; pos < 16; pos++)
        {
            temp = asc2_1608[(u16)num * 16 + pos];
            for (t = 0; t < 8; t++)
            {
                if (temp & 0x01)
                    LCD_WR_DATA(color);
                else
                    LCD_WR_DATA(LCD_BACK_COLOR);
                temp >>= 1;
                x++;
            }
            x = x0;
            y++;
        }
    }
    else
    {
        for (pos = 0; pos < 16; pos++)
        {
            temp = asc2_1608[(u16)num * 16 + pos];
            for (t = 0; t < 8; t++)
            {
                if (temp & 0x01)
                    LCD_DrawPoint(x + t, y + pos, color);
                temp >>= 1;
            }
        }
    }
}

void LCD_ShowString(u16 x, u16 y, const u8 *p, u16 color)
{
    while (*p != '\0')
    {
        if (x > S_width - 16)
        {
            x = 0;
            y += 16;
        }
        if (y > S_height - 16)
        {
            y = x = 0;
            LCD_Clear(RED);
        }
        LCD_ShowChar(x, y, *p, 0, color);
        x += 8;
        p++;
    }
}

static u32 LCD_mypow(u8 m, u8 n)
{
    u32 result = 1;
    while (n--)
        result *= m;
    return result;
}

void LCD_ShowNum(u16 x, u16 y, u16 num, u8 len, u16 color)
{
    u8 t, temp;
    u8 enshow = 0;
    for (t = 0; t < len; t++)
    {
        temp = (num / LCD_mypow(10, len - t - 1)) % 10;
        if (enshow == 0 && t < (len - 1))
        {
            if (temp == 0)
            {
                LCD_ShowChar(x + 8 * t, y, ' ', 0, color);
                continue;
            }
            else
                enshow = 1;
        }
        LCD_ShowChar(x + 8 * t, y, temp + 48, 0, color);
    }
}

void LCD_ShowNum1(u16 x, u16 y, float num, u8 len, u16 color)
{
    u8 t, temp;
    // u8 enshow=0;
    u16 num1;
    num1 = num * 100;
    for (t = 0; t < len; t++)
    {
        temp = (num1 / LCD_mypow(10, len - t - 1)) % 10;
        if (t == (len - 2))
        {
            LCD_ShowChar(x + 8 * (len - 2), y, '.', 0, color);
            t++;
            len += 1;
        }
        LCD_ShowChar(x + 8 * t, y, temp + 48, 0, color);
    }
}

/*
extern unsigned char image[12800];
void LCD_ShowPicture(u16 x1, u16 y1, u16 x2, u16 y2)
{
    int i;
    LCD_Address_Set(x1, y1, x2, y2);
    for (i = 0; i < 12800; i++) {
        LCD_WR_DATA8(image[i]);
    }
}
*/
