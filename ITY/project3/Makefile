all: proj3.tex
#	@echo -e "\e[1;92mRunning 'latex' for the 1st time\e[0m"
	latex proj3.tex
#	@echo -e "\e[1;92mRunning 'latex' for the 2st time\e[0m"
	latex proj3.tex proj3.dvi
#	@echo -e "\e[1;92mRunning 'dvips'\e[0m"
	dvips -t a4 proj3.dvi
#	@echo -e "\e[1;92mRunning 'ps2pdf'\e[0m"
	ps2pdf proj3.ps proj3.pdf
#	@echo -e "\e[1;92mDone\e[0m"

diff:
	diff-pdf --output-diff=diff.pdf vzor.pdf proj3.pdf || true

clean:
	rm -rfv *.ps *.log *.dvi *.aux proj3.pdf *.out *.zip

zip:
	zip xcoufa09-fit.zip Makefile proj3.tex *.eps algorithm2e.sty
