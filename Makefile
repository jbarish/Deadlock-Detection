# Name of your program:
NAME=deadlock

# List of all .cpp source code files included in your program (separated by spaces):
SRC=main.c 



SRCPATH=./
OBJ=$(addprefix $(SRCPATH), $(SRC:.c=.o))
CC=gcc
RM=rm -f
INCPATH=includes
CFLAGS+= -I $(INCPATH) -std=c99


all: $(OBJ)
	gcc $(OBJ) -o $(NAME)

clean:
	-$(RM) *~
	-$(RM) *#*
	-$(RM) *swp
	-$(RM) *.core
	-$(RM) *.stackdump
	-$(RM) $(SRCPATH)*.o
	-$(RM) $(SRCPATH)*.obj
	-$(RM) $(SRCPATH)*~
	-$(RM) $(SRCPATH)*#*
	-$(RM) $(SRCPATH)*swp
	-$(RM) $(SRCPATH)*.core
	-$(RM) $(SRCPATH)*.stackdump

fclean: clean
	-$(RM) $(NAME)

re: fclean all

run:
	./deadlock allocation.txt request.txt "0 1 0 2 1"
	./deadlock allocation.txt request.txt "1 1 0 2 1"
	./deadlock allocation.txt request.txt "0 2 1 2 1"
	./deadlock allocationD.txt requestD.txt "0 1 0 2 1"

