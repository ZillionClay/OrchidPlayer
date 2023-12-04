//
// Created by 11312 on 2023/12/1.
//

#ifndef ORCHIDPLAYER_IRECURRENT_HPP
#define ORCHIDPLAYER_IRECURRENT_HPP

class IRecurrent
{
public:
    virtual void ExecuteOpen()     =   0;
    virtual void ExecuteStart()    =   0;
    virtual void ExecuteRun()      =   0;
    virtual void ExecuteStop()     =   0;
    virtual void ExecuteClose()    =   0;
};


#endif //ORCHIDPLAYER_IRECURRENT_HPP
