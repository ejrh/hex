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


class CreateFactionMessage: public Message {
public:
    CreateFactionMessage() { }
    CreateFactionMessage(int faction_id, const std::string& type_name, const std::string& name): Message(CreateFaction), faction_id(faction_id), type_name(type_name), name(name) { }
    virtual ~CreateFactionMessage() { }

    int faction_id;
    std::string type_name;
    std::string name;

protected:
    virtual void read(Deserialiser& deserialiser) {
        deserialiser >> faction_id >> type_name >> name;
    }

    virtual void write(Serialiser& serialiser) const {
        serialiser << faction_id << type_name << name;
    };
};


class CreateStackMessage: public Message {
public:
    CreateStackMessage() { }
    CreateStackMessage(int stack_id, const Point position, int owner): Message(CreateStack), stack_id(stack_id), position(position), owner(owner) { }
    virtual ~CreateStackMessage() { }

    int stack_id;
    Point position;
    int owner;

protected:
    virtual void read(Deserialiser& deserialiser) {
        deserialiser >> stack_id >> position >> owner;
    }

    virtual void write(Serialiser& serialiser) const {
        serialiser << stack_id << position << owner;
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


class FactionReadyMessage: public Message {
public:
    FactionReadyMessage() { }
    FactionReadyMessage(int faction, bool ready): Message(FactionReady), faction(faction), ready(ready) { }
    virtual ~FactionReadyMessage() { }

    int faction;
    bool ready;

protected:
    virtual void read(Deserialiser& deserialiser) {
        deserialiser >> faction >> ready;
    }

    virtual void write(Serialiser& serialiser) const {
        serialiser << faction << ready;
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
