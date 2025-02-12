#include "text.h"
#include "ioprocessing.h"
#include <locale.h>

#define VARIANT L"5.16"
#define AUTHOR L"Aleksandr Vasilev"

int main() {
    setlocale(LC_ALL, ""); // Установка локали для поддержки UTF-8
    wprintf(L"Course work for option %ls, created by %ls.\n", VARIANT, AUTHOR);

    int selection;
    wscanf(L"%d", &selection);
    struct Text text;
    switch (selection) {
        case 0:
            text = get_text();
            print_text(text);
            break;
        case 1:
            text = get_text();
            text = modify_words_ending_with_digit(text);
            print_text(text);
            break;
        case 2:
            text = get_text();
            print_matching_sentences(text);
            break;
        case 3:
            text = get_text();
            text = sort_sentences_by_word_count(text);
            print_text(text);
            break;
        case 4:
            text = get_text();
            text = remove_sentences_with_two_or_fewer_words(text);
            print_text(text);
            break;
        case 5:
            print_help();
            break;
        default:
            wprintf(L"Error: некорректный ввод\n");
            break;
    }
    if(text.sentences != NULL){
        free_text(&text);
    }
    return 0;
}
