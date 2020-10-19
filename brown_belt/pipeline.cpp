#include "test_runner.h"
#include <functional>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;


struct Email {
	Email(string fr, string t, string bo) {
		from = move(fr);
		to = move(t);
		body = move(bo);
	}
	Email() {}
	string from;
	string to;
	string body;
};


class Worker {
public:
	virtual ~Worker() = default;
	virtual void Process(unique_ptr<Email> email) = 0;
	virtual void Run() {
		// только первому worker-у в пайплайне нужно это имплементировать
		throw logic_error("Unimplemented");
	}

protected:
	// реализации должны вызывать PassOn, чтобы передать объект дальше
	// по цепочке обработчиков
	void PassOn(unique_ptr<Email> email) const {
		if (next_worker)
			next_worker->Process(move(email));
	}
	unique_ptr<Worker> next_worker;

public:
	void SetNext(unique_ptr<Worker> next) {
		next_worker = move(next);
	}
};


class Reader : public Worker {
public:
	Reader(istream& in) : input(in) {}

	void Process(unique_ptr<Email> email) {
		PassOn(move(email));
	}

	void Run() override {
		for (;;) {
			auto email = make_unique<Email>();
			if (getline(input, email->from) &&
				getline(input, email->to) &&
				getline(input, email->body)) {
				Process(move(email));
			}
			else break;
		}
	}

private:
	istream& input;
};


class Filter : public Worker {
public:
	using Function = function<bool(const Email&)>;

public:
	Filter(Function pred) : func(move(pred)) {}
	void Process(unique_ptr<Email> email) {
		if (func(*email)) PassOn(move(email));
	}
private:
	Function func;
};


class Copier : public Worker {
public:
	Copier(string add) : address(move(add)) {}
	void Process(unique_ptr<Email> email) {
		if (address != email->to) {
			auto copy = make_unique<Email>(email->from, address, email->body);
			PassOn(move(email));
			PassOn(move(copy));
		}
		else PassOn(move(email));
	}
private:
	string address;
};


class Sender : public Worker {
public:
	Sender(ostream& os) : output(os) {}
	void Process(unique_ptr<Email> email) {
		output << email->from << '\n' << email->to << '\n' << email->body << '\n';
		PassOn(move(email));
	}
private:
	ostream& output;
};


// реализуйте класс
class PipelineBuilder {
public:
	// добавляет в качестве первого обработчика Reader
	explicit PipelineBuilder(istream& in) {
		workers.push_back(make_unique<Reader>(in));
	}

	// добавляет новый обработчик Filter
	PipelineBuilder& FilterBy(Filter::Function filter) {
		workers.push_back(make_unique<Filter>(filter));
		return *this;
	}

	// добавляет новый обработчик Copier
	PipelineBuilder& CopyTo(string recipient) {
		workers.push_back(make_unique<Copier>(move(recipient)));
		return *this;
	}

	// добавляет новый обработчик Sender
	PipelineBuilder& Send(ostream& out) {
		workers.push_back(make_unique<Sender>(out));
		return *this;
	}

	// возвращает готовую цепочку обработчиков
	unique_ptr<Worker> Build() {
		for (int i = workers.size() - 1; i > 0; --i) {
			workers[i - 1]->SetNext(move(workers[i]));
		}
		return move(workers.front());
	}
private:
	vector<unique_ptr<Worker>> workers;
};


void TestSanity() {
	string input = (
		"erich@example.com\n"
		"richard@example.com\n"
		"Hello there\n"

		"erich@example.com\n"
		"ralph@example.com\n"
		"Are you sure you pressed the right button?\n"

		"ralph@example.com\n"
		"erich@example.com\n"
		"I do not make mistakes of that kind\n"
		);
	istringstream inStream(input);
	ostringstream outStream;

	PipelineBuilder builder(inStream);
	builder.FilterBy([](const Email& email) {
		return email.from == "erich@example.com";
		});
	builder.CopyTo("richard@example.com");
	builder.Send(outStream);
	auto pipeline = builder.Build();

	pipeline->Run();

	string expectedOutput = (
		"erich@example.com\n"
		"richard@example.com\n"
		"Hello there\n"

		"erich@example.com\n"
		"ralph@example.com\n"
		"Are you sure you pressed the right button?\n"

		"erich@example.com\n"
		"richard@example.com\n"
		"Are you sure you pressed the right button?\n"
		);

	ASSERT_EQUAL(expectedOutput, outStream.str());
}

int main() {
	TestRunner tr;
	RUN_TEST(tr, TestSanity);
	return 0;
}
