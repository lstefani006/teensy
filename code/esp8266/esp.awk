
BEGIN { FS="."; }
/^generic.menu.FlashSize/	{ 
					if (NR==4 || NR=5) if ((p=index($4,"="))>0) 
					{
						x = substr($4,1,p-1);
						y = substr($4,p+1);
						printf("FLASH_DEF=%-23s %s\n", x, y)
					}
				}
/^generic.menu.LwIPVariant/	{ 
					if (NR==4 || NR=5) if ((p=index($4,"="))>0) 
					{
						x = substr($4,1,p-1);
						y = substr($4,p+1);
						printf("LWIP_VARIANT=%-20s %s%s\n", x, y, $5)
					}
				}
