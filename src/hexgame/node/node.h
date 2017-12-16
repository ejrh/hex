#ifndef NODE_H
#define NODE_H

#include "hexutil/messaging/publisher.h"
#include "hexutil/messaging/receiver.h"

#include "hexgame/game/game.h"
#include "hexgame/game/game_arbiter.h"
#include "hexgame/game/game_updater.h"
#include "hexgame/game/throttle.h"


class NodeInterface: public MessageReceiver {
public:
    virtual ~NodeInterface() { }
    virtual void update() = 0;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual void subscribe(MessageReceiver *receiver) = 0;
    virtual Throttle& get_throttle() = 0;
};

class LocalNode: public NodeInterface {
public:
    LocalNode():
            game(), game_updater(&game), publisher(1000), throttle(&publisher), arbiter(&game, &throttle), dispatch_queue(1000), update_logger("Update: ") {
        publisher.subscribe(&update_logger);
        publisher.subscribe(&game_updater);
    }

    virtual void receive(Message *command) {
        dispatch_queue.receive(command);
    }

    virtual void update() {
        dispatch_queue.flush(&arbiter);
    }

    virtual void start() {
        for (auto iter = ais.begin(); iter != ais.end(); iter++) {
            Ai *ai = *iter;
            ai->start();
        }
    }

    virtual void stop() {
        for (auto iter = ais.begin(); iter != ais.end(); iter++) {
            Ai *ai = *iter;
            ai->stop();
            delete ai;
        }
    }

    virtual void subscribe(MessageReceiver *receiver) {
        publisher.subscribe(receiver);
    }

    void add_ai(const std::string& faction_type) {
        Ai *ai = new Ai(faction_type, &dispatch_queue);
        subscribe(ai->get_receiver());
        ais.push_back(ai);
    }

    MessageReceiver& get_publisher() {
        return publisher;
    }

    Throttle& get_throttle() {
        return throttle;
    }

protected:
    Game game;
    GameUpdater game_updater;
    Publisher publisher;
    Throttle throttle;
    GameArbiter arbiter;
    MessageQueue dispatch_queue;
    MessageLogger update_logger;
    std::vector<Ai *> ais;
};

class ServerNode: public LocalNode {
public:
    ServerNode():
            server(9999, &arbiter) {
        publisher.subscribe(&server);
    }

    virtual void start() {
        LocalNode::start();
        server.start();
    }

    virtual void stop() {
        server.stop();
    }

private:
    Server server;
};

class ClientNode: public NodeInterface {
public:
    ClientNode(const std::string& host_addr):
            host_addr(host_addr), update_queue(1000), client(&update_queue), throttle(&publisher) {
    }

    virtual void receive(Message *command) {
        client.receive(command);
    }

    virtual void update() {
        update_queue.flush(&throttle);
    }

    virtual void start() {
        client.connect(host_addr);
    }

    virtual void stop() {
        client.disconnect();
    }

    virtual void subscribe(MessageReceiver *receiver) {
        publisher.subscribe(receiver);
    }

    Throttle& get_throttle() {
        return throttle;
    }

private:
    std::string host_addr;
    MessageQueue update_queue;
    Client client;
    Throttle throttle;
    Publisher publisher;
};

#endif
