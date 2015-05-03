PROGS=ipkhttpserver ipkhttpclient
TESTS=www test_it.sh

all:
	chmod +x $(PROGS)

.PHONY: clean test pack
.SILENT: clean
clean:
	rm -rf *.payload *.header *.log
test:
	bash test_it.sh
pack:
	tar czf xcoufa09.tgz $(PROGS) $(TESTS)
