#include "ini.h"
#include <string_view>
#include <iostream>

using namespace std;

namespace Ini {
	Section& Document::AddSection(string name) {
		return sections[move(name)];
	}

	const Section& Document::GetSection(const string& name) const {
		return sections.at(name);
	}

	size_t Document::SectionCount() const {
		return sections.size();
	}

	Document Load(istream& input) {
		Document doc;
		string cur_tag;
		for (string line; getline(input, line); ) {
			if (line.size() == 0) {
				continue;
			}
			else {
				string_view ln(line);
				if (ln[0] == '[') {
					cur_tag = string(ln.substr(1, ln.size() - 2));
					doc.AddSection(cur_tag);
				}
				else {
					size_t bound = ln.find('=');
					doc.AddSection(cur_tag)[string(ln.substr(0, bound))]
						= string(ln.substr(bound + 1, ln.size()));
				}
			}
		}
		return doc;
	}
}