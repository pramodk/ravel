#ifndef COLLECTIVERECORD_H
#define COLLECTIVERECORD_H

#include <QMap>
#include "commbundle.h"

class CollectiveEvent;

// Information we get from OTF about collectives
class CollectiveRecord : public CommBundle
{
public:
    CollectiveRecord(unsigned long long int _matching, unsigned int _root,
                     unsigned int _collective, unsigned int _entitygroup);

    unsigned long long int matchingId;
    unsigned int root;
    unsigned int collective;
    unsigned int entitygroup;
    bool mark;

    QList<CollectiveEvent *> * events;

    CommEvent * getDesignee();
    void draw(QPainter * painter, CommDrawInterface * vis);

    int set_basic_strides();
};

#endif // COLLECTIVERECORD_H
