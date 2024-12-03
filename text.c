#include "text.h"
#include "input_and_output_processing.c"


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


struct Text sort_sentences_by_word_count(struct Text text) {
    qsort(text.sentences, text.sentences_count, sizeof(struct Sentence), compare_sentences);
    return text;
}


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