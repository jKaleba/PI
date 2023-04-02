#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#define MAX_LINE 128

// consider chars from [FIRST_CHAR, LAST_CHAR)
#define FIRST_CHAR 33
#define LAST_CHAR 127
#define MAX_CHARS (LAST_CHAR - FIRST_CHAR)
#define MAX_BIGRAMS ((LAST_CHAR - FIRST_CHAR) * (LAST_CHAR - FIRST_CHAR))

#define NEWLINE '\n'
#define TAB '\t'
#define EOS '\0'

int count[MAX_CHARS] = {0};
int count_di[MAX_BIGRAMS] = {0};

// sort indices according to their respective counts.
// sort alphabetically if counts equal
int cmp (const void *a, const void *b) {
	int va = *(int*)a;
	int vb = *(int*)b;
	if (count[va] == count[vb]) return va - vb;	return count[vb] - count[va];
}

// sort indices according to their respective counts.
// sort alphabetically if counts equal
int cmp_di (const void *a, const void *b) {
//    zmienione count -> count_di
	int va = *(int*)a;
	int vb = *(int*)b;
	// sort according to second char if counts and the first char equal
	if (count_di[va] == count_di[vb] && va / MAX_CHARS == vb / MAX_CHARS) return va % MAX_CHARS - vb % MAX_CHARS;
	// sort according to first char if counts equal
	if (count_di[va] == count_di[vb]) return va / MAX_CHARS - vb / MAX_CHARS;
	return count_di[vb] - count_di[va];
}


//Szablon programu należy uzupełnić o definicję funkcji wc(), która czyta tekst ze standardowego
//wejścia i na tej podstawie zlicza linie, słowa oraz znaki, występujące w tym tekście, podobnie jak
//komenda wc systemu Unix. Słowo to ciąg znaków oddzielony spacją, tabulatorem lub znakiem
//nowej linii.
#define TRUE 1
#define FALSE 0
void wc(int *nl, int *nw, int *nc) {
    *nl = 0;
    *nw = 0;
    *nc = 0;
    char buffer[MAX_LINE];
    int new_word = FALSE;
    while(fgets(buffer, MAX_CHARS, stdin) != NULL) {
        for(int i = 0; buffer[i] != EOS; i++) {
            (*nc)++;
            if(buffer[i] == ' ' || buffer[i] == TAB) {
                new_word = FALSE;
            }
            else if(buffer[i] == NEWLINE) {
                (*nl)++;
                new_word = FALSE;
            } else {
                if(new_word == FALSE) {
                    (*nw)++;
                    new_word = TRUE;
                }
            }
        }
    }
}


//Szablon programu należy uzupełnić o definicję funkcji char count(), która czyta tekst ze
//standardowego wejścia i na tej podstawie zlicza krotności znaków występujących w tym tekści
void char_count(int char_no, int *n_char, int *cnt) {
    char buffer[MAX_LINE];

    while(fgets(buffer, MAX_LINE, stdin) != NULL) {
        for(int i = 0; buffer[i] != EOS; i++) {
            if(buffer[i] >= FIRST_CHAR && buffer[i] < LAST_CHAR) {
                count[(int)buffer[i] - FIRST_CHAR]++;
            }
        }
    }
    int index[MAX_CHARS];
    for(int i = 0; i < MAX_CHARS; i++) {
        index[i] = i;
    }

    qsort(index, MAX_CHARS, sizeof(int), cmp);

    *n_char = index[char_no - 1] + FIRST_CHAR;
    *cnt = count[index[char_no - 1]];
}


//Szablon programu należy uzupełnić o definicję funkcji digram count(), która czyta tekst ze
//standardowego wejścia i na tej podstawie zlicza krotności digramów znakowych (par znaków)
//występujących w tym tekście.
void bigram_count(int bigram_no, int bigram[]) {
    char buffer[MAX_LINE];

    while(fgets(buffer, MAX_LINE, stdin) != NULL) {
        for(int i = 1; buffer[i - 1] != EOS && buffer[i] != EOS; i++) {
            if(buffer[i] >= FIRST_CHAR && buffer[i] < LAST_CHAR
                && buffer[i - 1] >= FIRST_CHAR && buffer[i - 1] < LAST_CHAR) {
                count_di[((int)buffer[i - 1] - FIRST_CHAR) * MAX_CHARS + ((int)buffer[i] - FIRST_CHAR)]++;
            }
        }
    }

    int index[MAX_BIGRAMS];
    for(int i = 0; i < MAX_BIGRAMS; i++) {
        index[i] = i;
    }

    qsort(index, MAX_BIGRAMS, sizeof(int), cmp_di);
    bigram[0] = index[bigram_no - 1] / MAX_CHARS + FIRST_CHAR;
    bigram[1] = index[bigram_no - 1] % MAX_CHARS + FIRST_CHAR;
    bigram[2] = count_di[index[bigram_no - 1]];
}


//Szablon programu należy uzupełnić o definicję funkcji find comments(), która czyta ze standardowego wejścia ciąg znaków stanowiący program w języku C. Funkcja zlicza komentarze
//blokowe (/* ... */) i jednoliniowe (// ...) w przeczytanym tekście i zwraca uzyskane liczby
//do funkcji main() przy użyciu parametrów.
//Zagnieżdżone komentarze nie są liczone, czyli np. następujący fragment kodu:
///*// tekst*/
//jest uważany za jeden komentarz blokowy.
//Można założyć, że wszystkie komentarze blokowe są prawidłowo zamknięte.
void find_comments(int *line_comment_counter, int *block_comment_counter) {

    *line_comment_counter = 0;
    *block_comment_counter = 0;
    char buffer[MAX_LINE];
    int blockComment = FALSE;
    int line = 0, blockComLine, blockComBegin;
    while(fgets(buffer, MAX_LINE, stdin) != NULL) {
        line++;
        for(int i = 1; buffer[i - 1] != EOS && buffer[i] != EOS; i++) {
            if(buffer[i - 1] == '/' && buffer[i] == '/' && blockComment == FALSE) {
                (*line_comment_counter)++;
                break;
            }
            else if(blockComment == FALSE) {
                if(buffer[i - 1] == '/' && buffer[i] == '*') {
                    blockComment = TRUE;
                    blockComLine = line;
                    blockComBegin = i - 1;
                    (*block_comment_counter)++;
                }
            } else {
                if (buffer[i - 1] == '*' && buffer[i] == '/') {
                    if(line != blockComLine) {
                        blockComment = FALSE;
                    } else {
                        if(i - blockComBegin >  2) {
                            blockComment = FALSE;
                        }
                    }
                }
            }
        }
    }
}

int read_int() {
	char line[MAX_LINE];
	fgets(line, MAX_LINE, stdin); // to get the whole line
	return (int)strtol(line, NULL, 10);
}

int main(void) {
	int to_do;
	int nl, nw, nc, char_no, n_char, cnt;
	int line_comment_counter, block_comment_counter;
	int bigram[3];

	to_do = read_int();
	switch (to_do) {
		case 1: // wc()
			wc (&nl, &nw, &nc);
			printf("%d %d %d\n", nl, nw, nc);
			break;
		case 2: // char_count()
			char_no = read_int();
			char_count(char_no, &n_char, &cnt);
			printf("%c %d\n", n_char, cnt);
			break;
		case 3: // bigram_count()
			char_no = read_int();
			bigram_count(char_no, bigram);
			printf("%c%c %d\n", bigram[0], bigram[1], bigram[2]);
			break;
		case 4:
			find_comments(&line_comment_counter, &block_comment_counter);
			printf("%d %d\n", block_comment_counter, line_comment_counter);
			break;
		default:
			printf("NOTHING TO DO FOR %d\n", to_do);
			break;
	}
	return 0;
}
