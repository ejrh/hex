#ifndef MESSAGE_H
#define MESSAGE_H

#include "hex/messaging/serialiser.h"


class Message;

/* These be defined by implementation. */
extern int get_message_type(const std::string& name);
extern const std::string get_message_type_name(int type);
extern Message *new_message(int type);

class Message {
public:
    Message(): type(0), origin(0), id(0) { }
    Message(int type): type(type), origin(0), id(0) { }
    Message(int type, int origin, int id): type(type), origin(origin), id(id) { }
    virtual ~Message() { }

    int type;
    int origin;
    int id;

protected:
    virtual void read(Deserialiser& deserialiser) { }
    virtual void write(Serialiser& serialiser) const { }

    friend Serialiser& operator<<(Serialiser& serialiser, const Message *msg);
    friend Deserialiser& operator>>(Deserialiser& deserialiser, Message *& msg);
};

inline std::ostream& operator<<(std::ostream& os, const Message *msg) {
    Serialiser s(os);
    s << msg;
    return os;
}

static inline bool equal(boost::shared_ptr<Message> m1, boost::shared_ptr<Message> m2) {
    std::ostringstream s1;
    s1 << m1.get();
    std::ostringstream s2;
    s2 << m2.get();
    return s1.str() == s2.str();
}

template<typename T>
class WrapperMessage: public Message {
public:
    WrapperMessage() { }
    WrapperMessage(int type, const T& data): Message(type), data(data) { }
    virtual ~WrapperMessage() { }

    T data;

protected:
    virtual void read(Deserialiser& deserialiser) {
        deserialiser >> data;
    }

    virtual void write(Serialiser& serialiser) const {
        serialiser << data;
    };
};


template<typename T1, typename T2>
class WrapperMessage2: public Message {
public:
    WrapperMessage2() { }
    WrapperMessage2(int type, const T1& data1, const T2& data2): Message(type), data1(data1), data2(data2) { }
    virtual ~WrapperMessage2() { }

    T1 data1;
    T2 data2;

protected:
    virtual void read(Deserialiser& deserialiser) {
        deserialiser >> data1;
        deserialiser >> data2;
    }

    virtual void write(Serialiser& serialiser) const {
        serialiser << data1;
        serialiser << data2;
    };
};


template<typename T1, typename T2, typename T3>
class WrapperMessage3: public Message {
public:
    WrapperMessage3() { }
    WrapperMessage3(int type, const T1& data1, const T2& data2, const T3& data3): Message(type), data1(data1), data2(data2), data3(data3) { }
    virtual ~WrapperMessage3() { }

    T1 data1;
    T2 data2;
    T3 data3;

protected:
    virtual void read(Deserialiser& deserialiser) {
        deserialiser >> data1;
        deserialiser >> data2;
        deserialiser >> data3;
    }

    virtual void write(Serialiser& serialiser) const {
        serialiser << data1;
        serialiser << data2;
        serialiser << data3;
    };
};


template<typename T1, typename T2, typename T3, typename T4>
class WrapperMessage4: public Message {
public:
    WrapperMessage4() { }
    WrapperMessage4(int type, const T1& data1, const T2& data2, const T3& data3, const T4& data4): Message(type), data1(data1), data2(data2), data3(data3), data4(data4) { }
    virtual ~WrapperMessage4() { }

    T1 data1;
    T2 data2;
    T3 data3;
    T4 data4;

protected:
    virtual void read(Deserialiser& deserialiser) {
        deserialiser >> data1 >> data2 >> data3 >> data4;
    }

    virtual void write(Serialiser& serialiser) const {
        serialiser << data1 << data2 << data3 << data4;
    };
};


extern Serialiser& operator<<(Serialiser& serialiser, const Message *msg);

extern Deserialiser& operator>>(Deserialiser& deserialiser, Message *& msg);

#endif
