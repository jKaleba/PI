#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    int day;
    int month;
    int year;
} Date;

// 1 bsearch2

#define FOOD_MAX  30   // max. number of goods
#define ART_MAX   15   // max. length of name's name + 1
#define RECORD_MAX 40  // max. line length

typedef struct {
    char name[20];
    float price;
    int amount;
    Date valid_date;
} Food;

typedef int (*ComparFp)(const void *, const void *);

// compare dates
int cmp_date(const void *d1, const void *d2) {
    Date da = *(Date*)d1;
    Date db = *(Date*)d2;

    int y = da.year - db.year, m = da.month - db.month, d = da.day - db.day;
    return y != 0 ? y :
        m != 0 ? m :
            d != 0 ? d : 0;
}

// compare foods
int cmp(const void *a, const void *b) {
    Food fa = *(Food*)a;
    Food fb = *(Food*)b;
    int name, year, month, day, price;
    price = fa.price - fb.price > 0 ? 1 : fa.price - fb.price == 0 ? 0 : -1;
    name = strcmp(fa.name, fb.name);

    return name != 0 ? name :
        price != 0 ? price:
        cmp_date(&fa.valid_date, &fb.valid_date);
}

// bsearch returning address where to insert new element
void *bsearch2 (const void *key, const void *base, size_t nmemb, size_t size, ComparFp compar, char *result) {

    Food *tab = (Food*)base;
    if(nmemb == 0) {
        *result = 0;
        return (void*)base;
    }

    for(int i = 0; i < nmemb; i++) {
        if(compar(key, &tab[i]) < 0) {
            *result = 0;
            return (void*)&tab[i];
        }
        else if(compar(key, &tab[i]) == 0) {
            *result = 1;
            return (void*)&tab[i];
        }
    }
    *result = 0;
    return (void*)&tab[nmemb];
}

// print goods of given name
void print_art(Food *p, int n, char *art) {
    for(int i = 0; i < n; i++) {
        if(strcmp(p[i].name, art) == 0) {
            printf("%.2f %d ", p[i].price, p[i].amount);
            if(p[i].valid_date.day < 10) {
                printf("0");
            }
            printf("%d.", p[i].valid_date.day);
            if(p[i].valid_date.month < 10) {
                printf("0");
            }
            printf("%d.%d\n", p[i].valid_date.month, p[i].valid_date.year);
        }
    }
}



// add record to table if absent
Food* add_record(Food *tab, int *np, ComparFp compar, Food *new) {

    char result;
    Food *searched = (Food*)bsearch2(new, tab, *np, sizeof(Food), compar, &result);
    if(*np == 0 || searched == &tab[*np]) {
        (*np)++;
        *searched = *new;
    }
    else if(result == 0) {
        (*np)++;
        int counter = 0;
        while(searched <= &tab[*np - counter - 1]) {
            tab[*np - counter] = tab[*np - counter - 1];
            counter++;
        }
        *searched = *new;
    }
    else {
        searched->amount += new->amount;
    }

    return searched;
}

// read no lines of data
// calls add_record if sorted = 1 and just adds element if sorted = 0
int read_goods(Food *tab, int no, FILE *stream, int sorted) {

    int np = 0;
    for(int i = 0; i < no; i++) {
        Food food;
        Date date;
        fscanf(stream, "%s %f %d %d.%d.%d", &food.name, &food.price, &food.amount, &date.day, &date.month, &date.year);
//        printf("%s %f %d %d.%d.%d", food.name, food.price, food.amount, date.day, date.month, date.year);
        food.valid_date = date;
        if(sorted == 1) {
            add_record(tab, &np, cmp, &food);
        }
        else {
            char result;
            Food* location = (Food*)bsearch2(&food, tab, np, sizeof(Food), cmp, &result);
            if(result == 0) {
                tab[np] = food;
                np++;
            }
            else {
                location->amount += food.amount;
            }
        }
    }

    return np;
}

int cmp_qs(const void *a, const void *b) {
    Food *fa = (Food*)a, *fb = (Food*)b;
    return cmp_date(&fa->valid_date, &fb->valid_date);
}

int cmp_bs(const void *a, const void *b) {
    Date *pd = (Date*)a;
    Food *fb = (Food*)b;
    return cmp_date(pd, &fb->valid_date);
}


void addDays(Date *date, int daysToAdd) {
    struct tm time = {
            .tm_year = date->year - 1900,
            .tm_mon = date->month - 1,
            .tm_mday = date->day,
    };

    time_t timeValue = mktime(&time);
    timeValue += daysToAdd * 24 * 60 * 60;
    struct tm *newTime = localtime(&timeValue);

    date->year = newTime->tm_year + 1900;
    date->month = newTime->tm_mon + 1;
    date->day = newTime->tm_mday;
}


// finds the value of goods due on 'curr_date'
float value(Food *food_tab, size_t n, Date curr_date, int days) {
    float value = 0;
    Date finalDate = curr_date;
    addDays(&finalDate, days);

    for(int i = 0; i < n; i++) {
        if(food_tab[i].valid_date.day == finalDate.day && food_tab[i].valid_date.month == finalDate.month){
            value += food_tab[i].price * (float)food_tab[i].amount;
        }
    }

    return value;
}

/////////////////////////////////////////////////////////////////
// 3 Succession

#define CHILD_MAX 20

enum Sex {F, M};
enum BOOL {no, yes};

struct Bit_data {
    enum Sex sex:1;
    enum BOOL in_line:1;
};

typedef struct {
    char *name;
    struct Bit_data bits;
    Date born;
    char *parent;
} Person;

typedef struct {
    char *par_name;
    int index_first;
    int index_last;
} Parent;

const Date primo_date = { 28, 10, 2011 }; // new succession act

int cmp_primo(Person *person1, Person *person2) {
    if (person1->bits.sex == person2->bits.sex) return 0;
    if (person1->bits.sex == F && cmp_date(&person1->born, &primo_date) > 0) return 0;
    if (person2->bits.sex == F && cmp_date(&person2->born, &primo_date) > 0) return 0;
    return person2->bits.sex - person1->bits.sex;
}

// compare persons
int person_cmp(const void* a, const void* b){
    Person *aa = ((Person*)a);
    Person *bb = ((Person*)b);

    if(aa->parent == NULL)
        return 1;
    if(bb->parent == NULL)
        return -1;

    return strcmp(aa->parent, bb->parent);
}

int parent_cmp(const void* a, const void* b){
    Parent *aa = ((Parent*)a);
    Parent *bb = ((Parent*)b);

    return strcmp(aa->par_name, bb->par_name);
}

#define TRUE 1
#define FALSE 0

int fill_indices_tab(Parent *idx_tab, Person *pers_tab, int size) {
    int parents = 0;

    for(int i = 0; i < size; i++){
        int found = FALSE;

        for(int j = 0; j < parents; j++){
            if(strcmp(pers_tab[i].name, idx_tab[j].par_name) == 0){
                found = TRUE;
                break;
            }
        }

        if(!found){
            idx_tab[parents].par_name = pers_tab[i].name;

            Person key = {
                    .parent = pers_tab[i].name
            };

            Person* address = bsearch(&key, pers_tab, size, sizeof(Person), person_cmp);

            if(address == NULL)
                continue;

            int index = (int)(address - pers_tab);
            int firstIndex = index;

            while(firstIndex > 0 && person_cmp(&pers_tab[firstIndex], &pers_tab[firstIndex - 1]) == 0)
                firstIndex--;

            idx_tab[parents].index_first = firstIndex;
            firstIndex = index;

            while(firstIndex < size - 1 && person_cmp(&pers_tab[firstIndex], &pers_tab[firstIndex + 1]) == 0)
                firstIndex++;

            idx_tab[parents].index_last = firstIndex;

            parents++;
        }
    }

    qsort(idx_tab, parents, sizeof(Parent), parent_cmp);

    return parents;
}

void persons_shifts(Person *person_tab, int size, Parent *idx_tab, int no_parents) {
    int index = 0;

    Person tmp[CHILD_MAX];

    while(index < size){
        Parent key = {.par_name = person_tab[index].name};
        Parent* address = bsearch(&key, idx_tab, no_parents, sizeof(Parent), parent_cmp);

        if(address != NULL) {
            int to_copy = address->index_last - address->index_first + 1;

            memmove(tmp, &person_tab[address->index_first], to_copy * sizeof(Person));
            memmove(&person_tab[index + to_copy + 1], &person_tab[index + 1],
                    ((address->index_first) - index - 1) * sizeof(Person));
            memmove(&person_tab[index + 1], tmp, to_copy * sizeof(Person));

            for(int i = 0; i < no_parents; i++){
                if(idx_tab[i].index_first < address->index_first) {
                    idx_tab[i].index_first += to_copy;
                    idx_tab[i].index_last += to_copy;
                }
            }
        }

        index += 1;
    }
}

int cleaning(Person *person_tab, int n) {
    int left = n;
    for(int i = 0; i < left; i++){
        if(person_tab[i].bits.in_line == no){
            memmove(&person_tab[i], &person_tab[i + 1], (left - i)*sizeof(Person));
            i--;
            left--;
        }
    }

    return left;
}

void print_person(const Person *p) {
    printf("%s\n", p->name);
}


int compare(const void* _a, const void* _b){
    Person *a = ((Person*)_a);
    Person *b = ((Person*)_b);

    if(a->parent == NULL)
        return -1;
    if(b->parent == NULL)
        return 1;

    int parent = strcmp(a->parent, b->parent);

    if(parent != 0) {
        return parent;
    }
    else {
        return cmp_primo(a, b);
    }
}

int create_list(Person *person_tab, int n) {
    qsort(person_tab, n, sizeof(Person), compare);

    Parent parents_tab[33];

    int no_parents = fill_indices_tab(parents_tab, person_tab, n);

    persons_shifts(person_tab, n, parents_tab, no_parents);

    return cleaning(person_tab, n);
}

int main(void) {
    Person person_tab[] = {
            {"Charles III", {M, no}, {14, 11, 1948},"Elizabeth II"},
            {"Anne", {F,yes}, {15, 8, 1950},"Elizabeth II"},
            {"Andrew", {M,yes}, {19, 2, 1960},"Elizabeth II"},
            {"Edward", {M,yes}, {10, 3, 1964} ,"Elizabeth II"},
            {"David", {M,yes}, {3, 11, 1961} ,"Margaret"},
            {"Sarah", {F,yes}, {1, 5, 1964}, "Margaret"},
            {"William", {M,yes}, {21, 6, 1982}, "Charles III"},
            {"Henry", {M,yes}, {15, 9, 1984}, "Charles III"},
            {"Peter", {M,yes}, {15, 11, 1977}, "Anne"},
            {"Zara", {F,yes}, {15, 5, 1981}, "Anne"},
            {"Beatrice", {F,yes}, {8, 8, 1988}, "Andrew"},
            {"Eugenie", {F,yes}, {23, 3, 1990}, "Andrew"},
            {"James", {M,yes}, {17, 12, 2007}, "Edward"},
            {"Louise", {F,yes}, {8, 11, 2003}, "Edward"},
            {"Charles", {M,yes}, {1, 7, 1999}, "David"},
            {"Margarita", {F,yes}, {14, 5, 2002}, "David"},
            {"Samuel", {M,yes}, {28, 7, 1996}, "Sarah"},
            {"Arthur", {M,yes}, {6, 5, 2019}, "Sarah"},
            {"George", {M,yes}, {22, 7, 2013}, "William"},
            {"George VI", {M,no}, {14, 12, 1895}, NULL},
            {"Charlotte", {F,yes}, {2, 5, 2015}, "William"},
            {"Louis", {M,yes}, {23, 4, 2018}, "William"},
            {"Archie", {M,yes}, {6, 5, 2019}, "Henry"},
            {"Lilibet", {F,yes}, {4, 6, 2021}, "Henry"},
            {"Savannah", {F,yes}, {29, 12, 2010}, "Peter"},
            {"Isla", {F,yes}, {29, 3, 2012}, "Peter"},
            {"Mia", {F,yes}, {17, 1, 2014}, "Zara"},
            {"Lena", {F,yes}, {18, 6, 2018}, "Zara"},
            {"Elizabeth II", {F,no}, {21, 4, 1925}, "George VI"},
            {"Margaret", {F,no}, {21, 8, 1930}, "George VI"},
            {"Lucas", {M,yes}, {21, 3, 2021}, "Zara"},
            {"Sienna", {F,yes}, {18, 9, 2021}, "Beatrice"},
            {"August", {M,yes}, {9, 2, 2021}, "Eugenie"}
    };

    int to_do, no;
    int n;
    Food food_tab[FOOD_MAX];
    char buff[ART_MAX];
    fgets(buff, ART_MAX, stdin);
    sscanf(buff, "%d", &to_do);

    switch (to_do) {
        case 1:  // bsearch2
            fgets(buff, ART_MAX, stdin);
            sscanf(buff, "%d", &no);
            n = read_goods(food_tab, no, stdin, 1);
            scanf("%s",buff);
            print_art(food_tab, n, buff);
            break;
        case 2: // qsort
            fgets(buff, ART_MAX, stdin);
            sscanf(buff, "%d", &no);
            n = read_goods(food_tab, no, stdin, 0);
            Date curr_date;
            int days;
            scanf("%d %d %d", &curr_date.day, &curr_date.month, &curr_date.year);
            scanf("%d", &days);
            printf("%.2f\n", value(food_tab, (size_t)n, curr_date, days));
            break;
        case 3: // succession
            scanf("%d",&no);
            int no_persons = sizeof(person_tab) / sizeof(Person);
            create_list(person_tab,no_persons);
            print_person(person_tab + no - 1);
            break;
        default:
            printf("NOTHING TO DO FOR %d\n", to_do);
    }
    return 0;
}
