class Cell::Impl {
public:
    Impl() = default;

    virtual ~Impl() = default;

    virtual Value GetValue() const = 0;

    virtual std::string GetText() const = 0;

    virtual std::vector<Position> GetReferencedCells() const;

    virtual void InvalidateCache();
};

class Cell::EmptyImpl : public Impl {
public:
    Value GetValue() const override ;

    std::string GetText() const override;
};

class Cell::TextImpl : public Impl {
public:
    explicit TextImpl(std::string str);

    Value GetValue() const override;

    std::string GetText() const override;

private:
    std::string text_;
};

class Cell::FormulaImpl : public Impl {
public:
    explicit FormulaImpl(std::string str);

    Value GetValue() const override;

    std::string GetText() const override ;

    std::vector<Position> GetReferencedCells() const override;

    void InvalidateCache() override;

private:
    std::unique_ptr<FormulaInterface> formula_;
};

std::unique_ptr<CellInterface> CreateCell();