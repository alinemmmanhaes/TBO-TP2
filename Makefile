path1 = "casos_teste/caso_teste_1.txt"
path2 = "casos_teste/caso_teste_2.txt"
path3 = "casos_teste/caso_teste_3.txt"
path4 = "casos_teste/caso_teste_4.txt"

input = $(path1)
output = "saida.txt"

all:
	make comp
	make run

comp:
	@ gcc *.c -o bt

run:
	clear
	./bt  $(input) $(output)

val:
	clear
	make comp
	valgrind ./bt $(input) $(output)

## valgrind --leak-check=full --show-leak-kinds=all

clean:
	@ rm -f *.o BT bt main
