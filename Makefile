INSTALL_INCLUDE = /usr/local/include/expr

all:
	$(MAKE) -C src

install:
	$(MAKE) -C src install
	mkdir -p $(INSTALL_INCLUDE)
	cp *.h $(INSTALL_INCLUDE)/

uninstall:
	$(MAKE) -C src uninstall
	rm -rf $(INSTALL_INCLUDE)

clean:
	$(MAKE) -C src clean

.PHONY: all metainstall install clean
