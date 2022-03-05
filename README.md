Eduard Petcu - 314CC

===========================

Pentru a memora textul am construit o lista de linii astfel incat
in nodul fiecarei linie se va memora un caracter. Liniile la randul lor
vor fi noduri in lista principala. Am creat de asemenea o structura de date
speciala denumita "pozitie" pentru pozitia cursorului care retine linia pe
care se afla acesta (prin campul x) si indicele caracterului pe care se 
afla in acea linie (prin campul y) si doua structuri de tip stiva in care
tipurile de data retinute sunt siruri de caractere alocate static, char si
pozitie.

In programul meu am implementat urmatoarele
functii:
"::i" - schimba valoarea variabilei "text_mode" care ne spune daca urmatoarele
linii inserate de catre utilizator sunt comenzi sau text.

"s" - deschide fisierul de output si afiseaza textul memorat

"b" - sterge caracterul de pe pozitia cursorului si decrementeaza valoarea

"dl" - am eliminat nodul de tip linie de la pozitia cursorului sau de la 
pozitia data prin parametru

"gl + gc" - am schimbat valoarea variabilei cursor.

"d [chars] " - spre deosebire de implementarea functiei "b", cursorul ramane
constant si sterge caracterul urmator fata de pozitia cursorului. Parametrul
optional introdus de utilizator arata de cate ori se executa functia.

"re + ra old_word new_word" - le-am implementat in aceeasi functie. Textul 
este parcurs de la pozitia cursorului pana la final, si verifica aparitia
unui old_word. In cazul in care acesta este gasit, se inlocuiesc primele n 
caractere din old_word cu primele n caractere din new_word, unde n reprezinta
minimul lungimilor celor 2 cuvinte. Daca primul cuvant este mai mare, atunci 
trebuie sterse n1-n caractere, iar daca al doilea cuvant este mai mare, trebuie
inserate celelalte n2-n caractere din new_word (unde n1 si n2 sunt lungimile
cuvantului vechi si cuvantului nou). Textul este apoi parcurs din nou de la 
inceput pana la pozitia cursorului initial.

"dw + da word" - la fel ca pentru functia de replace all, am cautat cuvantul
in text si l-am sters folosindu-ma de functia deleteMoreChars implementata
pentru functia "d [chars]" trimitand ca parametru lungimea cuvantului de sters.
Am retinut de asemenea intr-o stiva pozitiile la care s-au facut stergerile 
pentru a putea implementa functia undo.

"u-undo" - realizeaza functia inversa a tuturor functiilor implementate anterior.
La fiecare apelare a unei functie se vor retine in 2 stive pozitia initiala 
a cursorului si functia apelata. La apelarea comenzii undo se revine la pozitia
din varful stivei de pozitii ale cursorului si insereaza in varful stivelor de
redo pozitia cursorului initiala si comanda pe care se face undo.

"r-redo" - reapeleaza functia din varful stivei redo. Cursorul ia valoarea
din varful stivei de indici de redo.
