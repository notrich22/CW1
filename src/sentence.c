#include "sentence.h"


struct Sentence create_sentence(wchar_t* start, int length) {
    struct Sentence sentence;
    sentence.content = (wchar_t*)malloc((length + 1) * sizeof(wchar_t));
    if (!sentence.content) {
        wprintf(L"Ошибка выделения памяти\n");
        exit(1);
    }
    wcsncpy(sentence.content, start, length);
    sentence.content[length] = L'\0';
    return sentence;
}


struct Sentence lower(struct Sentence sntnc) {
    struct Sentence lower;
    size_t len = wcslen(sntnc.content);
    lower.content = (wchar_t*)calloc(len + 1, sizeof(wchar_t));
    if (!lower.content) {
        wprintf(L"Ошибка выделения памяти\n");
        exit(1);
    }
    for (size_t i = 0; i < len; i++) {
        lower.content[i] = towlower(sntnc.content[i]);
    }
    return lower;
}


int compare_sentences(const void* a, const void* b) {
    struct Sentence* sentence_a = (struct Sentence*)a;
    struct Sentence* sentence_b = (struct Sentence*)b;
    int count_a = count_words(*sentence_a);
    int count_b = count_words(*sentence_b);
    return count_b - count_a;
}


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
