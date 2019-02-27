#include <map>
#include <iostream>

//Range map where the user can set a specific value for a range of Keys
//and search with a specific key
//The map insertion is O(log(N)) + O(M) where M is the maximum number of overlapping Keys
//Each map Key value is different from previous Key and preceding Key values so it is memory efficient 

template <typename Key, typename Value>
class range_map {

private:
	typedef typename std::map<Key, Value>::iterator iterator;

public:
	range_map(const Value& init) {
		my_map.insert(std::make_pair(std::numeric_limits<Key>::min(), init));
	}

	void insert(const Key& start, const Key& end, const Value& val) {

		iterator it = lookup(start);
		Value prevVal = it->second; //copy constructor not default constructor :)
		if(it->first == start) {
			it->second = val;
		} else if(!(it->second == val)) {
			my_map.insert(it, std::make_pair(start, val)); 
			++it;
		}

		for(++it; it != my_map.end() && it->first <= end; ) {
			prevVal = it->second;
			auto tmp = it;
			++it;
			my_map.erase(tmp);
		}

		if(it == my_map.end() || it->first != end+1) {
			my_map.insert(--it, std::make_pair(end+1, prevVal));
		}
	}

	
	void dump() {
		std::cout << "Map elements:\n";
		for(auto& p : my_map) {
			std::cout << p.first << " : " << p.second << std::endl;
		}
		std::cout << std::endl;
	}
	
	const Value& operator[](const Key& k) {
		return lookup(k)->second;
	}

private:
	iterator lookup(const Key& k) {
		return --my_map.upper_bound(k);
	}

private:
	std::map<Key, Value> my_map;
};