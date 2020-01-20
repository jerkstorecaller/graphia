#ifndef CENTREINGLAYOUT_H
#define CENTREINGLAYOUT_H

#include "layout.h"

class CentreingLayout : public Layout
{
    Q_OBJECT
public:
    CentreingLayout(const IGraphComponent& graphComponent,
                    NodeLayoutPositions& positions) :
        Layout(graphComponent, positions)
    {}

    void executeReal(bool) override;
};


#endif // CENTREINGLAYOUT_H
