#include "WordPoetry.h"

#define LINE1_LENGTH	32
#define LINE2_LENGTH	64
#define MAX_TAGS_PER_WORD 8
#define MIN_POS_VAL		10

enum _tag { NIL=0, CC=1,DET=2,EX=3,IN=4,JJ=5,JJR=6,JJS=7,MD=8,NN=9,NNS=10,PDT=11,PRP=12,PRPO=13,PRPS=14,PRPZ=15,RB=16,RBR=17,RBS=18,
RP=19,TO=20,VB=21,VBD=22,VBG=23,VBN=24,VBP=25,VBZ=26,WDT=27,WP=28,WPS=29,WRB=30};

WordPoetry::WordPoetry()
{

}


//--------------------------------------------------------------
void WordPoetry::setup()
{
    get_totals("data/sd-card.bin",&total_unique_words,&total_words);
    get_pos_totals("data/sd-card.bin",word_index);

    for (int i = 0; i < NUM_MAGNETS; i++)
    {
        strcpy(magnets[i].word,"");
        magnets[i].pos = 0;
    }

}


//--------------------------------------------------------------
string WordPoetry::getWord(int index)
{
    switch(index) {
        case 0:
            select_word(0, magnets,word_index);
            break;
        case 1:
            select_word(1, magnets,word_index);
            break;
        case 2:
            select_word(2, magnets,word_index);
            break;
        case 3:
            select_word(3, magnets,word_index);
            break;
        case 4:
            select_word(4, magnets,word_index);
            break;
        default:
            break;
    }

    return string(magnets[index].word);
}


//--------------------------------------------------------------
uint8_t WordPoetry::select_word(uint8_t loc, Magnet magnets[], uint16_t word_index[])
{
    uint8_t isleft,isright,result,index;
    uint8_t i,j;
    Magnet left,right;
    char new_word[MAX_WORD_LENGTH+1], old_word[MAX_WORD_LENGTH+1];
    pos_list taglist[MAX_TAGS_PER_WORD];
    pos_list poslist[MAX_NUM_TAGS],poslist2[MAX_NUM_TAGS];
    pos_list intersect[MAX_NUM_TAGS];
    uint16_t prob = 0,bestsofar = 0,best=0,previousbest=0;
    char fl;

    // get word to the left
    isleft = get_left_magnet(loc, magnets, &left);
    // get word to the right
    isright = get_right_magnet(loc, magnets, &right);

    // begin word selection
    if(isright == TRUE && isleft == FALSE)
    {
        // ***************** Word to the right, nothing to the left *****************
        //ofLogNotice() << "Word to the right and to the left";
        if(right.pos == 0) {
            // POS is unknown
            get_pos_list_from_word("data/sd-card.bin",right.word,taglist);
            for (i = 0; i < MAX_TAGS_PER_WORD; i++) {
                get_all_transitions("data/sd-card.bin",taglist[i].pos,poslist,'r');
                for (j = 0; j < MAX_NUM_TAGS; j++) {
                    prob = poslist[j].val * (taglist[i].val + 1);
                    if (prob > bestsofar) {
                        bestsofar = prob;
                        previousbest = best;
                        best = j;
                    }

                }

            }

        } else {
            // POS is known
            get_all_transitions("data/sd-card.bin",right.pos,poslist,'r');
            best = random_index_based_on_frequency(poslist,MAX_NUM_TAGS);
        }

        if(best == 0) {
            strcpy(new_word,get_random_word("data/sd-card.bin"));
        } else {
            // choose a random word based on the chosen POS
            strcpy(new_word,get_random_word_from_pos("data/sd-card.bin",best,word_index));
        }

    } else if (isright == FALSE && isleft == TRUE) {
        // ***************** Word to the left, nothing to the right *****************
         //ofLogNotice() << "Word to the left, nothing to the right";
        if(left.pos == 0) {
            // POS is unknown
            get_pos_list_from_word("data/sd-card.bin",left.word,taglist);
            for (i = 0; i < MAX_TAGS_PER_WORD; i++) {
                get_all_transitions("data/sd-card.bin",taglist[i].pos,poslist,'l');
                for (j = 0; j < MAX_NUM_TAGS; j++) {
                    prob = poslist[j].val * (taglist[i].val + 1);
                    if (prob > bestsofar) {
                        bestsofar = prob;
                        previousbest = best;
                        best = j;
                    }

                }

            }

        } else {
            // POS is known
            get_all_transitions("data/sd-card.bin",left.pos,poslist,'l');
            best = random_index_based_on_frequency(poslist,MAX_NUM_TAGS);
        }

        if(best == 0) {
            strcpy(new_word,get_random_word("data/sd-card.bin"));
        } else {
            // choose a random word based on the chosen POS
            strcpy(new_word,get_random_word_from_pos("data/sd-card.bin",best,word_index));
        }

    } else if (isright == TRUE && isleft == TRUE) {
        // ***************** Word to the right and to the left *****************
		//ofLogNotice() << "Word to the right and to the left";
        // assigns a POS to neighbours if they don't have one
        if(left.pos == 0) {
            get_pos_list_from_word("data/sd-card.bin",left.word,taglist);
            index = random_index_based_on_frequency(taglist,MAX_TAGS_PER_WORD);
            left.pos = taglist[index].pos;
        }
        if(right.pos == 0) {
            get_pos_list_from_word("data/sd-card.bin",right.word,taglist);
            index = random_index_based_on_frequency(taglist,MAX_TAGS_PER_WORD);
            right.pos = taglist[index].pos;
        }

        get_all_transitions("data/sd-card.bin",left.pos,poslist,'l');
        get_all_transitions("data/sd-card.bin",right.pos,poslist2,'r');
        result = do_intersection(poslist,poslist2,intersect);

        if(result > 0) {
            result = random_index_based_on_frequency(intersect,MAX_NUM_TAGS);
            best = intersect[result].pos;

            // choose a random word based on the chosen POS
            strcpy(new_word,get_random_word_from_pos("data/sd-card.bin",best,word_index));
        } else {
            strcpy(new_word,get_random_word("data/sd-card.bin"));
            best = 0;
        }

    } else {
        // ***************** Nothing to the right, nothing to the left *****************
		//ofLogNotice() << "Nothing to the right, nothing to the left";
        //assume start of sentence - select a random (appropriate) word
        i = (int) ofRandom(1,10);
        switch(i) {
        case 1:
                strcpy(new_word,get_random_word_from_pos("data/sd-card.bin",14,word_index));
                best = PRPS;
                break;
        case 2:
                strcpy(new_word,get_random_word_from_pos("data/sd-card.bin",12,word_index));
                best = PRP;
                break;
        case 3:
                strcpy(new_word,get_random_word_from_pos("data/sd-card.bin",2,word_index));
                best = DET;
                break;
        case 4:
                strcpy(new_word,get_random_word_from_pos("data/sd-card.bin",4,word_index));
                best = IN;
                break;
        default:
                strcpy(new_word,get_random_word_from_pos("data/sd-card.bin",5,word_index));
                best = JJ;
                break;
        }

    }

    // store the old word
    strcpy(old_word,magnets[loc].word);

    // try not to repeat words and handle empty selections
    if (strcmp(old_word,new_word) == 0 || strcmp(new_word,"") == 0) {
        /*
        if(strcmp(new_word,"") == 0)
            printf("Empty word!\n");
        if(strcmp(old_word,new_word) == 0)
            printf("Duplicate word!\n");
        */
        strcpy(new_word,get_random_word("data/sd-card.bin"));
        best = 0;
    }

    // store the pos
    magnets[loc].pos = best;

    // store the new word
    strcpy(magnets[loc].word,new_word);

    // Grammar rules
    if(strcmp(left.word,"a") == 0) {
        fl = new_word[0];
        if ((fl == 'a') || (fl == 'e') || (fl == 'i') || (fl == 'o') || (fl == 'u')) {
            //first letter is a vowel!
            /*** HACK hardware implementation may differ ***/
            set_left_word(loc-1,"an",magnets);
        }
    }
    if(strcmp(left.word,"an") == 0) {
        fl = new_word[0];
        if ((fl != 'a') && (fl != 'e') && (fl != 'i') && (fl != 'o') && (fl != 'u')) {
            //first letter is NOT a vowel!
            /*** HACK hardware implementation may differ ***/
            set_left_word(loc-1,"a",magnets);
        }
    }

    return 0;
}


//--------------------------------------------------------------
uint8_t	WordPoetry::get_left_magnet(uint8_t loc, Magnet magnets[], Magnet *left)
{

    if(loc == 0) {
        return FALSE; // no left neighbour
    } else if (strcmp(magnets[loc-1].word,"") == 0) {
        return FALSE; // no left neighbour
    }

    strcpy(left->word,magnets[loc-1].word);
    left->pos = magnets[loc-1].pos;
    return TRUE;
}


//--------------------------------------------------------------
uint8_t	WordPoetry::get_right_magnet(uint8_t loc, Magnet magnets[], Magnet *right)
{

    if(loc == (NUM_MAGNETS - 1)) {
        return FALSE; // no right neighbour
    } else if (strcmp(magnets[loc+1].word,"") == 0) {
        return FALSE; // no right neighbour
    }

    strcpy(right->word,magnets[loc+1].word);
    right->pos = magnets[loc+1].pos;
    return TRUE;
}


//--------------------------------------------------------------
uint8_t WordPoetry::get_pos_list_from_word(const char *filename, char *word, pos_list tags[] )
{
    FILE *pFile;
    uint16_t p,low,high;
    char buf[LINE2_LENGTH];
    unsigned char tmp[2];
    uint8_t	i=0;

    pFile = fopen(filename, "rb");
    if (pFile == NULL) {
        printf("failed to open %s.\n",filename);
        return FALSE;
    }

    //begin binary search
    low = 0;
    high = total_unique_words - 1;
    p = low+((high-low)/2);			// initial probe position
    while (low <= high) {
        //find word
        fseek ( pFile , ((total_words+1)*LINE1_LENGTH + p*LINE2_LENGTH) , SEEK_SET );
        fread(buf,MAX_WORD_LENGTH,1,pFile);
        if (strncmp(word,buf,MAX_WORD_LENGTH) < 0)
            high = p - 1;
        else if (strncmp(word,buf,MAX_WORD_LENGTH) > 0 )
            low = p + 1;
        else {
                // get the POS values
                for(i=0;i< MAX_TAGS_PER_WORD;i++) {
                    tags[i].pos = 0;
                    tags[i].val = 0;
                    fread(tmp,sizeof(uint8_t),1,pFile);
                    tags[i].pos |=  tmp[0];

                    fread(tmp,sizeof(uint16_t),1,pFile);
                    tags[i].val |= tmp[1];
                    tags[i].val <<= 8;
                    tags[i].val |= tmp[0];
                }
                return 0;
        }

        p = low+((high-low)/2);		// next probe position.
    }
    fclose(pFile);
    return TRUE;
}


//--------------------------------------------------------------
uint8_t WordPoetry::get_all_transitions(const char *filename, uint8_t pos1, pos_list p[], char type)
{
    FILE *pFile;
    unsigned char tmp[2*MAX_NUM_TAGS];
    uint8_t i;

    pFile = fopen(filename, "rb");
    if (pFile == NULL) {
        printf("failed to open %s.\n",filename);
        return 1;
    }

    if(type == 'l') {
        fseek (pFile,((total_words+1)*LINE1_LENGTH + total_unique_words*LINE2_LENGTH + pos1*MAX_NUM_TAGS*sizeof(uint16_t)), SEEK_SET);
    } else if (type == 'r') {
        fseek (pFile,((total_words+1)*LINE1_LENGTH + total_unique_words*LINE2_LENGTH + MAX_NUM_TAGS*MAX_NUM_TAGS*sizeof(uint16_t) + pos1*MAX_NUM_TAGS*sizeof(uint16_t)), SEEK_SET);
    } else {
        printf("wrong type!\n");
        exit(1);
    }

    fread(tmp,sizeof(uint16_t),MAX_NUM_TAGS,pFile);
    for (i = 0; i < MAX_NUM_TAGS; i++)
    {
        p[i].pos = i;
        p[i].val = 0;
        p[i].val |= tmp[i*2+1];
        p[i].val <<= 8;
        p[i].val |= tmp[i*2];
    }

    fclose(pFile);
    return TRUE;
}

//--------------------------------------------------------------
uint8_t WordPoetry::random_index_based_on_frequency(pos_list p[],uint8_t total_indices)
{
    uint8_t i,m;
    int16_t n=0;
    uint16_t total=0;

    for (i = 0; i < total_indices; i++) {
        total += p[i].val;
    }

    if(total == 0) {
        return 0;
    }

    n = rand();
    if(n < 0) n*=-1;

    n = (n % total) + 1;
    m = 0;

    while(n > 0) {
        n = n - p[m].val;
        m++;
    }

    // ###DEBUG CODE
    if(m >= MAX_NUM_TAGS)
        printf("**** m outside legal range!\n");


    return m-1;
}

//--------------------------------------------------------------
char* WordPoetry::get_random_word(const char *filename)
{
    FILE *pFile = fopen(filename, "rb");
    uint16_t range = 0;
    static char buf[LINE2_LENGTH];
    uint16_t randNumber = 0;

    if (pFile == NULL) {
        printf("failed to open %s.\n",filename);
        return NULL;
    }

    range = total_unique_words;
    randNumber = (uint16_t) rand();

    if(range == 0) {
        printf("range2 = 0!....floating point exception\n");
    }
    randNumber = randNumber % range;

    fseek ( pFile , ((total_words+1)*LINE1_LENGTH + randNumber*LINE2_LENGTH) , SEEK_SET );
    fread(buf,LINE2_LENGTH,1,pFile);

    fclose(pFile);
    return buf;
}

//--------------------------------------------------------------
char *WordPoetry::get_random_word_from_pos(const char *filename, uint8_t pos_type, uint16_t word_index[])
{
    FILE *pFile = fopen(filename, "rb");
    uint16_t offset = 0,range = 0;
    static char buf[LINE1_LENGTH];
    uint16_t randNumber = 0;

    if (pFile == NULL) {
        printf("failed to open %s.\n",filename);
        return NULL;
    }

    offset = word_index[pos_type];
    range = word_index[pos_type+1] - word_index[pos_type];

    //random word
    randNumber = (uint16_t) rand();
    if(range == 0) {
        printf("range = 0!....floating point exception\n");
    }
    randNumber = randNumber % range;

    offset += randNumber;
    fseek ( pFile , LINE1_LENGTH*(offset+1) , SEEK_SET );
    fread(buf,LINE1_LENGTH,1,pFile);

    fclose(pFile);
    return buf;
}

//--------------------------------------------------------------
uint8_t WordPoetry::do_intersection(pos_list p1[],pos_list p2[],pos_list intersect[])
{
    uint8_t i=0,j=0,k=0;

        for(i = 0; i < MAX_NUM_TAGS; i++){
            intersect[i].pos = 0;
            intersect[i].val = 0;
            for(j = 0; j < MAX_NUM_TAGS; j++){
                // ### left_freq and right_freq comparison values are hard coded ####
                if((p1[i].pos == p2[j].pos) && (p1[i].val > MIN_POS_VAL) && (p2[j].val > MIN_POS_VAL)    ){
                    intersect[k].pos = p1[i].pos;
                    intersect[k].val = p1[i].val + p2[j].val;
                    k++;
                }
            }
        }
    return k;
}

//--------------------------------------------------------------
void WordPoetry::set_left_word(uint8_t loc, const char *new_word, Magnet magnets[])
{

    strcpy(magnets[loc].word,new_word);
    return;
}

//--------------------------------------------------------------
void WordPoetry::get_totals(const char *filename,uint16_t *total_unique_words, uint16_t *total_words) {
    FILE *pFile = fopen(filename, "rb");
    unsigned char buf[2];

    if (pFile == NULL) {
        ofLogError() << "failed to open " << filename << ".";
        return;
    }

    *total_words = *total_unique_words= 0;
    fseek ( pFile , 0 , SEEK_SET );
    fread(buf,sizeof(uint16_t),1,pFile);
    *total_unique_words |= buf[1];
    *total_unique_words <<= 8;
    *total_unique_words |= buf[0];
    printf("total words = %hu ",*total_unique_words);

    fread(buf,sizeof(uint16_t),1,pFile);
    *total_words |= buf[1];
    *total_words <<= 8;
    *total_words |= buf[0];
    printf("total unique words = %hu\n",*total_words);

    fclose(pFile);
    return;
}


//--------------------------------------------------------------
void WordPoetry::get_pos_totals(const char *filename, uint16_t word_index[])
{
    FILE *pFile;
    uint8_t i;
    unsigned char buf[2];

    // set word_index array to zero as we shall be OR'ing  it.
    memset(word_index,0,MAX_NUM_TAGS*2);

    pFile = fopen(filename, "rb");
    fseek(pFile,((total_words+1)*LINE1_LENGTH + total_unique_words*LINE2_LENGTH + MAX_NUM_TAGS*MAX_NUM_TAGS*sizeof(uint16_t)*2),SEEK_SET);
    for(i=0;i < MAX_NUM_TAGS;i++) {
        fread(buf,sizeof(uint16_t),1,pFile);
        word_index[i] |= buf[1];
        word_index[i] <<= 8;
        word_index[i] |= buf[0];
        //printf("word_index[i] = %i\n",word_index[i]);
    }

    fclose(pFile);
    return;
}
