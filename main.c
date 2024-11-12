#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define variant "5.16"
#define author "Aleksandr Vasilev"

struct Sentence{
    char* content;
    size_t length;
};

struct Sentence create_sentence(char *start, int length) {
    struct Sentence sentence;
    sentence.content = (char *)malloc((length + 1) * sizeof(char));
    strncpy(sentence.content, start, length);
    sentence.content[length] = '\0'; // Добавляем завершающий нуль
    return sentence;
}

struct Sentence lower(struct Sentence sntnc){
    struct Sentence lower;
    for(int i = 0; i < sntnc.length; i++){
        lower.content[i] = tolower(sntnc.content[i]);
    }
    return lower;
}

struct Text{
    struct Sentence* sentences;
    size_t sentences_count;
};

struct Text string_handling(char* s, size_t length){
    struct Text text;
    char *start_pointer = s;
    char *end_pointer = s;

    //Memory allocation for Text.Sentences
    
    while(*start_pointer != '\0'){
        if(*start_pointer == '.'){
            text.sentences_count += 1;
        }
        start_pointer++;
    }
    text.sentences = (struct Sentence*) calloc(text.sentences_count,sizeof(struct Sentence*));
    
    //String handling part
    start_pointer = s;
    int i = 0;
    for (int j = 0; j < length; j++) {
        if (s[j] == '.') {
            text.sentences[i++] = create_sentence(start_pointer, j - (start_pointer - s) + 1);
            start_pointer = s + j + 1;
        }
    }

    // Удаление дубликатов
    for (int i = 0; i < text.sentences_count; i++) {
        for (int j = i + 1; j < text.sentences_count; j++) {
            if (text.sentences[j].content != NULL && text.sentences[i].content != NULL) {
                char *lower_i = lower(text.sentences[i]).content;
                char *lower_j = lower(text.sentences[j]).content;
                if (strcmp(lower_i, lower_j) == 0) {
                    free(text.sentences[j].content); // Освобождаем память
                    text.sentences[j].content = NULL; // Помечаем как удалённую
                }
                free(lower_i);
                free(lower_j);
            }
        }
    }

    // Перенос уникальных предложений в новый массив
    int unique_count = 0;
    for (int i = 0; i < text.sentences_count; i++) {
        if (text.sentences[i].content != NULL) {
            text.sentences[unique_count++] = text.sentences[i];
        }
    }
    return text;
}
struct Text get_text() {
    struct Text text;

    int len = 0;
    int capacity = 1;
    //Memory allocation for input string part
    char *s = (char*) malloc(sizeof(char));

    char c = getchar();
    c = getchar();

    while (c != '\n') {
        s[len++] = c;
        if (len >= capacity) {
            capacity *= 2;
            s = (char *)realloc(s, capacity * sizeof(char));
        }   
        c = getchar();
    }
    s[len] = '\0';

    return string_handling(s, len);
}

void print_text(struct Text text) {
    for (int i = 0; i < text.sentences_count; i++) {
        if (text.sentences[i].content != NULL) {
            printf("%s\n", text.sentences[i].content);
        }
    }
}


int main(){
    //Greeting
    printf("Course work for option %s, created by %s.\n", variant, author);
    //Пока вот на таком варианте остановимся
    //printf("\033[31m\033[4mЭто текст, подчеркнутый красным\033[0m\n");
    
    //Selection
    int selection;
    scanf("%d", &selection);
    struct Text s;
    switch (selection) {
        case 0:
            s = get_text();
            print_text(s);
            break;
        case 1:
            s = get_text();

            break;
        case 2:
            s = get_text();
            break;
        case 3:
            s = get_text();
            break;
        case 4:
            s = get_text();
            break;
        default:
            printf("Error: Данные некорректны\n");
            main();
            break;
    }
    for (int i = 0; i < s.sentences_count; i++) {
        free(s.sentences[i].content);
    }
    free(s.sentences);
    return 0;
}