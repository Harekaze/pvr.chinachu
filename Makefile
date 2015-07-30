STRIP = strip
ADDONNAME = PVRChinachu.pvr
VERSION = 0.0.1

.PHONY: all
all:
	cd src; make
	cp -r template/pvr.chinachu .
	cp src/$(ADDONNAME) pvr.chinachu/$(ADDONNAME)
	$(STRIP) -x pvr.chinachu/$(ADDONNAME)
	zip -9 -r ./pvr.chinachu.$(VERSION).zip pvr.chinachu
	rm -Rf pvr.chinachu

.PHONY: clean
clean:
	cd src; make clean
	$(RM) -f ./pvr.chinachu.$(VERSION).zip
