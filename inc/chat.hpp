#pragma once

#include <stdint.h>
#include "inc/db.hpp"
#include "inc/contact.hpp"

class ChatTable : public Table {
    protected:
        bool receiving;
    public:
        ChatTable(std::shared_ptr<Database> db,
                  std::string chatName,
                  std::function<void(std::shared_ptr<Database>)> func,
                  bool receiver)
            : Table(db, chatName, func)
            , receiving(receiver)
        {}
};

class SentChatTable : public ChatTable {
public:
    SentChatTable(std::shared_ptr<Database> db)
        : ChatTable(db, "sent", [](std::shared_ptr<Database> db) -> void {
            SQLPrepStatement stmt(std::move(db),
                    "CREATE TABLE IF NOT EXISTS sent\
                     (id INTEGER PRIMARY KEY,\
                      contact INTEGER,\
                      contents BLOB,\
                      timestamp INTEGER,\
                      nonce BLOB);");
            stmt.step();
        }, false)
    {}
};

class ReceivedChatTable : public ChatTable {
public:
    ReceivedChatTable(std::shared_ptr<Database> db)
        : ChatTable(db, "received", [](std::shared_ptr<Database> db) -> void {
            SQLPrepStatement stmt(std::move(db),
                    "CREATE TABLE IF NOT EXISTS received\
                     (id INTEGER PRIMARY KEY,\
                      contact INTEGER,\
                      contents BLOB,\
                      timestamp INTEGER,\
                      nonce BLOB);");
            stmt.step();
        }, true)
    {}
};

struct ChatMessage {
    int64_t id; ///< ID from database
    /// Reference either to contact id inside the database or a local instance
    std::variant<int64_t, Contact> contact;
    std::vector<unsigned char> contents;
    uint64_t timestamp; ///< Unix timestamp in seconds
    std::vector<unsigned char> nonce; ///< Nonce needed to decrypt message
    bool receiver; ///< Specify whether we are the sender or receiver

    ChatMessage()=default;
    ChatMessage(int64_t rowid, std::variant<int64_t, Contact> contactRef,
                std::vector<unsigned char> message, uint64_t unixTimestamp,
                std::vector<unsigned char> messageNonce, bool receiveTable)
        : id(rowid)
        , contact(contactRef)
        , contents(message)
        , timestamp(unixTimestamp)
        , nonce(messageNonce)
        , receiver(receiveTable)
    { }
};
