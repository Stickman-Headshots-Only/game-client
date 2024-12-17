#pragma once

#include <memory>
#include <vector>
#include <algorithm>
#include <cassert>
#include <stdexcept>

namespace Rei
{
    template <typename NodeT>
    class Graph;

    template <typename T>
    class GraphNode {
        friend Graph<T>;

    public:
        GraphNode(const GraphNode&) = delete;
        GraphNode(GraphNode&&) noexcept = default;

        const std::vector<T*>& getParents() const noexcept { return m_parents; }
        std::size_t getParentCount() const noexcept { return m_parents.size(); }

        const T& getParent(std::size_t index) const noexcept
        {
            assert("Error: The requested parent node is out of bounds." && index < m_parents.size());
            return *m_parents[index];
        }

        T& getParent(std::size_t index) noexcept
        {
            assert("Error: The requested parent node is out of bounds." && index < m_parents.size());
            return *m_parents[index];
        }

        const std::vector<T*>& getChildren() const noexcept { return m_children; }
        std::size_t getChildCount() const noexcept { return m_children.size(); }

        const T& getChild(std::size_t index) const noexcept
        {
            assert("Error: The requested child node is out of bounds." && index < m_children.size());
            return *m_children[index];
        }

        T& getChild(std::size_t index) noexcept
        {
            assert("Error: The requested child node is out of bounds." && index < m_children.size());
            return *m_children[index];
        }

        bool isRoot() const noexcept { return m_parents.empty(); }

        bool isLeaf() const noexcept { return m_children.empty(); }

        bool isIsolated() const noexcept { return isRoot() && isLeaf(); }

        template<typename... OtherNodesTs>
        void addParents(GraphNode& node, OtherNodesTs&&... otherNodes)
        {
            if (&node == this)
                throw std::invalid_argument("Error: A graph node cannot be a parent of itself");

            if (std::find(m_parents.cbegin(), m_parents.cend(), &node) == m_parents.cend())
                m_parents.emplace_back(static_cast<T*>(&node));

            if (std::find(node.m_children.cbegin(), node.m_children.cend(), this) == node.m_children.cend())
                node.m_children.emplace_back(static_cast<T*>(this));

            // Stop the recursive unpacking if no more nodes are to be added as parents
            if constexpr (sizeof...(otherNodes) > 0)
                addParents(std::forward<OtherNodesTs>(otherNodes)...);
        }
        
        template<typename... OtherNodesTs>
        void removeParents(GraphNode& node, OtherNodesTs&&... otherNodes)
        {
            unlinkParent(node);
            node.unlinkChild(*this);

            if constexpr (sizeof...(otherNodes) > 0)
                removeParents(std::forward<OtherNodesTs>(otherNodes)...);
        }

        template<typename... OtherNodesTs>
        void addChildren(GraphNode& node, OtherNodesTs&&... otherNodes)
        {
            if (&node == this)
                throw std::invalid_argument("Error: A graph node cannot be a child of itself");

            if (std::find(m_children.cbegin(), m_children.cend(), &node) == m_children.cend())
                m_children.emplace_back(static_cast<T*>(&node));

            if (std::find(node.m_parents.cbegin(), node.m_parents.cend(), this) == node.m_parents.cend())
                node.m_parents.emplace_back(static_cast<T*>(this));

            // Stop the recursive unpacking if no more nodes are to be added as children
            if constexpr (sizeof...(otherNodes) > 0)
                addChildren(std::forward<OtherNodesTs>(otherNodes)...);
        }
        
        template<typename... OtherNodesTs>
        void removeChildren(GraphNode& node, OtherNodesTs&&... otherNodes)
        {
            unlinkChild(node);
            node.unlinkParent(*this);

            // Stop the recursive unpacking if no more nodes are to be removed as children
            if constexpr (sizeof...(otherNodes) > 0)
                removeChildren(std::forward<OtherNodesTs>(otherNodes)...);
        }

        GraphNode& operator=(const GraphNode&) = delete;
        GraphNode& operator=(GraphNode&&) noexcept = default;

        virtual ~GraphNode() = default;

    protected:
        GraphNode() = default;

        /// Unlinks the given node only as a parent of the current one; the given node's children are left untouched.
        /// \param node Node to be unlinked.
        void unlinkParent(const GraphNode& node)
        {
            if (&node == this)
                throw std::invalid_argument("Error: A graph node cannot be unlinked from itself");

            const auto parentIt = std::find(m_parents.cbegin(), m_parents.cend(), &node);
            if (parentIt != m_parents.cend())
                m_parents.erase(parentIt);
        }
        /// Unlinks the given node only as a child of the current one; the given node's parents are left untouched.
        /// \param node Node to be unlinked.
        void unlinkChild(const GraphNode& node)
        {
            if (&node == this)
                throw std::invalid_argument("Error: A graph node cannot be unlinked from itself");

            const auto childIt = std::find(m_children.cbegin(), m_children.cend(), &node);
            if (childIt != m_children.cend())
                m_children.erase(childIt);
        }

        std::vector<T*> m_parents{};
        std::vector<T*> m_children{};
    }; // class GraphNode


    template <typename NodeT>
    class Graph {
        static_assert(std::is_base_of_v<GraphNode<NodeT>, NodeT>, "Error: The graph node type must be derived from GraphNode.");

        using NodePtr = std::unique_ptr<NodeT>;

    public:
        /// Default constructor.
        Graph() = default;
        /// Creates a Graph while preallocating the given amount of nodes.
        /// \param nodeCount Amount of nodes to reserve.
        explicit Graph(std::size_t nodeCount) { m_nodes.reserve(nodeCount); }
        Graph(const Graph&) = delete;
        Graph(Graph&&) noexcept = default;

        std::size_t getNodeCount() const noexcept { return m_nodes.size(); }

        const NodeT& getNode(std::size_t index) const noexcept
        {
            assert("Error: The requested node is out of bounds." && index < m_nodes.size());
            return *m_nodes[index];
        }

        NodeT& getNode(std::size_t index) noexcept
        {
            assert("Error: The requested node is out of bounds." && index < m_nodes.size());
            return *m_nodes[index];
        }

        /// Adds a node into the graph.
        /// \tparam Args Types of the arguments to be forwarded to the node's constructor.
        /// \param args Arguments to be forwarded to the node's constructor.
        /// \return Reference to the newly added node.
        template <typename... Args>
        NodeT& addNode(Args&&... args)
        {
            m_nodes.emplace_back(std::make_unique<NodeT>(std::forward<Args>(args)...));
            return *m_nodes.back();
        }
        /// Removes a node from the graph, after unlinking it from all its parents & children.
        /// \param node Node to be removed.
        void removeNode(NodeT& node)
        {
            const auto nodeIt = std::find_if(m_nodes.cbegin(), m_nodes.cend(), [&node](const NodePtr& nodePtr) { return (nodePtr.get() == &node); });

            if (nodeIt == m_nodes.cend())
                throw std::invalid_argument("Error: The graph node to be removed does not exist");

            for (NodeT* parent : node.m_parents)
                parent->unlinkChild(node);

            for (NodeT* child : node.m_children)
                child->unlinkParent(node);

            m_nodes.erase(nodeIt);
        }

        Graph& operator=(const Graph&) = delete;
        Graph& operator=(Graph&&) noexcept = default;

    protected:
        std::vector<NodePtr> m_nodes{};
    }; // class Graph

} // namespace Rei