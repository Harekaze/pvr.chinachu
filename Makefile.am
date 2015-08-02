SUBDIRS = src

ACLOCAL_AMFLAGS = -I m4

all:
	cp -r template/pvr.chinachu .
	cp ChangeLog.txt pvr.chinachu/
	cp LICENSE pvr.chinachu/
	cp src/$(ADDONNAME) pvr.chinachu/$(ADDONNAME)
	$(STRIP) -x pvr.chinachu/$(ADDONNAME)
	zip -9 -r ./pvr.chinachu.$(VERSION).zip pvr.chinachu
	rm -rf pvr.chinachu

clean-local:
	rm -f ./pvr.chinachu.$(VERSION).zip
