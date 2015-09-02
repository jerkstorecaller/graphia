#include "componentmanager.h"

#include "../utils/utils.h"
#include "../utils/namethread.h"

#include <queue>
#include <map>

ComponentManager::ComponentManager(Graph& graph, bool ignoreMultiElements) :
    AbstractComponentManager(graph, ignoreMultiElements),
    _nextComponentId(0),
    _nodesComponentId(graph),
    _edgesComponentId(graph)
{
    updateComponents(&graph);
}

ComponentIdSet ComponentManager::assignConnectedElementsComponentId(const Graph* graph,
        NodeId rootId, ComponentId componentId,
        NodeArray<ComponentId>& nodesComponentId,
        EdgeArray<ComponentId>& edgesComponentId)
{
    std::queue<NodeId> nodeIds;
    ComponentIdSet oldComponentIdsAffected;

    nodeIds.push(rootId);

    while(!nodeIds.empty())
    {
        auto nodeId = nodeIds.front();
        nodeIds.pop();
        oldComponentIdsAffected.insert(_nodesComponentId[nodeId]);
        nodesComponentId[nodeId] = componentId;

        for(auto edgeId : graph->nodeById(nodeId).edgeIds())
        {
            if(_ignoreMultiElements && graph->typeOf(edgeId) == MultiEdgeId::Type::Tail)
                continue;

            edgesComponentId[edgeId] = componentId;
            auto oppositeNodeId = graph->edgeById(edgeId).oppositeId(nodeId);

            if(nodesComponentId[oppositeNodeId] != componentId)
            {
                nodeIds.push(oppositeNodeId);
                nodesComponentId[oppositeNodeId] = componentId;
            }
        }
    }

    // We don't count nodes that haven't yet been assigned a component
    oldComponentIdsAffected.erase(ComponentId());

    return oldComponentIdsAffected;
}

void ComponentManager::updateComponents(const Graph* graph)
{
    std::unique_lock<std::recursive_mutex> lock(_updateMutex);

    std::map<ComponentId, ComponentIdSet> splitComponents;
    ComponentIdSet splitComponentIds;
    std::map<ComponentId, ComponentIdSet> mergedComponents;
    ComponentIdSet mergedComponentIds;
    ComponentIdSet componentIds;

    NodeArray<ComponentId> newNodesComponentId(*graph);
    EdgeArray<ComponentId> newEdgesComponentId(*graph);

    // Search for mergers and splitters
    for(auto nodeId : graph->nodeIds())
    {
        if(_ignoreMultiElements && graph->typeOf(nodeId) == MultiNodeId::Type::Tail)
            continue;

        auto oldComponentId = _nodesComponentId[nodeId];

        if(newNodesComponentId[nodeId].isNull() && !oldComponentId.isNull())
        {
            if(Utils::contains(componentIds, oldComponentId))
            {
                // We have already used this ID so this is a component that has split
                auto newComponentId = generateComponentId();
                componentIds.insert(newComponentId);
                assignConnectedElementsComponentId(graph, nodeId, newComponentId,
                                                   newNodesComponentId, newEdgesComponentId);

                queueGraphComponentUpdate(graph, oldComponentId);
                queueGraphComponentUpdate(graph, newComponentId);

                splitComponents[oldComponentId].insert(oldComponentId);
                splitComponents[oldComponentId].insert(newComponentId);
                splitComponentIds.insert(newComponentId);
            }
            else
            {
                componentIds.insert(oldComponentId);
                auto componentIdsAffected = assignConnectedElementsComponentId(graph, nodeId, oldComponentId,
                                                                               newNodesComponentId, newEdgesComponentId);
                queueGraphComponentUpdate(graph, oldComponentId);

                if(componentIdsAffected.size() > 1)
                {
                    // More than one old component IDs were observed so components have merged
                    mergedComponents[oldComponentId].insert(componentIdsAffected.cbegin(), componentIdsAffected.cend());
                    componentIdsAffected.erase(oldComponentId);
                    mergedComponentIds.insert(componentIdsAffected.cbegin(), componentIdsAffected.cend());
                }
            }
        }
    }

    // Search for entirely new components
    for(auto nodeId : graph->nodeIds())
    {
        if(_ignoreMultiElements && graph->typeOf(nodeId) == MultiNodeId::Type::Tail)
            continue;

        if(newNodesComponentId[nodeId].isNull() && _nodesComponentId[nodeId].isNull())
        {
            auto newComponentId = generateComponentId();
            componentIds.insert(newComponentId);
            assignConnectedElementsComponentId(graph, nodeId, newComponentId, newNodesComponentId, newEdgesComponentId);
            queueGraphComponentUpdate(graph, newComponentId);
        }
    }

    // Search for added or removed components
    auto componentIdsToBeAdded = Utils::setDifference(componentIds, _componentIds);
    auto componentIdsToBeRemoved = Utils::setDifference(_componentIds, componentIds);

    // Notify all the merges
    for(auto& mergee : mergedComponents)
    {
        if(_debug) qDebug() << "componentsWillMerge" << mergee.second << "->" << mergee.first;
        emit componentsWillMerge(graph, ComponentMergeSet(std::move(mergee.second), mergee.first));
    }

    // Removed components
    for(auto componentId : componentIdsToBeRemoved)
    {
        if(_debug) qDebug() << "componentWillBeRemoved" << componentId;
        Utils::removeByValue(_componentIds, componentId);
        emit componentWillBeRemoved(graph, componentId, Utils::contains(mergedComponentIds, componentId));

        removeGraphComponent(componentId);
    }

    _nodesComponentId = std::move(newNodesComponentId);
    _edgesComponentId = std::move(newEdgesComponentId);

    for(auto componentId : _updatesRequired)
        updateGraphComponent(graph, componentId);

    _updatesRequired.clear();

    // Notify all the new components
    for(auto componentId : componentIdsToBeAdded)
    {
        if(_debug) qDebug() << "componentAdded" << componentId;
        _componentIds.push_back(componentId);
        emit componentAdded(graph, componentId, Utils::contains(splitComponentIds, componentId));
    }

    // Notify all the splits
    for(auto& splitee : splitComponents)
    {
        if(_debug) qDebug() << "componentSplit" << splitee.first << "->" << splitee.second;
        emit componentSplit(graph, ComponentSplitSet(splitee.first, std::move(splitee.second)));
    }
}

ComponentId ComponentManager::generateComponentId()
{
    ComponentId newComponentId;

    if(!_vacatedComponentIdQueue.empty())
    {
        newComponentId = _vacatedComponentIdQueue.front();
        _vacatedComponentIdQueue.pop();
    }
    else
        newComponentId = _nextComponentId++;

    for(auto* componentArray : _componentArrays)
        componentArray->resize(componentArrayCapacity());

    return newComponentId;
}

void ComponentManager::queueGraphComponentUpdate(const Graph* graph, ComponentId componentId)
{
    _updatesRequired.insert(componentId);

    if(!Utils::contains(_componentsMap, componentId))
    {
        std::shared_ptr<GraphComponent> graphComponent = std::make_shared<GraphComponent>(graph);
        _componentsMap.emplace(componentId, graphComponent);
    }
}

void ComponentManager::updateGraphComponent(const Graph* graph, ComponentId componentId)
{
    std::shared_ptr<GraphComponent> graphComponent = _componentsMap[componentId];

    std::vector<NodeId>& nodeIds = graphComponentNodeIds(*graphComponent);
    std::vector<EdgeId>& edgeIds = graphComponentEdgeIds(*graphComponent);

    nodeIds.clear();
    for(auto nodeId : graph->nodeIds())
    {
        if(_ignoreMultiElements && graph->typeOf(nodeId) == MultiNodeId::Type::Tail)
            continue;

        if(_nodesComponentId[nodeId] == componentId)
            nodeIds.push_back(nodeId);
    }

    edgeIds.clear();
    for(auto edgeId : graph->edgeIds())
    {
        if(_ignoreMultiElements && graph->typeOf(edgeId) == MultiEdgeId::Type::Tail)
            continue;

        if(_edgesComponentId[edgeId] == componentId)
            edgeIds.push_back(edgeId);
    }
}

void ComponentManager::removeGraphComponent(ComponentId componentId)
{
    if(Utils::contains(_componentsMap, componentId))
    {
        _componentsMap.erase(componentId);
        _vacatedComponentIdQueue.push(componentId);
        _updatesRequired.erase(componentId);
    }
}

void ComponentManager::onGraphChanged(const Graph* graph)
{
    graph->debugPauser.pause("Call updateComponents from ComponentManager::onGraphChanged", &_debugPaused);
    updateComponents(graph);
    graph->debugPauser.pause("Signal Graph::onGraphChanged", &_debugPaused);
}

template<typename T> class unique_lock_with_warning
{
public:
    unique_lock_with_warning(T& mutex, bool alreadyBlocked) :
        _lock(mutex, std::try_to_lock)
    {
        if(!_lock.owns_lock())
        {
            qWarning() << "Needed to acquire lock when reading from ComponentManager; "
                          "this implies inappropriate concurrent access which is bad "
                          "because it means the thread" << currentThreadName() <<
                          "is blocked until the update completes";

            // If the DebugPauser is in action, it is already effectively locking things
            // for us, so trying to lock again here causes deadlock
            if(!alreadyBlocked)
                _lock.lock();
        }
    }

    ~unique_lock_with_warning()
    {
        if(_lock.owns_lock())
            _lock.unlock();
    }

private:
    std::unique_lock<T> _lock;
};

const std::vector<ComponentId>& ComponentManager::componentIds() const
{
    unique_lock_with_warning<std::recursive_mutex> lock(_updateMutex, _debugPaused);

    return _componentIds;
}

const GraphComponent* ComponentManager::componentById(ComponentId componentId) const
{
    unique_lock_with_warning<std::recursive_mutex> lock(_updateMutex, _debugPaused);

    if(Utils::contains(_componentsMap, componentId))
        return _componentsMap.at(componentId).get();

    Q_ASSERT(!"ComponentManager::componentById returning nullptr");
    return nullptr;
}

ComponentId ComponentManager::componentIdOfNode(NodeId nodeId) const
{
    if(nodeId.isNull())
        return ComponentId();

    unique_lock_with_warning<std::recursive_mutex> lock(_updateMutex, _debugPaused);

    auto componentId = _nodesComponentId.at(nodeId);
    auto i = std::find(_componentIds.cbegin(), _componentIds.cend(), componentId);
    return i != _componentIds.end() ? *i : ComponentId();
}

ComponentId ComponentManager::componentIdOfEdge(EdgeId edgeId) const
{
    if(edgeId.isNull())
        return ComponentId();

    unique_lock_with_warning<std::recursive_mutex> lock(_updateMutex, _debugPaused);

    auto componentId = _edgesComponentId.at(edgeId);
    auto i = std::find(_componentIds.cbegin(), _componentIds.cend(), componentId);
    return i != _componentIds.end() ? *i : ComponentId();
}

