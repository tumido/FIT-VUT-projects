all: proj2.tex
#	@echo -e "\e[1;92mRunning 'latex' for the 1st time\e[0m"
	latex proj2.tex
#	@echo -e "\e[1;92mRunning 'latex' for the 2st time\e[0m"
	latex proj2.tex proj2.dvi
#	@echo -e "\e[1;92mRunning 'dvips'\e[0m"
	dvips -t a4 proj2.dvi
#	@echo -e "\e[1;92mRunning 'ps2pdf'\e[0m"
	ps2pdf proj2.ps proj2.pdf
#	@echo -e "\e[1;92mDone\e[0m"

diff:
	diff-pdf --output-diff=diff.pdf vzor.pdf proj2.pdf || true

clean:
	rm -rfv *.ps *.log *.dvi *.aux proj2.pdf *.out *.zip

zip:
	zip xcoufa09-fit.zip Makefile proj2.tex
