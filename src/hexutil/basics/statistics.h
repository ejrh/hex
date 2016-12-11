#ifndef STATISTICS_H
#define STATISTICS_H

class Counter;

void register_counter(Counter *counter, const std::string& name);
void register_counter(Counter *counter, const Counter *existing);
void unregister_counter(Counter *counter);
void log_statistics(const std::string& label);

class Counter {
public:
    typedef unsigned long int counter_type;

    Counter() { }
    Counter(const std::string& name): value(0) {
        register_counter(this, name);
    }
    Counter(const std::string& name, counter_type value): value(value) {
        register_counter(this, name);
    }
    Counter(const Counter& other) {
        register_counter(this, &other);
        value = 0;
    }

    ~Counter() {
        unregister_counter(this);
    }

    Counter& operator=(const Counter& other) {
        register_counter(this, &other);
        value = 0;
        return *this;
    }
    Counter& operator++() { value++; return *this; }
    Counter& operator+=(int incr) { value += incr; return *this; }
    operator counter_type() { return value; }

    counter_type get_value() { return value; }
    counter_type reset() { counter_type old_value = value; value = 0; return old_value; }

private:
    counter_type value;
};

#endif
