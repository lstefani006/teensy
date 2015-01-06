all : 
	@make -C TeensyLib
	@make -C Test
	@make -C Blink
	@make -C RTC_Set
	@make -C 522
	@make -C SDFat
	@make -C NRF24L01
	@make -C Nokia-5110

clean : 
	@make -C TeensyLib  clean
	@make -C Test       clean
	@make -C Blink      clean
	@make -C RTC_Set    clean
	@make -C 522        clean
	@make -C SDFat      clean
	@make -C NRF24L01   clean
	@make -C Nokia-5110 clean
