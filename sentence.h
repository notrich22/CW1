#pragma once
#include <stdlib.h>
#include <wchar.h>
#include <wctype.h>

// Класс описывающий предложение
struct Sentence {
    wchar_t* content;
    size_t length;
};


// Функция для создания предложения
struct Sentence create_sentence(wchar_t* start, int length);
// Функция для преобразования предложения в нижний регистр
struct Sentence lower(struct Sentence sntnc);
// Функция сравнения для qsort
int compare_sentences(const void* a, const void* b);
// Подсчет слов в предложении
int count_words(struct Sentence sentence);
// Метод очищения памяти
void free_sentence(struct Sentence* sentence);