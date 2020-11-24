#include "json.h"

using namespace std;

namespace Json {
	Document::Document(Node root) : root(move(root)) {
	}

	const Node& Document::GetRoot() const {
		return root;
	}

	Node LoadNode(istream& input);

	Node LoadArray(istream& input) {
		vector<Node> result;

		for (char c; input >> c && c != ']'; ) {
			if (c != ',') {
				input.putback(c);
			}
			result.push_back(LoadNode(input));
		}

		return Node(move(result));
	}

	Node LoadNumber(istream& input) {
		stringstream buf;
		while (isdigit(input.peek()) || input.peek() == '.' || input.peek() == '-') {
			char ch = input.get();
			buf << ch;
		}
		double result;
		buf >> result;
		return Node(result);
	}

	Node LoadString(istream& input) {
		string line;
		getline(input, line, '"');
		return Node(move(line));
	}

	Node LoadDict(istream& input) {
		map<string, Node> result;

		for (char c; input >> c && c != '}'; ) {
			if (c == ',') {
				input >> c;
			}

			string key = LoadString(input).AsString();
			input >> c;
			result.emplace(move(key), LoadNode(input));
		}

		return Node(move(result));
	}

	Node LoadBool(istream& input) {
		if (input.peek() == 't') {
			input.ignore(4);
			return Node(true);
		}
		else {
			input.ignore(5);
			return Node(false);
		}
	}

	Node LoadNode(istream& input) {
		char c;
		input >> c;

		if (c == '[') {
			return LoadArray(input);
		}
		else if (c == '{') {
			return LoadDict(input);
		}
		else if (c == '"') {
			return LoadString(input);
		}
		else if (isdigit(c) || c == '.' || c == '-') {
			input.putback(c);
			return LoadNumber(input);
		}
		else if (c == 't' || c == 'f') {
			input.putback(c);
			return LoadBool(input);
		}
		return LoadNode(input);

	}

	Document Load(istream& input) {
		return Document{ LoadNode(input) };
	}

	//UPLOADING FUNCTIONS

	void UploadNode(const Node& node, ostream& out);

	void UploadArray(const Node& node, ostream& out) {
		const vector<Node> nodes = node.AsArray();
		out << "[";
		for (size_t i = 0; i < nodes.size(); ++i) {
			out << endl;
			UploadNode(nodes[i], out);
			if (i < nodes.size() - 1) out << ",";
			else out << endl;
		}
		out << "]";
	}
	void UploadDict(const Node& node, ostream& out) {
		map<string, Node> nodes = node.AsMap();
		out << "{";
		for (auto it = nodes.begin(); it != nodes.end(); ++it) {
			out << endl << '"' << it->first << "\": ";
			UploadNode(it->second, out);
			if (it != prev(nodes.end())) out << ",";
			else out << endl;
		}
		out << "}";
	}
	void UploadString(const Node& node, ostream& out) {
		out << "\"" << node.AsString() << "\"";
	}
	void UploadDouble(const Node& node, ostream& out) {
		stringstream buf;
		buf << fixed << node.AsDouble();
		string result = buf.str();
		while (result.back() == '0') result.resize(result.size() - 1);
		if (result.back() == '.') result.resize(result.size() - 1);
		out << result;
	}

	void UploadNode(const Node& node, ostream& out) {
		if (holds_alternative<vector<Node>>(node)) UploadArray(node, out);
		else if (holds_alternative<map<string, Node>>(node)) UploadDict(node, out);
		else if (holds_alternative<string>(node)) UploadString(node, out);
		else if (holds_alternative<double>(node)) UploadDouble(node, out);
	}

	void UploadDocument(const Document& doc, ostream& out) {
		UploadNode(doc.GetRoot(), out);
	}

}
