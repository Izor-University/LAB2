template <class T>
class Option {
private:
    T value;
    bool hasValue;

public:
    Option() : hasValue(false) {} // Значение инициализируется по умолчанию
    Option(const T& val) : value(val), hasValue(true) {}

    bool IsSome() const { return hasValue; }
    bool IsNone() const { return !hasValue; }

    const T& GetValue() const {
        if (!hasValue) throw Exception("Trying to unwrap Option::None!");
        return value;
    }
};