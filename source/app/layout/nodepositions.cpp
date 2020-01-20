#include "nodepositions.h"

#include <cmath>
#include <numeric>

const QVector3D& NodePositions::getUnsafe(NodeId nodeId) const
{
    return elementFor(nodeId).newest();
}

void NodePositions::lock() const
{
    _mutex.lock();
    _threadId = std::this_thread::get_id();
}

void NodePositions::unlock() const
{
    _threadId = {};
    _mutex.unlock();
}

bool NodePositions::unlocked() const
{
    return _threadId == std::thread::id{};
}

QVector3D NodePositions::get(NodeId nodeId) const
{
    std::unique_lock<const NodePositions> lock(*this);

    return elementFor(nodeId).mean(_smoothing) * _scale;
}

void NodePositions::update(const NodePositions& other)
{
    std::unique_lock<const NodePositions> lock(*this);

    _array = other._array;
}

template<typename GetFn>
static QVector3D centreOfMassWithFn(const std::vector<NodeId>& nodeIds, GetFn&& getFn)
{
    float reciprocal = 1.0f / nodeIds.size();

    return std::accumulate(nodeIds.begin(), nodeIds.end(), QVector3D(),
    [&](const auto& com, auto nodeId)
    {
        return com + (getFn(nodeId) * reciprocal);
    });
}

QVector3D NodePositions::centreOfMass(const std::vector<NodeId>& nodeIds) const
{
    std::unique_lock<const NodePositions> lock(*this);

    return centreOfMassWithFn(nodeIds, [this](NodeId nodeId) { return get(nodeId); });
}

const QVector3D& NodePositions::at(NodeId nodeId) const
{
    std::unique_lock<const NodePositions> lock(*this);

    return getUnsafe(nodeId);
}

const QVector3D& NodeLayoutPositions::get(NodeId nodeId) const
{
    Q_ASSERT(unlocked());

    return getUnsafe(nodeId);
}

void NodeLayoutPositions::set(NodeId nodeId, const QVector3D& position)
{
    Q_ASSERT(unlocked());
    Q_ASSERT(!std::isnan(position.x()) && !std::isnan(position.y()) && !std::isnan(position.z()));

    elementFor(nodeId).push_back(position);
}

void NodeLayoutPositions::set(const std::vector<NodeId>& nodeIds, const ExactNodePositions& nodePositions)
{
    Q_ASSERT(unlocked());

    for(auto nodeId : nodeIds)
    {
        auto position = nodePositions.at(nodeId);

        Q_ASSERT(!std::isnan(position.x()) && !std::isnan(position.y()) && !std::isnan(position.z()));
        elementFor(nodeId).fill(position);
    }
}

QVector3D NodeLayoutPositions::centreOfMass(const std::vector<NodeId>& nodeIds) const
{
    Q_ASSERT(unlocked());

    return centreOfMassWithFn(nodeIds, [this](NodeId nodeId) { return getUnsafe(nodeId); });
}

BoundingBox3D NodeLayoutPositions::boundingBox(const std::vector<NodeId>& nodeIds) const
{
    Q_ASSERT(unlocked());

    if(nodeIds.empty())
        return {};

    Q_ASSERT(!nodeIds.empty());

    auto firstPosition = getUnsafe(nodeIds.front());
    BoundingBox3D boundingBox(firstPosition, firstPosition);

    for(NodeId nodeId : nodeIds)
        boundingBox.expandToInclude(get(nodeId));

    return boundingBox;
}
