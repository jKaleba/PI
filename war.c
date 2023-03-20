#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

enum state { OK = 0, UNDERFLOW = -1, OVERFLOW = -2 };

#define colors 4
#define figures 13
#define activePlayers 2
int l = colors * figures;

struct CyclicBuffer {
    int cards[colors * figures];
    int out; //next index
    int len; //current cards state
};

int push(struct CyclicBuffer *buffer, int card) {
    if(buffer->len == colors * figures) {
        return OVERFLOW;
    }
    buffer->cards[(buffer->out + buffer->len++) % l] = card;
    return OK;
}

int pop(struct CyclicBuffer *buffer) {
    if(buffer->len == 0) {
        return UNDERFLOW;
    }
    int x = buffer->cards[buffer->out];
    buffer->out = (buffer->out + 1) % l;
    buffer->len--;
    return x;
}

int cardValue(int code) {
    return code / colors;
}

int myRandom(int low, int top) {
    if(low > top) {
        return INT_MIN;
    }
    if(top - low > RAND_MAX) {
        return INT_MAX;
    }
    if(low == top) {
        return low;
    }

    return low + rand() % (top - low + 1);
}

void swap(struct CyclicBuffer *buffer, int i, int j) {
    int t = buffer->cards[i];
    buffer->cards[i] = buffer->cards[j];
    buffer->cards[j] = t;
}

void shuffle(struct CyclicBuffer *buffer) {
    for(int i = buffer->out; i < buffer->out + buffer->len - 1; i++) {
        int k = myRandom(i, buffer->out + buffer->len - 1) % l;
        swap(buffer, i, k);
    }
}

void fillDeck(struct CyclicBuffer *buffer) {
    for(int i = 0; i < colors * figures; i++) {
        push(buffer, i);
    }
}

void deal(struct CyclicBuffer deck, struct CyclicBuffer buffer[]) {
    for(int i = 0; i < activePlayers; i++) {
        for(int j = 0; j < colors * figures / activePlayers; j++) {
            push(&buffer[i], deck.cards[i * colors * figures / activePlayers + j]);
        }
    }
}

void printBuffer(struct CyclicBuffer *buffer) {
    for(int i = 0; i < buffer->len; i++) {
        printf("%d ", buffer->cards[(buffer->out + i) % l]);
    }
    printf("\n");
}

enum boolean { true = 1, false = 0};
int hasCards(struct CyclicBuffer buffer) {
    return buffer.len > 0 ? true : false;
}

int war(struct CyclicBuffer conflictBuffer) {
    return cardValue(conflictBuffer.cards[conflictBuffer.out + conflictBuffer.len - 2]) == cardValue(conflictBuffer.cards[conflictBuffer.out + conflictBuffer.len - 1])
    ?
    true : false;
}

int has2Cards(struct CyclicBuffer buffer) {
    return buffer.len >= 2 ? true : false;
}

enum dir { TIE = -1, LEFT = 0, RIGHT = 1 };
int winner(struct CyclicBuffer conflictBuffer) {
    int left = cardValue(conflictBuffer.cards[(conflictBuffer.out + conflictBuffer.len - 2) % l]);
    int right = cardValue(conflictBuffer.cards[(conflictBuffer.out + conflictBuffer.len - 1) % l]);

    if(left > right) return LEFT;
    if(left == right) return TIE;
    return RIGHT;
}

int main(void) {
    int seed, version, maxConflicts;

    scanf("%d%d%d", &seed, &version, &maxConflicts);
    srand(seed);

    struct CyclicBuffer deck;
    deck.len = 0;
    deck.out = 0;
    fillDeck(&deck);
    shuffle(&deck);

    struct CyclicBuffer players[activePlayers];
    for(int i = 0; i < activePlayers; i++) {
        players[i].len = 0;
        players[i].out = 0;
    }
    deal(deck, players);

    struct CyclicBuffer conflictBuffer;
    conflictBuffer.len = 0;
    conflictBuffer.out = 0;

    int conflicts = 0;

    if(version == 0) {
        while(hasCards(players[0]) == true && hasCards(players[1]) == true && conflicts < maxConflicts) {
            for(int i = 0; i < activePlayers; i++) {
                push(&conflictBuffer, pop(&players[i]));
            }
            conflicts++;

            while(has2Cards(players[0]) == true && has2Cards(players[1]) == true && conflicts < maxConflicts && war(conflictBuffer) == true) {
                for(int i = 0; i < 2; i++) {
                    for(int j = 0; j < activePlayers; j++) {
                        push(&conflictBuffer, pop(&players[j]));
                    }
                }
                conflicts++;
            }

            int result = winner(conflictBuffer);
            if(result == TIE) {
                for(int i = 0; i < conflictBuffer.len / 2; i++) {
                    push(&players[0], pop(&conflictBuffer));
                    push(&players[1], pop(&conflictBuffer));
                }
                break;
            }
            else if(result == LEFT) {
                int n = conflictBuffer.len;
                int temp[n];
                for(int i = 0; i < n / 2; i++) {
                    temp[i] = pop(&conflictBuffer);
                    temp[i + n / 2] = pop(&conflictBuffer);
                }
                for(int i = 0; i < n; i++) {
                    push(&players[LEFT], temp[i]);
                }
            }
            else {
                int n = conflictBuffer.len;
                int temp[n];
                for(int i = 0; i < n / 2; i++) {
                    temp[i + n / 2] = pop(&conflictBuffer);
                    temp[i] = pop(&conflictBuffer);
                }
                for(int i = 0; i < n; i++) {
                    push(&players[RIGHT], temp[i]);
                }
            }
        }
    }
    else if(version == 1) {
        while(hasCards(players[0]) == true && hasCards(players[1]) == true && conflicts < maxConflicts) {
            for(int i = 0; i < activePlayers; i++) {
                push(&conflictBuffer, pop(&players[i]));
            }
            conflicts++;
            int result = winner(conflictBuffer);
            if(result == TIE) {
                for(int i = 0; i < activePlayers; i++) {
                    push(&players[i], pop(&conflictBuffer));
                }
            }
            else if(result == LEFT) {
                for(int i = 0; i < activePlayers; i++) {
                    push(&players[LEFT], pop(&conflictBuffer));
                }
            }
            else {
                int t[2];
                t[0] = pop(&conflictBuffer); t[1] = pop(&conflictBuffer);
                for(int i = 0; i < activePlayers; i++) {
                    push(&players[RIGHT], t[1 - i]);
                }
            }
        }
    }
    else {
        return 0;
    }

    if(conflicts == maxConflicts) {
        printf("%d %d %d\n", 0, players[0].len, players[1].len);
    }
    else if(players[0].len == colors * figures) {
        printf("%d %d\n", 2, conflicts);
    }
    else if(players[0].len == 0) {
        printf("%d\n", 3);
        printBuffer(&players[1]);
    }
    else {
        printf("%d %d %d", 1, players[0].len, players[1].len);
    }
    return 0;
}
