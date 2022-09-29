#pragma once

class Expr {
public:
    virtual ~Expr() = default;

    virtual void Print(std::ostream &out) const = 0;

    virtual void DoPrintFormula(std::ostream &out, ExprPrecedence precedence) const = 0;

    virtual double Evaluate(const std::function<double(Position)> CellByPosition) const = 0;

    // higher is tighter
    virtual ExprPrecedence GetPrecedence() const = 0;

    void PrintFormula(std::ostream &out, ExprPrecedence parent_precedence,
                      bool right_child = false) const;
};

class BinaryOpExpr final : public Expr {
public:
    explicit BinaryOpExpr(Type type, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs);

    void Print(std::ostream &out) const override;

    void DoPrintFormula(std::ostream &out, ExprPrecedence precedence) const override;

    ExprPrecedence GetPrecedence() const override;

    // Реализуйте метод Evaluate() для бинарных операций.
    // При делении на 0 выбрасывайте ошибку вычисления FormulaError
    double Evaluate(const SheetInterface& sheet) const override;
    }

private:
    Type type_;
    std::unique_ptr<Expr> lhs_;
    std::unique_ptr<Expr> rhs_;
};

class UnaryOpExpr final : public Expr {
public:
    explicit UnaryOpExpr(Type type, std::unique_ptr<Expr> operand);

    void Print(std::ostream &out) const override;

    void DoPrintFormula(std::ostream &out, ExprPrecedence precedence) const override;

    ExprPrecedence GetPrecedence() const override;

    // Реализуйте метод Evaluate() для унарных операций.
    double Evaluate() const override;

private:
    Type type_;
    std::unique_ptr<Expr> operand_;
};

class NumberExpr final : public Expr {
public:
    explicit NumberExpr(double value);

    void Print(std::ostream &out) const override;

    void DoPrintFormula(std::ostream &out, ExprPrecedence /* precedence */) const override;

    ExprPrecedence GetPrecedence() const override;

    // Для чисел метод возвращает значение числа.
    double Evaluate() const override;

private:
    double value_;
};

class CellExpr final : public Expr {
public:
    explicit CellExpr(const Position *cell);

    void Print(std::ostream &out) const override;

    void DoPrintFormula(std::ostream &out,
                        ExprPrecedence /* precedence */) const override;

    ExprPrecedence GetPrecedence() const override;

    double Evaluate(const CellLookup &cell_lookup) const override;

private:
    const Position cell_;
};