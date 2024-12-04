#pragma once
#include "sentence.h"
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>


// Класс, описывающий текст
struct Text {
    struct Sentence* sentences;
    size_t sentences_count;
};



// Преобразование цифры в слово на кириллице
const wchar_t* digit_to_cyrillic_name(wchar_t digit);
// Функция для изменения слов, оканчивающихся на цифры
struct Text modify_words_ending_with_digit(struct Text text);
// Сортировка предложений по количеству слов
struct Text sort_sentences_by_word_count(struct Text text);
// Удаление предложений с двумя и меньше словами
struct Text remove_sentences_with_two_or_fewer_words(struct Text text);
// Метод очищения памяти
void free_text(struct Text* text);