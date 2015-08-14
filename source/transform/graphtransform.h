#ifndef GRAPHTRANSFORM_H
#define GRAPHTRANSFORM_H

class Graph;
class TransformedGraph;

class GraphTransform
{
public:
    // In some circumstances it may be a performance win to reimplement this instead of going
    // for the inplace transform version
    virtual void apply(const Graph& source, TransformedGraph& target) const;

    virtual void apply(TransformedGraph&) const {}
};

using IdentityTransform = GraphTransform;

#endif // GRAPHTRANSFORM_H
