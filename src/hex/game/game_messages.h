#ifndef GAME_MESSAGES_H
#define GAME_MESSAGES_H

#include "hex/basics/point.h"
#include "hex/game/game_serialisation.h"


#define MSG_TYPE(s, c) s,
enum MessageType {
    UndefinedMessageType,
#include "hex/game/message_types.h"
    NUM_MESSAGE_TYPES
};
#undef MSG_TYPE


class CreateStackMessage: public Message {
public:
    CreateStackMessage() { }
    CreateStackMessage(int stack_id, const Point position): Message(CreateStack), stack_id(stack_id), position(position) { }
    virtual ~CreateStackMessage() { }

    int stack_id;
    Point position;

protected:
    virtual void read(Deserialiser& deserialiser) {
        deserialiser >> stack_id;
        deserialiser >> position;
    }

    virtual void write(Serialiser& serialiser) const {
        serialiser << stack_id;
        serialiser << position;
    };
};


class CreateUnitMessage: public Message {
public:
    CreateUnitMessage() { }
    CreateUnitMessage(int stack_id, const std::string& type_name): Message(CreateUnit), stack_id(stack_id), type_name(type_name) { }
    virtual ~CreateUnitMessage() { }

    int stack_id;
    std::string type_name;

protected:
    virtual void read(Deserialiser& deserialiser) {
        deserialiser >> stack_id;
        deserialiser >> type_name;
    }

    virtual void write(Serialiser& serialiser) const {
        serialiser << stack_id;
        serialiser << type_name;
    };
};


class PlayerReadyMessage: public Message {
public:
    PlayerReadyMessage() { }
    PlayerReadyMessage(int player, bool ready): Message(PlayerReady), player(player), ready(ready) { }
    virtual ~PlayerReadyMessage() { }

    int player;
    bool ready;

protected:
    virtual void read(Deserialiser& deserialiser) {
        deserialiser >> player;
        deserialiser >> ready;
    }

    virtual void write(Serialiser& serialiser) const {
        serialiser << player;
        serialiser << ready;
    };
};


class PlayerNameMessage: public Message {
public:
    PlayerNameMessage() { }
    PlayerNameMessage(int player, const std::string& name): Message(PlayerName), player(player), name(name) { }
    virtual ~PlayerNameMessage() { }

    int player;
    std::string name;

protected:
    virtual void read(Deserialiser& deserialiser) {
        deserialiser >> player;
        deserialiser >> name;
    }

    virtual void write(Serialiser& serialiser) const {
        serialiser << player;
        serialiser << name;
    };
};


class TurnBeginMessage: public Message {
public:
    TurnBeginMessage() { }
    TurnBeginMessage(int turn_number): Message(TurnBegin), turn_number(turn_number) { }
    virtual ~TurnBeginMessage() { }

    int turn_number;

protected:
    virtual void read(Deserialiser& deserialiser) {
        deserialiser >> turn_number;
    }

    virtual void write(Serialiser& serialiser) const {
        serialiser << turn_number;
    };
};


class TurnEndMessage: public Message {
public:
    TurnEndMessage() { }
    TurnEndMessage(int turn_number): Message(TurnEnd), turn_number(turn_number) { }
    virtual ~TurnEndMessage() { }

    int turn_number;

protected:
    virtual void read(Deserialiser& deserialiser) {
        deserialiser >> turn_number;
    }

    virtual void write(Serialiser& serialiser) const {
        serialiser << turn_number;
    };
};


class UnitMoveMessage: public Message {
public:
    UnitMoveMessage() { }
    UnitMoveMessage(int unit, std::vector<Point> path): Message(UnitMove), unit(unit), path(path) { }
    virtual ~UnitMoveMessage() { }

    int unit;
    std::vector<Point> path;

protected:
    virtual void read(Deserialiser& deserialiser) {
        deserialiser >> unit;
        deserialiser >> path;
    }

    virtual void write(Serialiser& serialiser) const {
        serialiser << unit;
        serialiser << path;
    };
};


#endif
