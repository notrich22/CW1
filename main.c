#include <ctype.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define VARIANT "5.16"
#define AUTHOR "Aleksandr Vasilev"

// Определение структур
struct Sentence {
    char* content;
    size_t length;
};

struct Text {
    struct Sentence* sentences;
    size_t sentences_count;
};

// Функция для создания предложения
struct Sentence create_sentence(char* start, int length) {
    struct Sentence sentence;
    sentence.content = (char*)malloc((length + 1) * sizeof(char));
    strncpy(sentence.content, start, length);
    sentence.content[length] = '\0';
    return sentence;
}

// Функция для преобразования строки в нижний регистр
struct Sentence lower(struct Sentence sntnc) {
    struct Sentence lower;
    size_t len = strlen(sntnc.content);
    lower.content = (char*)calloc(len + 1, sizeof(char));
    for (size_t i = 0; i < len; i++) {
        lower.content[i] = tolower((unsigned char)sntnc.content[i]);
    }
    return lower;
}

// Обработка строки и создание структуры Text
struct Text string_handling(char* s, size_t length) {
    struct Text text;
    text.sentences_count = 0;

    // Подсчет количества предложений
    char* start_pointer = s;
    while (*start_pointer != '\0') {
        if (*start_pointer == '.') {
            text.sentences_count++;
        }
        start_pointer++;
    }

    // Выделение памяти для предложений
    text.sentences = (struct Sentence*)calloc(text.sentences_count, sizeof(struct Sentence));
    start_pointer = s;
    int i = 0;

    // Разделение строки на предложения
    for (size_t j = 0; j < length; j++) {
        if (s[j] == '.') {
            int offset = 0;
            if (s[j + 1] == ' ') offset = 1;
            text.sentences[i++] = create_sentence(start_pointer, j - (start_pointer - s) + 1);
            start_pointer = s + j + 1 + offset;

            // Пропуск начальных пробелов
            while (*start_pointer == ' ' && *start_pointer != '\0') {
                start_pointer++;
            }
        }
    }

    // Удаление дубликатов (без учета регистра)
    for (int i = 0; i < text.sentences_count; i++) {
        if (text.sentences[i].content != NULL) {
            for (int j = i + 1; j < text.sentences_count; j++) {
                if (text.sentences[j].content != NULL) {
                    char* lower_i = lower(text.sentences[i]).content;
                    char* lower_j = lower(text.sentences[j]).content;
                    if (strcmp(lower_i, lower_j) == 0) {
                        free(text.sentences[j].content);
                        text.sentences[j].content = NULL;
                    }
                    free(lower_i);
                    free(lower_j);
                }
            }
        }
    }

    // Перенос уникальных предложений
    struct Sentence* unique_sentences = (struct Sentence*)calloc(text.sentences_count, sizeof(struct Sentence));
    int unique_count = 0;
    for (int i = 0; i < text.sentences_count; i++) {
        if (text.sentences[i].content != NULL) {
            unique_sentences[unique_count++] = text.sentences[i];
        }
    }
    free(text.sentences);
    text.sentences = unique_sentences;
    text.sentences_count = unique_count;

    return text;
}

// Функция для выделения текста красным цветом
void highlight_word(const char* word) {
    printf("\033[31m\033[4m%s\033[0m", word);
}

// Функция для нахождения последнего слова
char* find_last_word(struct Sentence sntnc) {
    const char* str = sntnc.content;
    const char* end = str + strlen(str);
    const char* start = end;
    while (start > str && !isspace((unsigned char)*(start - 1))) {
        start--;
    }
    size_t length = end - start - 1;
    char* word = (char*)malloc(length + 1);
    if (word) {
        strncpy(word, start, length);
        word[length] = '\0';
    }
    return word;
}

// Функция для получения текста от пользователя
struct Text get_text() {
    struct Text text;
    int len = 0, capacity = 1;
    char* s = (char*)malloc(capacity * sizeof(char));
    if (!s) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        exit(1);
    }
    char c;
    while ((c = getchar()) != '\n' && c != EOF) {
        s[len++] = c;
        if (len >= capacity) {
            capacity *= 2;
            s = (char*)realloc(s, capacity * sizeof(char));
            if (!s) {
                fprintf(stderr, "Ошибка выделения памяти\n");
                exit(1);
            }
        }
    }
    s[len] = '\0';
    text = string_handling(s, len);
    free(s);
    return text;
}

// Преобразование цифры в слово на кириллице
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

// Функция для изменения слов, оканчивающихся на цифры
struct Text modify_words_ending_with_digit(struct Text text) {
    for (int i = 0; i < text.sentences_count; i++) {
        char* sentence = text.sentences[i].content;
        size_t len = strlen(sentence);
        char* new_sentence = (char*)malloc(len * 4);
        if (!new_sentence) continue;

        char* word_start = sentence;
        char* new_sentence_ptr = new_sentence;

        for (size_t j = 0; j <= len; j++) {
            if (isspace(sentence[j]) || sentence[j] == '\0' || ispunct(sentence[j])) {
                size_t word_length = &sentence[j] - word_start;
                if (word_length > 0 && isdigit(word_start[word_length - 1])) {
                    char digit = word_start[word_length - 1];
                    strncpy(new_sentence_ptr, word_start, word_length - 1);
                    new_sentence_ptr += word_length - 1;
                    const char* digit_name = digit_to_cyrillic_name(digit);
                    strcpy(new_sentence_ptr, digit_name);
                    new_sentence_ptr += strlen(digit_name);
                } else {
                    strncpy(new_sentence_ptr, word_start, word_length);
                    new_sentence_ptr += word_length;
                }
                if (sentence[j] != '\0') {
                    *new_sentence_ptr++ = sentence[j];
                }
                word_start = &sentence[j + 1];
            }
        }
        *new_sentence_ptr = '\0';
        free(text.sentences[i].content);
        text.sentences[i].content = new_sentence;
    }
    return text;
}

// Подсчет слов в предложении
int count_words(struct Sentence sentence) {
    int count = 0, in_word = 0;
    const char* str = sentence.content;
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
    int count_a = count_words(*sentence_a);
    int count_b = count_words(*sentence_b);
    return count_b - count_a;
}

// Сортировка предложений по количеству слов
struct Text sort_sentences_by_word_count(struct Text text) {
    qsort(text.sentences, text.sentences_count, sizeof(struct Sentence), compare_sentences);
    return text;
}

// Удаление предложений с двумя и меньше словами
struct Text remove_sentences_with_two_or_fewer_words(struct Text text) {
    struct Sentence* new_sentences = (struct Sentence*)malloc(text.sentences_count * sizeof(struct Sentence));
    if (!new_sentences) {
        fprintf(stderr, "Ошибка выделения памяти\n");
        exit(1);
    }
    int new_count = 0;
    for (int i = 0; i < text.sentences_count; i++) {
        if (count_words(text.sentences[i]) > 2) {
            new_sentences[new_count++] = text.sentences[i];
        } else {
            free(text.sentences[i].content);
        }
    }
    free(text.sentences);
    text.sentences = new_sentences;
    text.sentences_count = new_count;
    return text;
}

// Печать предложений с выделением совпадающих слов
void print_matching_sentences(struct Text text) {
    if (text.sentences_count <= 0) return;
    char* last_word = find_last_word(text.sentences[text.sentences_count - 1]);
    for (int i = 0; i < text.sentences_count; i++) {
        struct Sentence current_sentence = text.sentences[i];
        char* found = strstr(current_sentence.content, last_word);
        if (found) {
            const char* ptr = current_sentence.content;
            while (ptr < found) putchar(*ptr++);
            highlight_word(last_word);
            ptr += strlen(last_word);
            printf("%s\n", ptr);
        }
        free(last_word);
        last_word = find_last_word(current_sentence);
    }
    free(last_word);
}

// Печать текста
void print_text(struct Text text) {
    for (int i = 0; i < text.sentences_count; i++) {
        if (text.sentences[i].content != NULL) {
            printf("%s\n", text.sentences[i].content);
        }
    }
}

// Печать справки
void print_help() {
    printf("1) Изменить все слова в тексте, заканчивающиеся на цифры, на названия цифр на кириллице.\n");
    printf("2) Вывести все предложения, в которых встречается последнее слово предыдущего предложения.\n");
    printf("3) Отсортировать предложения по уменьшению количества слов в предложении.\n");
    printf("4) Удалить все предложения, в которых два и меньше слов.\n");
    printf("5) Вызов справки.\n");
}

int main() {
    setlocale(LC_ALL, ""); // Устанавливаем локаль для поддержки UTF-8
    printf("Course work for option %s, created by %s.\n", VARIANT, AUTHOR);

    int selection;
    scanf("%d", &selection);
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
            print_matching_sentences(s);
            break;
        case 3:
            s = get_text();
            s = sort_sentences_by_word_count(s);
            print_text(s);
            break;
        case 4:
            s = get_text();
            s = remove_sentences_with_two_or_fewer_words(s);
            print_text(s);
            break;
        case 5:
            print_help();
            break;
        default:
            printf("Ошибка: некорректный ввод\n");
            break;
    }
    free(s.sentences);
    return 0;
}
