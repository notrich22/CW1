#include <stdio.h>
#include <stdlib.h>
#define variant "5.16"
#define author "Aleksandr Vasilev"

typedef struct Sentence{
    char* content;
} Sentence;

struct Sentence create_sentence(char* content, int length){
    struct Sentence sentence;
    sentence.content = content;
    return sentence;
}
struct Text{
    struct Sentence* sentences;

};
struct Text standart_text_handling(struct Text text){

}
struct Text *get_text() {
    struct Text text;

    int len = 0;
    int capacity = 1;
    //Memory allocation part
    char *s = (char*) malloc(sizeof(char));

    char c = getchar();

    while (c != '\n') {
        s[(len)++] = c;
        if (len >= capacity) {
            capacity *= 2;
            s = (char*) realloc(s, capacity * sizeof(char));
        }
        c = getchar();
    }
    s[len] = '\0';
    //String handling part
    char *start_pointer = s;
    char *end_pointer = s;
    while(*end_pointer != '\n' && *end_pointer-1 != '\n'){
        if(*end_pointer == '.'){
            text.sentences += create_sentence(start_pointer, end_pointer - start_pointer);
            start_pointer = end_pointer;
        }
        end_pointer++;
    }
    return text;
}

int main(){
    //Greeting
    printf("Course work for option %s, created by %s.\n", variant, author);
    //Пока вот на таком варианте остановимся
    //printf("\033[31m\033[4mЭто текст, подчеркнутый красным\033[0m\n");
    
    //Selection
    int selection;
    scanf("%d", &selection);
    struct Text *s = get_text();

    switch (selection) {
        case 0:
            //printf("%d\n", );
            break;
        case 1:
            //printf("%d\n", );
            break;
        case 2:
            //printf("%d\n", );
            break;
        case 3:
            //printf("%d\n", );
            break;
        default:
            printf("Error: Данные некорректны\n");
            break;
    }

    return 0;
}