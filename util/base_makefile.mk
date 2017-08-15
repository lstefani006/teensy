W?=4

ifeq ($(shell test $(W) -ge 1; echo $$?),0)
	WCOMMON_1=-Wreturn-type
	WC+=$(WCOMMON_1)
	WCXX+=$(WCOMMON_1)
endif

ifeq ($(shell test $(W) -ge 2; echo $$?),0)
	WCOMMON_2= -Wall \
			 -Wdouble-promotion \
			 -Wformat \
			 -Winit-self \
			 -Wmissing-include-dirs \
			 -Wparentheses \
			 -Wswitch-enum \
			 -Wuninitialized \
			 -Wmaybe-uninitialized \
			 -Wstrict-overflow \
			 -Wshadow \
			 -Wcast-qual \
			 -Wwrite-strings \
			 -Wlogical-op \
			 -Wwrite-strings \
			 -Wcast-align \
			 -Wcast-qual \
			 -Wpointer-arith \
			 -Wstrict-aliasing \
			 -Wformat \
			 -Wmissing-include-dirs \
			 -Wno-unused-parameter \
			 -Wuninitialized -Wredundant-decls
	WC+= $(WCOMMON_2) \
		 -Wstrict-prototypes 
	WCXX+= $(WCOMMON_2) \
		 -Wuseless-cast \
		 -Wzero-as-null-pointer-constant
endif

ifeq ($(shell test $(W) -ge 3; echo $$?),0)
	WCOMMON_3= -Wextra \
			 -Wempty-body
	WC+=$(WCOMMON_3)
	WCXX+=$(WCOMMON_3)
endif

ifeq ($(shell test $(W) -ge 4; echo $$?),0)
	# -Wpedantic emette warnings per violazioni ISC C++... ma non aiuta molto
	WCOMMON_4= \
			   -Waggregate-return \
			   -Wconversion
	WC+=$(WCOMMON_4)
	WCXX+=$(WCOMMON_4) -Weffc++
endif


