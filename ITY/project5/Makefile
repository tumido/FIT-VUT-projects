FILE=proj5

all: $(FILE).tex
	latex $(FILE).tex
	latex $(FILE).tex $(FILE).dvi
	dvips $(FILE).dvi
	ps2pdf $(FILE).ps $(FILE).pdf

diff:
	diff-pdf --output-diff=diff.pdf vzor.pdf $(FILE).pdf || true

clean:
	rm -rfv *.ps *.log *.dvi *.aux *.toc *.nav *.snm $(FILE).pdf *.out *.zip

zip:
		zip xcoufa09-fit.zip Makefile schema-pocitace-sapo.eps $(FILE).tex
