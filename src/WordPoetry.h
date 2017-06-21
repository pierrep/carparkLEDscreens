#ifndef WORDPOETRY_H
#define WORDPOETRY_H

#include <stdint.h>
#include "ofMain.h"

#define MAX_NUM_TAGS	32
#define MAX_WORD_LENGTH 20
#define NUM_MAGNETS		5

class WordPoetry
{
public:

    typedef struct {
        uint16_t val;
        uint8_t pos;
    } pos_list;

    typedef struct {
       char word[MAX_WORD_LENGTH+1]; // what current work the magnet holds.
       uint8_t pos;	// what POS the magnet currently holds
       uint8_t loc;	//location of magnet in sentence
    } Magnet;

    WordPoetry();

    void setup();
    string getWord(int i);
    void get_totals(const char *filename,uint16_t *totaluniquewords, uint16_t *totalwords);
    void get_pos_totals(const char *filename, uint16_t word_index[]);
    uint8_t select_word(uint8_t loc, Magnet magnets[], uint16_t word_index[]);
    uint8_t	get_left_magnet(uint8_t loc, Magnet magnets[], Magnet *left);
    uint8_t	get_right_magnet(uint8_t loc, Magnet magnets[], Magnet *right);
    uint8_t get_pos_list_from_word(const char *filename, char *word, pos_list tags[] );
    uint8_t get_all_transitions(const char *filename, uint8_t pos1, pos_list p[], char type);
    uint8_t random_index_based_on_frequency(pos_list p[],uint8_t total_indices);
    char* get_random_word(const char *filename);
    char* get_random_word_from_pos(const char *filename, uint8_t pos_type, uint16_t word_index[]);
    uint8_t do_intersection(pos_list p1[],pos_list p2[],pos_list intersect[]);
    void set_left_word(uint8_t loc, const char *new_word, Magnet magnets[]);

    uint16_t word_index[MAX_NUM_TAGS];
    uint16_t total_unique_words;
    uint16_t total_words;
    Magnet magnets[NUM_MAGNETS];
};


#endif // WORDPOETRY_H
