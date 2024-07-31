#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdint>

namespace mll {

	using std::string;
	using std::cout;
	using std::vector;
	using std::ifstream;
	using std::stoi;
	using std::stof;

	void SplitString(const string& str, const string& split, vector<string>& outSubstrings) {
		int32_t start = 0, end = str.find_first_of(split);
		while (end != -1) {
			outSubstrings.push_back(str.substr(start, end - start));
			start = end + 1;
			end = str.find_first_of(split, start);
		}
		outSubstrings.push_back(str.substr(start, str.length() - start));
	}

	inline bool IsMetaTag(const string& tagString) {
		return tagString[1] == '?';
	}

	inline bool IsSingleTag(const string& tagString) {
		return tagString[tagString.length() - 2] == '/';
	}

	inline bool IsClosingTag(const string& tagString) {
		return tagString[1] == '/';
	}

	struct XMLTagAttribute {
		string name;
		string value;
		bool hasValue;

		XMLTagAttribute(const string& tagAttrString) {
			int32_t index = tagAttrString.find_first_of("=");
			if (index == -1) {
				hasValue = false;
				name = tagAttrString;
			}
			else {
				hasValue = true;
				name = tagAttrString.substr(0, index);
				value = tagAttrString.substr(index + 2, tagAttrString.length() - index - 3);
			}
		}

		XMLTagAttribute(const string& _name, const string& _value, bool _hasValue = true)
			: name(_name), value(_value), hasValue(_hasValue) {}
	};

	struct Filter {
		string name;
		string id;
		vector<XMLTagAttribute> attributes;
	};

	struct XMLTag {
		string name;
		bool isSingle;
		bool isMeta;
		vector<XMLTagAttribute> attributes;
		XMLTag* parent;
		vector<XMLTag*> children;
		string data;

		XMLTag(const string& tagString)
			: parent(nullptr)
		{
			isSingle = IsSingleTag(tagString);
			isMeta = IsMetaTag(tagString);

			string tagBody = tagString.substr(isMeta ? 2 : 1, tagString.length() - (isMeta ? 4 : (isSingle ? 3 : 2)));
			vector<string> tokens;

			SplitString(tagBody, " ", tokens);

			name = tokens[0];
			for (int32_t i = 1; i < tokens.size(); i++) {
				attributes.push_back(XMLTagAttribute(tokens[i]));
			}
		}

		~XMLTag() {
			for (int32_t i = 0; i < children.size(); i++) {
				delete children[i];
			}
		}

		void Print() {
			cout << (isMeta ? "<?" : "<") << name;
			for (int32_t i = 0; i < attributes.size(); i++) {
				cout << " " << attributes[i].name + (attributes[i].hasValue ? ("=\"" + attributes[i].value + "\"") : "");
			}
			cout << (isMeta ? "?>" : (isSingle ? "/>" : ">")) << "\n";
		}

		void AddChild(XMLTag* tag) {
			tag->parent = this;
			children.push_back(tag);
		}

		bool MatchFilter(const string& filter) {
			int32_t index = filter.find("#");
			if (index != -1) {
				string n = filter.substr(0, index);
				string id = filter.substr(index + 1, filter.length() - 1);
				return ((n == "") || (n == name)) && (id == GetAttribute("id"));
			}
			return filter == name;
		}

		XMLTag* FindFirst(const string& query) {
			vector<string> filters;
			SplitString(query, " ", filters);
			return FindFirst(filters, 0);
		}

		XMLTag* FindFirst(const vector<string>& filters, int32_t index) {
			if (MatchFilter(filters[index])) {
				index++;
				if (index == filters.size())
					return this;
			}
			for (int32_t i = 0; i < children.size(); i++) {
				XMLTag* result = children[i]->FindFirst(filters, index);
				if (result != nullptr) return result;
			}
			return nullptr;
		}
		
		vector<XMLTag*> Find(const string& query) {
			vector<XMLTag*> found;
			vector<string> filters;
			SplitString(query, " ", filters);
			Find(filters, 0, found);
			return found;
		}

		void Find(const vector<string>& filters, int32_t index, vector<XMLTag*>& outFound) {
			if (MatchFilter(filters[index])) {
				index++;
				if (index == filters.size()) {
					outFound.push_back(this);
					index--;
				}
			}
			for (int32_t i = 0; i < children.size(); i++) {
				children[i]->Find(filters, index, outFound);
			}
		}
		
		bool HasAttribute(const string& name) {
			for (int32_t i = 0; i < attributes.size(); i++) {
				if (attributes[i].name == name) return true;
			}
			return false;
		}

		string GetAttribute(const string& name) {
			for (int32_t i = 0; i < attributes.size(); i++) {
				if (attributes[i].name == name) return attributes[i].value;
			}
			return "";
		}

	};

	XMLTag* LoadXMLTree(const string& filePath) {
		ifstream t(filePath);
		string file((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
		cout << "Loading file: \"" << filePath << "\" File size: " << file.length() << "\n";
		t.close();

		XMLTag* root = new XMLTag("<root>");
		XMLTag* activeTag = root;
		XMLTag* newTag = nullptr;

		int32_t start = 0, end = 0;
		int32_t lastTagEnd = 0, newTagStart = 0;
		string tagString;
		while (true) {
			start = file.find_first_of("<", start);
			end = file.find_first_of(">", start);
			if ((start == -1) || (end == -1)) {
				cout << "Parsing Error! XML tree is incomplete.\nLast tag name: " << activeTag->name << "\nLast processed tag: '" << tagString << "'\n";
				break;
			}

			tagString = file.substr(start, end - start + 1);
			newTagStart = start;
			start = end;

			if (IsClosingTag(tagString)) {
				if (activeTag->children.size() == 0)
					activeTag->data = file.substr(lastTagEnd + 1, newTagStart - lastTagEnd - 1);
				activeTag = activeTag->parent;
				if (activeTag == nullptr || activeTag == root)
					break;
				continue;
			}

			newTag = new XMLTag(tagString);
			activeTag->AddChild(newTag);

			if (!newTag->isMeta && !newTag->isSingle)
				activeTag = newTag;

			lastTagEnd = end;
		}

		return root;
	}



	struct Mesh {
		
	};

	struct Geometry {
		string id;
		string name;
		Mesh mesh;
	};

	struct SceneData {
		vector<Geometry> geometries;

	};



	SceneData LoadColladaFile(const string& filePath) {
		SceneData scene;
		XMLTag* root = LoadXMLTree(filePath);


		auto geometries = root->Find("geometry");
		for (int32_t i = 0; i < geometries.size(); i++) {
			XMLTag* verticesSource = geometries[i]->FindFirst(geometries[i]->FindFirst("vertices input")->GetAttribute("source"));

			XMLTag* accessor = verticesSource->FindFirst("accessor");
			int32_t accessorCount = stoi(accessor->GetAttribute("count"));
			int32_t accessorStride = stoi(accessor->GetAttribute("stride"));
			string accessorSourceID = accessor->GetAttribute("source");

			XMLTag* verticesSourceArray = geometries[i]->FindFirst(accessorSourceID);
			int32_t verticesSourceArrayCount = stoi(verticesSourceArray->GetAttribute("count"));
			
			vector<string> stringValues;
			SplitString(verticesSourceArray->data, " ", stringValues);
			vector<float> floatValues;
			for (int32_t i = 0; i < stringValues.size(); i++) {
				floatValues.push_back(std::stof(stringValues[i]));
			}


		}


		delete root;
		return scene;
	}

}