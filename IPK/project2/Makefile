PROGS=ipkhttpserver ipkhttpclient
TESTS=

all:
	chmod +x $(PROGS)

.PHONY: clean test pack
clean:
	rm -rfv *.payload *.header *.log
test:
	echo 'test'
pack:
	tar czf xcoufa09.tgz $(PROGS) $(TESTS)
