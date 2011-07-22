#
#      Makefile
#      
#      Copyright 2011 Juan I Carrano <juan@superfreak.com.ar>
#

OPTIFLAGS = -O2 -march=native
CFLAGS = $(OPTIFLAGS) -pedantic -Wall -Wno-variadic-macros -I.. -I/home/juan/source

LIBTOOL_COMPILE = libtool --mode=compile --tag=cc $(CC) $(CFLAGS)
LIBTOOL_LINK = libtool --mode=link --tag=cc $(CC) $(CFLAGS)
LIBTOOL_INSTALL = libtool --mode=install install
LIBTOOL_CLEAN = libtool --mode=clean rm -f

LIB_PATH = /usr/local/lib
ADDITIONAL_LIBS = -lm -lchain
LIB_NAME = libexpr.la
LIB_NAME2 = libexpr_x_math.la

LIBTOOL_FINISH = libtool finish $(LIB_PATH)
LIBTOOL_UNINSTALL = libtool --mode=uninstall rm

#redefino reglas implicitas para usar libtool
%.lo : %.c ; $(LIBTOOL_COMPILE) -c $< -o $@

objects = vm.lo parse.lo expr.lo assemble.lo compile.lo errors.lo input.lo \
		main.lo

extra_obj = extra-math.lo main.lo

#hacer la libreria

all : $(objects)
	$(LIBTOOL_LINK) -o $(LIB_NAME) $(objects) -rpath $(LIB_PATH) \
		$(ADDITIONAL_LIBS)

extra : $(extra_obj)
	$(LIBTOOL_LINK) -o $(LIB_NAME2) $(extra_obj) -rpath $(LIB_PATH) \
		$(ADDITIONAL_LIBS)

clean : 
	$(LIBTOOL_CLEAN) $(LIB_NAME)
	$(LIBTOOL_CLEAN) $(LIB_NAME2)
	find ./ -name "*.o" -exec rm "{}" \;
	find ./ -name "*.lo" -exec rm "{}" \;
	

## cuidado! !!!!! ########

install: $(LIB_NAME)
	$(LIBTOOL_INSTALL) $(LIB_NAME) $(LIB_PATH)/$(LIB_NAME)
	$(LIBTOOL_FINISH)

uninstall:
	$(LIBTOOL_UNINSTALL) $(LIB_PATH)/$(LIB_NAME)

install_extra: $(LIB_NAME2)
	$(LIBTOOL_INSTALL) $(LIB_NAME2) $(LIB_PATH)/$(LIB_NAME2)
	$(LIBTOOL_FINISH)

uninstall_extra:
	$(LIBTOOL_UNINSTALL) $(LIB_PATH)/$(LIB_NAME2)

##help!!!

help :
	@echo \?\?\?

# fin
