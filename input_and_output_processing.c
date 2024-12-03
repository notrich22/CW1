#include <assert.h>
#include <stdlib.h>
#include <wchar.h>
#include "text.h"
#include "sentence.h"


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


void highlight_word(const wchar_t* word) {
    wprintf(L"\033[31m\033[4m%ls\033[0m", word);
}


void print_text(struct Text text) {
    for (int i = 0; i < text.sentences_count; i++) {
        if (text.sentences[i].content != NULL) {
            wprintf(L"%ls\n", text.sentences[i].content);
        }
    }
}


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

void print_help() {
    wprintf(L"1) Изменить все слова в тексте, заканчивающиеся на цифры, на названия цифр на кириллице.\n");
    wprintf(L"2) Вывести все предложения, в которых встречается последнее слово предыдущего предложения.\n");
    wprintf(L"3) Отсортировать предложения по уменьшению количества слов в предложении.\n");
    wprintf(L"4) Удалить все предложения, в которых два и меньше слов.\n");
    wprintf(L"5) Вызов справки.\n");
}