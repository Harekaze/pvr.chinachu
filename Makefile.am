SUBDIRS = src

ACLOCAL_AMFLAGS = -I m4

.PHONY: all
all:
	$(MAKE) -C ./src
	mkdir -p dist
	cp -r template/pvr.chinachu dist/
	cp ChangeLog.txt dist/pvr.chinachu/
	cp LICENSE dist/pvr.chinachu/
	cp src/$(ADDONNAME) dist/pvr.chinachu/$(ADDONNAME)
	$(STRIP) -x dist/pvr.chinachu/$(ADDONNAME)

.PHONY: release
release: all
	cd dist; zip -9 -r ../pvr.chinachu.zip ./pvr.chinachu

.PHONY: clean
clean:
	$(MAKE) clean -C ./src
	rm -rf ./dist/
	rm -f ./pvr.chinachu.zip
