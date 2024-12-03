#include "text.h"
#include "ioprocessing.h"
#include <locale.h>

#define VARIANT L"5.16"
#define AUTHOR L"Aleksandr Vasilev"

int main() {
    setlocale(LC_ALL, ""); // Установка локаль для поддержки UTF-8
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
