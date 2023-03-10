DOC		=	theo.adoc
HTML	=	index.html

doc:
	@printf "$(YELLOW)Generating documentations..$(DEFAULT)\n"
	@asciidoctor $(DOC) -o $(HTML)

docdocker:
	@printf "$(YELLOW)launch the asciidoctor/docker-asciidoctor docker image..$(DEFAULT)\n"
	@docker run --rm -v $(shell pwd):/documents/ asciidoctor/docker-asciidoctor make doc
