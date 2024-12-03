#include <time.h>
#include <wctype.h>
#include <locale.h>
#include <wchar.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

#define VARIANT L"5.16"
#define AUTHOR L"Aleksandr Vasilev"

// Определение структур
struct Sentence {
    wchar_t* content;
    size_t length;
};

struct Text {
    struct Sentence* sentences;
    size_t sentences_count;
};

// Функция для создания предложения
struct Sentence create_sentence(wchar_t* start, int length) {
    struct Sentence sentence;
    sentence.content = (wchar_t*)malloc((length + 1) * sizeof(wchar_t));
    assert(sentence.content != NULL);
    wcsncpy(sentence.content, start, length);
    sentence.content[length] = L'\0';
    return sentence;
}

// Функция для преобразования строки в нижний регистр
struct Sentence lower(struct Sentence sntnc) {
    struct Sentence lower;
    size_t len = wcslen(sntnc.content);
    lower.content = (wchar_t*)calloc(len + 1, sizeof(wchar_t));
    assert(lower.content != NULL);
    for (size_t i = 0; i < len; i++) {
        lower.content[i] = towlower(sntnc.content[i]);
    }
    return lower;
}

// Обработка строки и создание структуры Text
struct Text string_handling(wchar_t* s, size_t length) {
    struct Text text;
    text.sentences_count = 0;

    // Подсчет количества предложений
    wchar_t* start_pointer = s;
    while (*start_pointer != L'\0') {
        if (*start_pointer == L'.') {
            text.sentences_count++;
        }
        start_pointer++;
    }

    // Выделение памяти для предложений
    text.sentences = (struct Sentence*)calloc(text.sentences_count, sizeof(struct Sentence));
    assert(text.sentences != NULL);
    start_pointer = s;
    int i = 0;

    // Разделение строки на предложения
    for (size_t j = 0; j < length; j++) {
        if (s[j] == L'.') {
            int offset = 0;
            if (s[j + 1] == L' ') offset = 1;
            text.sentences[i++] = create_sentence(start_pointer, j - (start_pointer - s) + 1);
            start_pointer = s + j + 1 + offset;

            // Пропуск начальных пробелов
            while (*start_pointer == L' ' && *start_pointer != L'\0') {
                start_pointer++;
            }
        }
    }

    // Удаление дубликатов (без учета регистра)
    for (int i = 0; i < text.sentences_count; i++) {
        if (text.sentences[i].content != NULL) {
            for (int j = i + 1; j < text.sentences_count; j++) {
                if (text.sentences[j].content != NULL) {
                    wchar_t* lower_i = lower(text.sentences[i]).content;
                    wchar_t* lower_j = lower(text.sentences[j]).content;
                    if (wcscmp(lower_i, lower_j) == 0) {
                        free(text.sentences[j].content);
                        text.sentences[j].content = NULL;
                    }
                    free(lower_i);
                    free(lower_j);
                }
            }
        }
    }

    // Подсчет количества уникальных предложений
    int unique_count = 0;
    for (int i = 0; i < text.sentences_count; i++) {
        if (text.sentences[i].content != NULL) {
            unique_count++;
        }
    }

    // Перенос уникальных предложений
    struct Sentence* unique_sentences = (struct Sentence*)calloc(unique_count, sizeof(struct Sentence));
    assert(unique_sentences != NULL);
    int index = 0;
    for (int i = 0; i < text.sentences_count; i++) {
        if (text.sentences[i].content != NULL) {
            unique_sentences[index++] = text.sentences[i];
        }
    }
    free(text.sentences);
    text.sentences = unique_sentences;
    text.sentences_count = unique_count;

    return text;
}

// Функция для выделения текста красным цветом
void highlight_word(const wchar_t* word) {
    wprintf(L"\033[31m\033[4m%ls\033[0m", word);
}

// Функция для нахождения последнего слова
wchar_t* find_last_word(struct Sentence sntnc) {
    const wchar_t* str = sntnc.content;
    const wchar_t* end = str + wcslen(str);
    const wchar_t* start = end;
    while (start > str && (iswspace(*(start - 1)) || iswpunct(*(start - 1)))) {
        start--;
    }
    while (start > str && !iswspace(*(start - 1)) && !iswpunct(*(start - 1))) {
        start--;
    }
    size_t length = end - start - 1;
    wchar_t* word = (wchar_t*)malloc((length + 1) * sizeof(wchar_t));
    assert(word != NULL);
    wcsncpy(word, start, length);
    word[length] = L'\0';
    return word;
}

// Функция для получения текста от пользователя
struct Text get_text() {
    struct Text text;
    int len = 0, capacity = 1;
    wchar_t* s = (wchar_t*)malloc(capacity * sizeof(wchar_t));
    assert(s != NULL);
    wchar_t c;
    while ((c = getwchar()) != L'\n' && c != WEOF) {
        s[len++] = c;
        if (len >= capacity) {
            capacity *= 2;
            wchar_t* temp = realloc(s, capacity * sizeof(wchar_t));
            if (!temp) {
                free(s);
                fwprintf(stderr, L"Ошибка выделения памяти\n");
                exit(1);
            }
            s = temp;
        }
    }
    s[len] = L'\0';
    text = string_handling(s, len);
    free(s);
    return text;
}

// Преобразование цифры в слово на кириллице
const wchar_t* digit_to_cyrillic_name(wchar_t digit) {
    switch (digit) {
        case L'0': return L"ноль";
        case L'1': return L"один";
        case L'2': return L"два";
        case L'3': return L"три";
        case L'4': return L"четыре";
        case L'5': return L"пять";
        case L'6': return L"шесть";
        case L'7': return L"семь";
        case L'8': return L"восемь";
        case L'9': return L"девять";
        default: return L"";
    }
}

// Функция для изменения слов, оканчивающихся на цифры
struct Text modify_words_ending_with_digit(struct Text text) {
    for (int i = 0; i < text.sentences_count; i++) {
        wchar_t* sentence = text.sentences[i].content;
        size_t len = wcslen(sentence);
        wchar_t* new_sentence = (wchar_t*)malloc(len * 4 * sizeof(wchar_t));
        assert(new_sentence != NULL);

        wchar_t* word_start = sentence;
        wchar_t* new_sentence_ptr = new_sentence;

        for (size_t j = 0; j <= len; j++) {
            if (iswspace(sentence[j]) || sentence[j] == L'\0' || iswpunct(sentence[j])) {
                size_t word_length = &sentence[j] - word_start;
                if (word_length > 0 && iswdigit(word_start[word_length - 1])) {
                    wchar_t digit = word_start[word_length - 1];
                    wcsncpy(new_sentence_ptr, word_start, word_length - 1);
                    new_sentence_ptr += word_length - 1;
                    const wchar_t* digit_name = digit_to_cyrillic_name(digit);
                    wcscpy(new_sentence_ptr, digit_name);
                    new_sentence_ptr += wcslen(digit_name);
                } else {
                    wcsncpy(new_sentence_ptr, word_start, word_length);
                    new_sentence_ptr += word_length;
                }
                if (sentence[j] != L'\0') {
                    *new_sentence_ptr++ = sentence[j];
                }
                word_start = &sentence[j + 1];
            }
        }
        *new_sentence_ptr = L'\0';
        free(text.sentences[i].content);
        text.sentences[i].content = new_sentence;
    }
    return text;
}

// Подсчет слов в предложении
int count_words(struct Sentence sentence) {
    int count = 0, in_word = 0;
    const wchar_t* str = sentence.content;
    while (*str) {
        if (iswspace(*str)) {
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
    int new_count = 0;
    for (int i = 0; i < text.sentences_count; i++) {
        if (count_words(text.sentences[i]) > 2) {
            new_count++;
        }
    }

    struct Sentence* new_sentences = (struct Sentence*)malloc(new_count * sizeof(struct Sentence));
    assert(new_sentences != NULL);
    int index = 0;
    for (int i = 0; i < text.sentences_count; i++) {
        if (count_words(text.sentences[i]) > 2) {
            new_sentences[index++] = text.sentences[i];
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
    wchar_t* last_word = find_last_word(text.sentences[text.sentences_count - 1]);
    for (int i = 0; i < text.sentences_count; i++) {
        wchar_t* current_last_word = last_word;
        if (i > 0) {
            if(current_last_word != NULL)free(current_last_word);
            current_last_word = find_last_word(text.sentences[i - 1]);
        }
        struct Sentence current_sentence = text.sentences[i];
        const wchar_t* found = wcsstr(current_sentence.content, current_last_word);
        if (found) {
            size_t word_length = wcslen(current_last_word);
            const wchar_t* ptr = current_sentence.content;
            while (ptr < found) putwchar(*ptr++);
            highlight_word(current_last_word);
            ptr += word_length;
            wprintf(L"%ls\n", ptr);
        }
    }
    free(last_word);
}

// Печать текста
void print_text(struct Text text) {
    for (int i = 0; i < text.sentences_count; i++) {
        if (text.sentences[i].content != NULL) {
            wprintf(L"%ls\n", text.sentences[i].content);
        }
    }
}

// Печать справки
void print_help() {
    wprintf(L"1) Изменить все слова в тексте, заканчивающиеся на цифры, на названия цифр на кириллице.\n");
    wprintf(L"2) Вывести все предложения, в которых встречается последнее слово предыдущего предложения.\n");
    wprintf(L"3) Отсортировать предложения по уменьшению количества слов в предложении.\n");
    wprintf(L"4) Удалить все предложения, в которых два и меньше слов.\n");
    wprintf(L"5) Вызов справки.\n");
}

int main() {
    setlocale(LC_ALL, ""); // Устанавливаем локаль для поддержки UTF-8
    wprintf(L"Course work for option %ls, created by %ls.\n", VARIANT, AUTHOR);

    int selection;
    wscanf(L"%d", &selection);
    int c;
    while ((c = getwchar()) != L'\n' && c != WEOF);

    struct Text s;
    switch (selection) {
        case 0://TESTED
            s = get_text();
            print_text(s);
            break;
        case 1://TESTED
            s = get_text();
            s = modify_words_ending_with_digit(s);
            print_text(s);
            break;
        case 2://TESTED
            s = get_text();
            print_matching_sentences(s);
            break;
        case 3://BUGS COULD BE
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
            wprintf(L"Ошибка: некорректный ввод\n");
            break;
    }
    if(s.sentences != NULL){
        for (int i = 0; i < s.sentences_count; i++) {
            free(s.sentences[i].content);
        }
        free(s.sentences);
    }
    return 0;
}
