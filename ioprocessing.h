#pragma once
#include <stdlib.h>


// Функция для нахождения последнего слова
wchar_t* find_last_word(struct Sentence sntnc);
// Обработка строки и создание структуры Text
struct Text string_handling(wchar_t* s, size_t length);
// Получение текста от пользователя
struct Text get_text();
// Функция для выделения текста красным цветом
void highlight_word(const wchar_t* word);
// Печать текста
void print_text(struct Text text);
// Печать справки
void print_help();
// Печать предложений с выделением совпадающих слов
void print_matching_sentences(struct Text text);