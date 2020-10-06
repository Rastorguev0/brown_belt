#include "Common.h"

class ValueNode : public Expression {
public:
  ValueNode(int val) : value(val) {}
  int Evaluate() const override {
		return value;
	}
  std::string ToString() const override {
		return std::to_string(value);
	}
private:
  int value;
};

class SumNode : public Expression {
public:
  SumNode(ExpressionPtr lhs, ExpressionPtr rhs) : left(move(lhs)), right(move(rhs)) {}
  int Evaluate() const override {
		return left->Evaluate() + right->Evaluate();
	}
  std::string ToString() const override {
		return "(" + left->ToString() + ")" + "+" + "(" + right->ToString() + ")";
	}
private:
  ExpressionPtr left, right;
};

class ProdNode : public Expression {
public:
  ProdNode(ExpressionPtr lhs, ExpressionPtr rhs) : left(move(lhs)), right(move(rhs)) {}
  int Evaluate() const override {
		return left->Evaluate() * right->Evaluate();
	}
  std::string ToString() const override {
		return "(" + left->ToString() + ")" + "*" + "(" + right->ToString() + ")";
	}
private:
  ExpressionPtr left, right;
};


ExpressionPtr Value(int value) {
	return std::make_unique<ValueNode>(value);
}
ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right) {
	return std::make_unique<SumNode>(move(left), move(right));
}
ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right) {
	return std::make_unique<ProdNode>(move(left), move(right));
}
