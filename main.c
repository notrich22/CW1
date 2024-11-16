#include <ctype.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define variant "5.16"
#define author "Aleksandr Vasilev"
#define DEBUG 
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
    lower.content =  (char*)calloc(strlen(sntnc.content),sizeof(char*));
    for(int i = 0; i < strlen(sntnc.content); i++){
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
    text.sentences_count = 0;
    char *start_pointer = s;
    char *end_pointer = s;

    //Memory allocation for Text.Sentences
    while(*start_pointer != '\0'){
        if(*start_pointer == '.'){
            text.sentences_count += 1;
        }
        start_pointer++;
    }
    text.sentences = (struct Sentence*) calloc(text.sentences_count,sizeof(struct Sentence));
    
    //String handling part
    start_pointer = s;
    int i = 0;
    for (int j = 0; j < length; j++) {
        if (s[j] == '.') {
            // Пропуск пробела после точки
            int offset = 0;
            if (s[j + 1] == ' ') {
                offset = 1; // Указываем смещение, чтобы пропустить пробел
            }

            // Создаем предложение
            text.sentences[i++] = create_sentence(start_pointer, j - (start_pointer - s) + 1);

            // Обновляем start_pointer, пропуская точку и пробел (если он есть)
            start_pointer = s + j + 1 + offset;

            // Пропуск начальных пробелов перед следующим предложением
            while (*start_pointer == ' ' && *start_pointer != '\0') {
                start_pointer++;
            }
        }
    }

 // Удаление дубликатов
    for (int i = 0; i < text.sentences_count; i++) {
        if (text.sentences[i].content != NULL) {
            for (int j = i + 1; j < text.sentences_count; j++) {
                if (text.sentences[j].content != NULL) {
                    char* lower_i = lower(text.sentences[i]).content;
                    char* lower_j = lower(text.sentences[j]).content;
                    #ifdef DEBUG
                        printf("lower_i: '%s' (length: %zu)\n", lower_i, strlen(lower_i));
                        printf("lower_j: '%s' (length: %zu)\n", lower_j, strlen(lower_j));
                    #endif
                    if (strcmp(lower_i, lower_j) == 0) {
                        free(text.sentences[j].content); // Освобождаем память
                        text.sentences[j].content = NULL; // Помечаем как удалённую
                    }
                    free(lower_i);
                    free(lower_j);
                }
            }
        }
    }
    
    // Перенос уникальных предложений в новый массив
    int unique_count = 0;
    struct Sentence* unique_sentences = (struct Sentence*) calloc(text.sentences_count, sizeof(struct Sentence));
    for (int i = 0; i < text.sentences_count; i++) {
        if (text.sentences[i].content != NULL) {
            unique_sentences[unique_count++] = text.sentences[i];
        }
    }

    // Освобождаем старый массив и обновляем указатель
    free(text.sentences);
    text.sentences = unique_sentences;
    text.sentences_count = unique_count;

    return text;
}


struct Text get_text() {
    struct Text text;

    int len = 0;
    int capacity = 1;
    //Memory allocation for input string part
    char *s = (char*) malloc(sizeof(char));

    char c = getchar();

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


void print_help(){
    printf("1) Изменить все слова в тексте заканчивающиеся на символ цифры (0-9) так чтобы они заканчивались на название цифры на кириллице\n2) Вывести все предложения в которых встречается последнее слово предыдущего предложения.\n3) Отсортировать предложения по уменьшению количества слов в предложении.\n4) Удалить все предложения в которых два и меньше слова.\n5) Вызов справки\n");
    return;
}   

int main(){
    setlocale(LC_ALL, ""); // Устанавливаем локаль для поддержки UTF-8
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
        case 5:
            print_help();
            break;
        default:
            printf("Error: Данные некорректны\n");
            main();
            break;
    }
    free(s.sentences);
    return 0;
}