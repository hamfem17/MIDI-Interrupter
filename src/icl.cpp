#include "icl.h"

class ICL
{
    private:
        static uint8 *music;
        static uint32 position;
        

    public:
        // static ICL *getInstance()
        // {
        //     if (!instance)
        //     {
        //         instance = new ICL();                //singleton 
        //     }

        //     return instance;
        // }
    ICL(uint8 *music)
    {
        this->music = music;
        this->pos = 0;
    }

    int validateHeader()
    {
        if(music[pos++] != 'I' || music[pos++] != 'C' || music[pos++] != 'L') //Von ICL
        {
            return 1;
        }

        if(music[pos++] != 1) //Version
        {
            return 2;
        }
    }


    Action getNextAction()
    {
        struct Action action;


    }

};