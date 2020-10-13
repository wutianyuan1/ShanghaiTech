BASENAME=HW_solution
NAME=$(BASENAME).tex
PDF=$(BASENAME).pdf
LATEX_COMPILER=xelatex
LATEX_FLAGS=-shell-escape
BIB_COMPILER=bibtex

all: 
	$(LATEX_COMPILER) $(LATEX_FLAGS) $(NAME)

clean: 
	sh clean.sh
