
all: proj4.tex proj4.bib
	latex proj4.tex
	bibtex proj4
	latex proj4.tex
	latex proj4.tex proj4.dvi
	dvips -t a4 proj4.dvi
	ps2pdf proj4.ps proj4.pdf

diff:
	diff-pdf --output-diff=diff.pdf vzor.pdf proj4.pdf || true

clean:
	rm -rfv *.ps *.log *.dvi *.aux proj4.pdf *.out *.zip *.bbl *.blg

zip:
	zip xcoufa09-fit.zip Makefile proj4.tex proj4.bib czplain.bst
