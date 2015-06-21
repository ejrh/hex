#ifndef MESSAGE_H
#define MESSAGE_H

class Message;

/* These be defined by implementation. */
extern int get_message_type(const std::string& name);
extern const std::string get_message_type_name(int type);
extern Message *create_message(int type);

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


template<typename T>
class WrapperMessage: public Message {
public:
    WrapperMessage() { }
    WrapperMessage(T& data): Message(), data(data) { }
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
    WrapperMessage2(T1& data1, T2& data2): Message(), data1(data1), data2(data2) { }
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


extern Serialiser& operator<<(Serialiser& serialiser, const Message *msg);

extern Deserialiser& operator>>(Deserialiser& deserialiser, Message *& msg);

#endif
