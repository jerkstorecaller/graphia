#ifndef NODEPOSITIONS_H
#define NODEPOSITIONS_H

#include "shared/graph/grapharray.h"
#include "shared/utils/circularbuffer.h"
#include "maths/boundingsphere.h"

#include <array>
#include <mutex>

#include <QVector3D>

static const int MAX_SMOOTHING = 8;

class MeanPosition : public CircularBuffer<QVector3D, MAX_SMOOTHING>
{ public: MeanPosition() { push_back(QVector3D()); } };

class NodePositions : public NodeArray<MeanPosition>
{
private:
    std::recursive_mutex _mutex;
    float _scale = 1.0f;
    int _smoothing = 1;

public:
    using NodeArray::NodeArray;

    std::recursive_mutex& mutex() { return _mutex; }
    void setScale(float scale) { _scale = scale; }
    float scale() const { return _scale; }
    void setSmoothing(int smoothing) { Q_ASSERT(smoothing <= MAX_SMOOTHING); _smoothing = smoothing; }
    int smoothing() const { return _smoothing; }

    const QVector3D& get(NodeId nodeId) const;
    const QVector3D getScaledAndSmoothed(NodeId nodeId) const;
    void set(NodeId nodeId, const QVector3D& position);
    void setExact(NodeId nodeId, const QVector3D& position);

    void update(const NodePositions& other);

    static QVector3D centreOfMass(const NodePositions& nodePositions,
                                  const std::vector<NodeId>& nodeIds);
    static QVector3D centreOfMassScaledAndSmoothed(const NodePositions& nodePositions,
                                                   const std::vector<NodeId>& nodeIds);

    static std::vector<QVector3D> positionsVector(const NodePositions& nodePositions,
                                                  const std::vector<NodeId>& nodeIds);
    static std::vector<QVector3D> positionsVectorScaled(const NodePositions& nodePositions,
                                                        const std::vector<NodeId>& nodeIds);

    static BoundingSphere boundingSphere(const NodePositions& nodePositions,
                                         const std::vector<NodeId>& nodeIds);

    const QVector3D& at(NodeId nodeId) const { return get(nodeId); }

    // Delete base accessors that could be harmful
    MeanPosition& operator[](NodeId nodeId) = delete;
    const MeanPosition& operator[](NodeId nodeId) const = delete;
    MeanPosition& at(NodeId nodeId) = delete;
};

using ExactNodePositions = NodeArray<QVector3D>;

#endif // NODEPOSITIONS_H
