/* Copyright (c) 2020 vesoft inc. All rights reserved.
 *
 * This source code is licensed under Apache 2.0 License,
 * attached with Common Clause Condition 1.0, found in the LICENSES directory.
 */

#ifndef COMMON_EXPRESSION_FUNCTIONCALLEXPRESSION_H_
#define COMMON_EXPRESSION_FUNCTIONCALLEXPRESSION_H_

#include "common/expression/Expression.h"

namespace nebula {

class ArgumentList final {
public:
    ArgumentList() = default;
    explicit ArgumentList(size_t sz) {
        args_.reserve(sz);
    }

    void addArgument(std::unique_ptr<Expression> arg) {
        CHECK(!!arg);
        args_.emplace_back(std::move(arg));
    }

    const auto& args() const {
        return args_;
    }

    auto& args() {
        return args_;
    }

    size_t numArgs() const {
        return args_.size();
    }

    void setArg(size_t i, std::unique_ptr<Expression> arg) {
        DCHECK_LT(i, numArgs());
        args_[i] = std::move(arg);
    }

    void setArgs(std::vector<std::unique_ptr<Expression>> args) {
        args_ = std::move(args);
    }

    bool operator==(const ArgumentList& rhs) const;

private:
    std::vector<std::unique_ptr<Expression>> args_;
};


class FunctionCallExpression final : public Expression {
    friend class Expression;

public:
    FunctionCallExpression(std::string* name = nullptr,
                           ArgumentList* args = nullptr)
        : Expression(Kind::kFunctionCall) {
        if (args == nullptr) {
            args_ = std::make_unique<ArgumentList>();
        } else {
            args_.reset(args);
        }

        name_.reset(name);
    }

    const Value& eval(ExpressionContext& ctx) override;

    bool operator==(const Expression& rhs) const override;

    std::string toString() const override;

    void accept(ExprVisitor* visitor) override;

    std::unique_ptr<Expression> clone() const override {
        auto arguments = new ArgumentList(args_->numArgs());
        for (auto& arg : args_->args()) {
            arguments->addArgument(arg->clone());
        }
        return std::make_unique<FunctionCallExpression>(new std::string(*name_), arguments);
    }

    const std::string* name() const {
        return name_.get();
    }

    const ArgumentList* args() const {
        return args_.get();
    }

    ArgumentList* args() {
        return args_.get();
    }

    void setArgs(ArgumentList* args) {
        args_.reset(args);
    }

private:
    void writeTo(Encoder& encoder) const override;

    void resetFrom(Decoder& decoder) override;

    std::unique_ptr<std::string>    name_;
    std::unique_ptr<ArgumentList>   args_;
    Value                           result_;
};

}  // namespace nebula
#endif  // EXPRESSION_FUNCTIONCALLEXPRESSION_H_
