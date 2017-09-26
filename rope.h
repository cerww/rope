#pragma once
#include <memory>
#include <string>
#include <string_view>
using namespace std::string_literals;

//template<typename T>
class rope {
public:
private:
	struct ropeNode {
		ropeNode(std::string t_str) {
			if (t_str.size() < 2048){
				str = std::make_unique<std::string>(std::move(t_str));
			}else {
				left = std::make_unique<ropeNode>(t_str.substr(0, t_str.size() / 2));
				//left = std::make_unique<ropeNode>(std::string_view(&t_str[0], t_str.size() / 2));
				right = std::make_unique<ropeNode>(t_str.substr(t_str.size() / 2));
				//right = std::make_unique<ropeNode>(std::string_view(&t_str[t_str.size() / 2], t_str.size() - (t_str.size() / 2)));
				weight = t_str.size() / 2;
			}
		}
		ropeNode(std::string_view t_str) {
			if (t_str.size() < 2048) {
				str = std::make_unique<std::string>(t_str);
			}
			else {
				left = std::make_unique<ropeNode>(t_str.substr(0, t_str.size() / 2));
				right = std::make_unique<ropeNode>(t_str.substr(t_str.size() / 2));
				weight = t_str.size() / 2;
			}
		}
		ropeNode(std::unique_ptr<ropeNode> t_left,size_t t_weight):
			left(std::move(t_left)),
			weight(t_weight){}
		~ropeNode() {
			if (isLeaf()) str.~unique_ptr<std::string>();
		};
		std::unique_ptr<ropeNode> left = nullptr;
		std::unique_ptr<ropeNode> right = nullptr;
		union{
			size_t weight = 0;
			std::unique_ptr<std::string> str;
		};
		bool isLeaf() const {
			return (left == nullptr) && (right == nullptr);
		}
		size_t size()const {//size is O(nlog(n))
			if (isLeaf()) 
				return str->size();
			return weight + (right ? right->size(): 0);//weight == left->size()
		}
		std::string to_string()const {
			if (isLeaf())
				return *str;
			return (left ? left->to_string() : ""s) + (right ? right->to_string() : ""s);
		}
		char& operator[](size_t index) {
			if (isLeaf())return (*str)[index];
			if (index > weight)
				return (*right)[index - weight];
			return (*left)[index];
		}		
		void insert(size_t index,std::string_view strToAdd) {
			if (isLeaf()) {
				str->insert(index, strToAdd);
				if (str->size() > 4096) {
					const size_t halfStrSize= str->size() / 2;
					//left = std::make_unique<ropeNode>(str->substr(0, halfStrSize));
					left = std::make_unique<ropeNode>(std::string_view(&str->at(0),halfStrSize));
					//left = std::make_unique<ropeNode>(std::string(str->cbegin(), str->cbegin() + halfStrSize));
					//right = std::make_unique<ropeNode>(str->substr(halfStrSize));
					right = std::make_unique<ropeNode>(std::string_view(&str->at(halfStrSize), str->size() - halfStrSize));//7/2 = 3;7-3 = 4
					//right = std::make_unique<ropeNode>(std::string(str->cbegin()+halfStrSize,str->cend()));
					str.~unique_ptr<std::string>();
					weight = halfStrSize;
				}
			}
			else if (index < weight) {
				weight += strToAdd.size();
				left->insert(index, strToAdd);
			}else {
				right->insert(index - weight, strToAdd);
			}
		}
		void erase(size_t start,size_t end) {
			if (isLeaf()) {
				str->erase(start, end-start);
			}else if (start < weight) {
				if (end < weight) {
					left->erase(start, end);
					weight -= (end - start);
				}else {
					left->erase(start, weight);
					right->erase(0, end - weight);
					weight = start;
				}
			}else {
				right->erase(start - weight, end - weight);
			}
		}
	};
	std::unique_ptr<ropeNode> m_root = nullptr;
public:
	rope(std::string t_str) {
		const size_t sizeOfStr = t_str.size();
		std::string_view t_strView(&t_str[0], t_str.size());
		m_root = std::make_unique<ropeNode>(t_strView);
		m_root = std::make_unique<ropeNode>(std::move(m_root), sizeOfStr);
	}
	char& operator[](size_t index) {
		return (*m_root)[index];
	}
	void erase(size_t start,size_t end){
		m_root->erase(start, end);
	}
	void insert(size_t index,std::string strToAdd) {
		std::string_view strToAddView(&strToAdd[0],strToAdd.size());
		m_root->insert(index, strToAddView);
	}
	size_t size()const {
		return m_root->weight;
	}
	operator std::string()const {
		return m_root->to_string();
	}
};