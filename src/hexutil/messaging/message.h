#ifndef MESSAGE_H
#define MESSAGE_H

#include "hexutil/messaging/serialiser.h"

enum {
    UndefinedMessageType = 0
};

class Message: public boost::enable_shared_from_this<Message> {
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

static inline bool equal(Message *m1, Message *m2) {
    std::ostringstream s1;
    s1 << m1;
    std::ostringstream s2;
    s2 << m2;
    return s1.str() == s2.str();
}

static inline bool equal(const boost::shared_ptr<Message>& m1, const boost::shared_ptr<Message>& m2) {
    return equal(m1.get(), m2.get());
}

typedef std::vector<boost::shared_ptr<Message> > MessageSequence;

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
class WrapperMessage2_3: public Message {
public:
    WrapperMessage2_3() { }
    WrapperMessage2_3(int type, const T1& data1, const T2& data2, const T3& data3): Message(type), data1(data1), data2(data2), data3(data3) { }
    virtual ~WrapperMessage2_3() { }

    T1 data1;
    T2 data2;
    T3 data3;

protected:
    virtual void read(Deserialiser& deserialiser) {
        deserialiser >> data1;
        deserialiser >> data2;
        if (deserialiser.peek() != ')')
            deserialiser >> data3;
        else
            data3 = default_value<T3>();
    }

    virtual void write(Serialiser& serialiser) const {
        serialiser << data1;
        serialiser << data2;
        serialiser << data3;
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

extern Serialiser& operator<<(Serialiser& serialiser, const MessageSequence& sequence);
extern Deserialiser& operator>>(Deserialiser& deserialiser, MessageSequence& sequence);

class MessageFactory {
public:
    virtual ~MessageFactory() { }

    virtual int min_type() = 0;
    virtual int max_type() = 0;

    virtual int get_message_type(const std::string& name) = 0;
    virtual const std::string& get_message_type_name(int type) = 0;
    virtual Message *new_message(int type) = 0;

public:
    static const std::string empty_string;
};


class AbstractMessageFactory: public MessageFactory {
public:
    AbstractMessageFactory(int min_message_type, int max_message_type):
            min_message_type(min_message_type), max_message_type(max_message_type) {
    }

    int min_type() {
        return min_message_type;
    }

    int max_type() {
        return max_message_type;
    }

    int get_message_type(const std::string& name) {
        for (int i = min_message_type; i <= max_message_type; i++) {
            if (name == msg_type_names[i - min_message_type])
                return i;
        }
        return UndefinedMessageType;
    }

    const std::string& get_message_type_name(int type) {
        if (type >= min_message_type && type <= max_message_type) {
            return msg_type_names[type - min_message_type];
        } else {
            return empty_string;
        }
    }

    virtual Message *new_message(int type) = 0;

protected:
    int min_message_type, max_message_type;
    std::vector<std::string> msg_type_names;
};


class MessageTypeRegistry {
public:
    static void add_factory(MessageFactory *factory);

    static int get_message_type(const std::string& name);
    static const std::string& get_message_type_name(int type);
    static Message *new_message(int type);

private:
    static MessageTypeRegistry& get_instance() {
        return instance;
    }

private:
    std::vector<MessageFactory *> factories;

    static MessageTypeRegistry instance;
    static std::string undefined_message_type_str;
};

#define create_message(s, ...) boost::make_shared<s ## Message>(s, ##__VA_ARGS__)

#endif
