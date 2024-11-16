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
    // Memory allocation for input string
    char *s = (char*) malloc(capacity * sizeof(char));
    if (!s) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        exit(1);
    }

    char c;
    while ((c = getchar()) != '\n' && c != EOF) {
        s[len++] = c;
        if (len >= capacity) {
            capacity *= 2;
            s = (char *)realloc(s, capacity * sizeof(char));
            if (!s) {
                fprintf(stderr, "Ошибка выделения памяти\n");
                exit(1);
            }
        }
    }
    s[len] = '\0';

    return string_handling(s, len);
}


const char* digit_to_cyrillic_name(char digit) {
    switch (digit) {
        case '0': return "ноль";
        case '1': return "один";
        case '2': return "два";
        case '3': return "три";
        case '4': return "четыре";
        case '5': return "пять";
        case '6': return "шесть";
        case '7': return "семь";
        case '8': return "восемь";
        case '9': return "девять";
        default: return "";
    }
}


struct Text modify_words_ending_with_digit(struct Text text) {
    for (int i = 0; i < text.sentences_count; i++) {
        char* sentence = text.sentences[i].content;
        size_t len = strlen(sentence);
        char* new_sentence = (char*)malloc(len * 4); // Резервируем достаточно места для возможных изменений
        if (!new_sentence) continue; // Проверка на успешное выделение памяти

        char* word_start = sentence;
        char* new_sentence_ptr = new_sentence;

        for (size_t j = 0; j <= len; j++) {
            if (isspace(sentence[j]) || sentence[j] == '\0' || ispunct(sentence[j])) {
                // Найдено слово
                size_t word_length = &sentence[j] - word_start;
                if (word_length > 0) {
                    // Проверяем, оканчивается ли слово на цифру
                    if (isdigit(word_start[word_length - 1])) {
                        char digit = word_start[word_length - 1];
                        strncpy(new_sentence_ptr, word_start, word_length - 1); // Копируем часть слова без последней цифры
                        new_sentence_ptr += word_length - 1;
                        const char* digit_name = digit_to_cyrillic_name(digit); // Получаем название цифры
                        strcpy(new_sentence_ptr, digit_name); // Копируем название цифры
                        new_sentence_ptr += strlen(digit_name);
                    } else {
                        strncpy(new_sentence_ptr, word_start, word_length); // Копируем слово целиком
                        new_sentence_ptr += word_length;
                    }
                }
                // Копируем символ-разделитель (если не конец строки)
                if (sentence[j] != '\0') {
                    *new_sentence_ptr++ = sentence[j];
                }
                // Обновляем начало следующего слова
                word_start = &sentence[j + 1];
            }
        }
        *new_sentence_ptr = '\0'; // Завершаем строку
        free(text.sentences[i].content); // Освобождаем старое содержимое
        text.sentences[i].content = new_sentence; // Обновляем содержимое
    }
    return text;
}

// Функция для подсчета количества слов в строке
int count_words(const char* str) {
    int count = 0;
    int in_word = 0;

    while (*str) {
        if (isspace((unsigned char)*str)) {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            count++;
        }
        str++;
    }

    return count;
}

// Функция сравнения для qsort
int compare_sentences(const void* a, const void* b) {
    struct Sentence* sentence_a = (struct Sentence*)a;
    struct Sentence* sentence_b = (struct Sentence*)b;

    int count_a = count_words(sentence_a->content);
    int count_b = count_words(sentence_b->content);

    // Для сортировки по убыванию (обратный порядок)
    return count_b - count_a;
}

// Функция для сортировки предложений в структуре Text
struct Text sort_sentences_by_word_count(struct Text text) {
    qsort(text.sentences, text.sentences_count, sizeof(struct Sentence), compare_sentences);
    return text;
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
    // Цикл для очистки буфера после считывания числа
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    struct Text s;
    switch (selection) {
        case 0:
            s = get_text();
            print_text(s);
            break;
        case 1:
            s = get_text();
            s = modify_words_ending_with_digit(s);
            print_text(s);
            break;
        case 2:
            s = get_text();
            break;
        case 3:
            s = get_text();
            sort_sentences_by_word_count(s);
            print_text(s);
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