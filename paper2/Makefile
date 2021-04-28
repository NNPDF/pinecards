default:
ifeq (, $(shell which latexmk))
	pdflatex paper
	bibtex paper
	pdflatex paper
	pdflatex paper	
else
	latexmk -pdf paper
endif

clean:
	rm -rf *~ *.out *.toc *.log *.aux *.blg *.bbl *.fdb_latexmk *.fls
