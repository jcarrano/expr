#
#      Makefile
#      
#      Copyright 2011 Juan I Carrano <juan@superfreak.com.ar>
#

all : libexpr.la libexpr_x_math.la

include ../Makefile.lt

#OPTIFLAGS = -O2 -march=native
#ADDITIONAL_LIBS = -lm -lchain
ADDITIONAL_LIBS = -lm

objects = vm.lo parse.lo expr.lo assemble.lo compile.lo errors.lo input.lo chain.lo

extra_obj = extra-math.lo

#definir dependencias

libexpr.la : $(objects)

libexpr_x_math.la : $(extra_obj)


## cuidado! !!!!! ########

install: libexpr.la.install libexpr_x_math.la.install

uninstall: libexpr.la.uninstall libexpr_x_math.la.uninstall
	

##help!!!

help :
	@echo \?\?\?

# fin
