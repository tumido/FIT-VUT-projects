all: proj1.tex
	latex proj1.tex proj1.dvi
	dvips -t a4 proj1.dvi
	ps1pdf proj1.ps proj1.pdf

diff:
	diff-pdf --output-diff=diff.pdf vzor.pdf proj1.pdf || true

clean:
	rm -rfv *.ps *.log *.dvi *.aux proj1.pdf *.out *.zip

zip:
	zip xcoufa09-fit.zip Makefile proj1.tex
