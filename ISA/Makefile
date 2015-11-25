PROGS=antipirat
require=requirements.txt
txt=`ls *.txt | grep -v $(require)`

all:
	pip install --user -r $(require)
	chmod +x $(PROGS)

.PHONY: clean pack
.SILENT: clean
clean:
	rm -rf $(txt) *.peerlist *.torrent *.xml Util/__pycache__
pack:
	tar czf xcoufa09.tgz $(PROGS) Makefile Util BencodePy README* manual.pdf $(require)
