//Linked list with fast search O(1) and insertion O(1)
//Carries unique elements only

#include <unordered_map>

template <typename DATATYPE>
class Node {
public:
	Node() :
		prev(nullptr),
		next(nullptr) {}
	Node(const DATATYPE& data) :
		prev(nullptr),
		next(nullptr),
		data(data) {}
	Node* prev;
	Node* next;
	DATATYPE data;
};

template <typename DATATYPE>
class linkedList {
public:
	typedef typename Node<DATATYPE> NodeType;

	linkedList() : head(nullptr) {}

	NodeType* find(const DATATYPE& data) {
		auto it = m_map.find(data);
		return (it==m_map.end()) ? nullptr : it->second;
	}

	NodeType* insertAfter(NodeType* node, const DATATYPE& data) {
		if(node == nullptr) return nullptr;

		NodeType* curr = find(data);
		if(curr != nullptr) {
			if(curr == head) {
				head = curr->next;
				head->prev = nullptr;
			} else {
				curr->prev->next = curr->next;
				if(curr->next != nullptr)
					curr->next->prev = curr->prev;
			}
		} else {
			curr = new NodeType(data);
			curr->next = node->next;
			if(node->next != nullptr)
				node->next->prev = curr;
			node->next = curr;
			curr->prev = node;
			m_map.insert(std::make_pair(data, curr));
		}
	}

	NodeType* insertAtHead(const DATATYPE& data) {
		if(head == nullptr) {
			head = new NodeType(data);
			m_map.insert(std::make_pair(data, head));
		} else {
			NodeType* node = find(data); 
			if(node == nullptr) {
				node = new NodeType(data);
				node->next = head;
				head->prev = node;
				head = node;
				m_map.insert(std::make_pair(data, head));
			} else {
				moveToHead(node);
			}
		}
		return head;
	}

	void moveToHead(NodeType* node) {
		if(node == head)
			return;
		if(node->prev != nullptr)
			node->prev->next = node->next;
		if(node->next != nullptr)
			node->next->prev = node->prev;
		node->prev = nullptr;
		node->next = head;
		head->prev = node;
		head = node;
	}

	size_t getIndex(NodeType* node) {
		size_t pos = 0;
		NodeType* curr = head;
		while(curr != nullptr) {
			if(curr == node)
				return pos;
			++pos;
			curr = curr->next;
		}
		return pos;
	}

	~linkedList() {
		NodeType* curr = head;
		while(curr != nullptr) {
			NodeType* tmp= curr;
			curr = curr->next;
			delete tmp;
		}
	}
private:
	NodeType* head;
	std::unordered_map<DATATYPE, NodeType* > m_map; 
};