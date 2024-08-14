NAME := hiragana

$(NAME):
	cc $(NAME).c -o $(NAME)
	./$(NAME) 25 4 6
	rm $(NAME)
