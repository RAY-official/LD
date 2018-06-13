#include "BlockchainMessages.h"

namespace CryptoNote {

NewBlockMessage::NewBlockMessage(const Crypto::Hash& hash) : blockHash(hash) {}

void NewBlockMessage::get(Crypto::Hash& hash) const {
  hash = blockHash;
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
NewAlternativeBlockMessage::NewAlternativeBlockMessage(const Crypto::Hash& hash) : blockHash(hash) {}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
void NewAlternativeBlockMessage::get(Crypto::Hash& hash) const {
  hash = blockHash;
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
ChainSwitchMessage::ChainSwitchMessage(std::vector<Crypto::Hash>&& hashes) : blocksFromCommonRoot(std::move(hashes)) {}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
ChainSwitchMessage::ChainSwitchMessage(const ChainSwitchMessage& other) : blocksFromCommonRoot(other.blocksFromCommonRoot) {}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
void ChainSwitchMessage::get(std::vector<Crypto::Hash>& hashes) const {
  hashes = blocksFromCommonRoot;
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
BlockchainMessage::BlockchainMessage(NewBlockMessage&& message) : type(MessageType::NEW_BLOCK_MESSAGE), newBlockMessage(std::move(message)) {}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
BlockchainMessage::BlockchainMessage(NewAlternativeBlockMessage&& message) : type(MessageType::NEW_ALTERNATIVE_BLOCK_MESSAGE), newAlternativeBlockMessage(std::move(message)) {}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
BlockchainMessage::BlockchainMessage(ChainSwitchMessage&& message) : type(MessageType::CHAIN_SWITCH_MESSAGE) {
	chainSwitchMessage = new ChainSwitchMessage(std::move(message));
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
BlockchainMessage::BlockchainMessage(const BlockchainMessage& other) : type(other.type) {
  switch (type) {
    case MessageType::NEW_BLOCK_MESSAGE:
      new (&newBlockMessage) NewBlockMessage(other.newBlockMessage);
      break;
    case MessageType::NEW_ALTERNATIVE_BLOCK_MESSAGE:
      new (&newAlternativeBlockMessage) NewAlternativeBlockMessage(other.newAlternativeBlockMessage);
      break;
    case MessageType::CHAIN_SWITCH_MESSAGE:
	  chainSwitchMessage = new ChainSwitchMessage(*other.chainSwitchMessage);
      break;
  }
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
BlockchainMessage::~BlockchainMessage() {
  switch (type) {
    case MessageType::NEW_BLOCK_MESSAGE:
      newBlockMessage.~NewBlockMessage();
      break;
    case MessageType::NEW_ALTERNATIVE_BLOCK_MESSAGE:
      newAlternativeBlockMessage.~NewAlternativeBlockMessage();
      break;
    case MessageType::CHAIN_SWITCH_MESSAGE:
	  delete chainSwitchMessage;
      break;
  }
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
BlockchainMessage::MessageType BlockchainMessage::getType() const {
  return type;
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
bool BlockchainMessage::getNewBlockHash(Crypto::Hash& hash) const {
  if (type == MessageType::NEW_BLOCK_MESSAGE) {
    newBlockMessage.get(hash);
    return true;
  } else {
    return false;
  }
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
bool BlockchainMessage::getNewAlternativeBlockHash(Crypto::Hash& hash) const {
  if (type == MessageType::NEW_ALTERNATIVE_BLOCK_MESSAGE) {
    newAlternativeBlockMessage.get(hash);
    return true;
  } else {
    return false;
  }
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
bool BlockchainMessage::getChainSwitch(std::vector<Crypto::Hash>& hashes) const {
  if (type == MessageType::CHAIN_SWITCH_MESSAGE) {
    chainSwitchMessage->get(hashes);
    return true;
  } else {
    return false;
  }
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
}
