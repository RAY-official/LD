#include "BlockchainIndices.h"

#include "common/StringTools.h"
#include "base/CryptoNoteTools.h"
#include "base/CryptoNoteFormatUtils.h"
#include "blockchain_explorer/BlockchainExplorerDataBuilder.h"
#include "base/CryptoNoteBasicImpl.h"

namespace CryptoNote {

namespace {
  const size_t DEFAULT_BUCKET_COUNT = 5;
}

PaymentIdIndex::PaymentIdIndex(bool _enabled) : enabled(_enabled), index(DEFAULT_BUCKET_COUNT, paymentIdHash) {
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
bool PaymentIdIndex::add(const Transaction& transaction) {
  if (!enabled) {
    return false;
  }

  Crypto::Hash paymentId;
  Crypto::Hash transactionHash = getObjectHash(transaction);
  if (!BlockchainExplorerDataBuilder::getPaymentId(transaction, paymentId)) {
    return false;
  }

  index.emplace(paymentId, transactionHash);

  return true;
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
bool PaymentIdIndex::remove(const Transaction& transaction) {
  if (!enabled) {
    return false;
  }

  Crypto::Hash paymentId;
  Crypto::Hash transactionHash = getObjectHash(transaction);
  if (!BlockchainExplorerDataBuilder::getPaymentId(transaction, paymentId)) {
    return false;
  }

  auto range = index.equal_range(paymentId);
  for (auto iter = range.first; iter != range.second; ++iter){
    if (iter->second == transactionHash) {
      index.erase(iter);
      return true;
    }
  }

  return false;
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
bool PaymentIdIndex::find(const Crypto::Hash& paymentId, std::vector<Crypto::Hash>& transactionHashes) {
  if (!enabled) {
    throw std::runtime_error("Payment id index disabled.");
  }

  bool found = false;
  auto range = index.equal_range(paymentId);
  for (auto iter = range.first; iter != range.second; ++iter){
    found = true;
    transactionHashes.emplace_back(iter->second);
  }
  return found;
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
void PaymentIdIndex::clear() {
  if (enabled) {
    index.clear();
  }
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
void PaymentIdIndex::serialize(ISerializer& s) {
  if (!enabled) {
    throw std::runtime_error("Payment id index disabled.");
  }

  s(index, "index");
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
TimestampBlocksIndex::TimestampBlocksIndex(bool _enabled) : enabled(_enabled) {
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
bool TimestampBlocksIndex::add(uint64_t timestamp, const Crypto::Hash& hash) {
  if (!enabled) {
    return false;
  }

  index.emplace(timestamp, hash);
  return true;
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
bool TimestampBlocksIndex::remove(uint64_t timestamp, const Crypto::Hash& hash) {
  if (!enabled) {
    return false;
  }

  auto range = index.equal_range(timestamp);
  for (auto iter = range.first; iter != range.second; ++iter) {
    if (iter->second == hash) {
      index.erase(iter);
      return true;
    }
  }

  return false;
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
bool TimestampBlocksIndex::find(uint64_t timestampBegin, uint64_t timestampEnd, uint32_t hashesNumberLimit, std::vector<Crypto::Hash>& hashes, uint32_t& hashesNumberWithinTimestamps) {
  if (!enabled) {
    throw std::runtime_error("Timestamp block index disabled.");
  }

  uint32_t hashesNumber = 0;
  if (timestampBegin > timestampEnd) {
    //std::swap(timestampBegin, timestampEnd);
    return false;
  }
  auto begin = index.lower_bound(timestampBegin);
  auto end = index.upper_bound(timestampEnd);

  hashesNumberWithinTimestamps = static_cast<uint32_t>(std::distance(begin, end));

  for (auto iter = begin; iter != end && hashesNumber < hashesNumberLimit; ++iter){
    ++hashesNumber;
    hashes.emplace_back(iter->second);
  }
  return hashesNumber > 0;
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
void TimestampBlocksIndex::clear() {
  if (enabled) {
    index.clear();
  }
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
void TimestampBlocksIndex::serialize(ISerializer& s) {
  if (!enabled) {
    throw std::runtime_error("Timestamp block index disabled.");
  }

  s(index, "index");
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
TimestampTransactionsIndex::TimestampTransactionsIndex(bool _enabled) : enabled(_enabled) {
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
bool TimestampTransactionsIndex::add(uint64_t timestamp, const Crypto::Hash& hash) {
  if (!enabled) {
    return false;
  }

  index.emplace(timestamp, hash);
  return true;
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
bool TimestampTransactionsIndex::remove(uint64_t timestamp, const Crypto::Hash& hash) {
  if (!enabled) {
    return false;
  }

  auto range = index.equal_range(timestamp);
  for (auto iter = range.first; iter != range.second; ++iter) {
    if (iter->second == hash) {
      index.erase(iter);
      return true;
    }
  }

  return false;
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
bool TimestampTransactionsIndex::find(uint64_t timestampBegin, uint64_t timestampEnd, uint64_t hashesNumberLimit, std::vector<Crypto::Hash>& hashes, uint64_t& hashesNumberWithinTimestamps) {
  if (!enabled) {
    throw std::runtime_error("Timestamp transactions index disabled.");
  }

  uint32_t hashesNumber = 0;
  if (timestampBegin > timestampEnd) {
    //std::swap(timestampBegin, timestampEnd);
    return false;
  }
  auto begin = index.lower_bound(timestampBegin);
  auto end = index.upper_bound(timestampEnd);
  if (timestampEnd == static_cast<uint64_t>(0) && end == begin && begin == index.begin() && index.size() > 0)
	  ++end; //fix for genesis non-zero timestamp

  hashesNumberWithinTimestamps = static_cast<uint32_t>(std::distance(begin, end));

  for (auto iter = begin; iter != end && hashesNumber < hashesNumberLimit; ++iter) {
    ++hashesNumber;
    hashes.emplace_back(iter->second);
  }

  return hashesNumber > 0;
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
void TimestampTransactionsIndex::clear() {
  if (enabled) {
    index.clear();
  }
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
void TimestampTransactionsIndex::serialize(ISerializer& s) {
  if (!enabled) {
    throw std::runtime_error("Timestamp transactions index disabled.");
  }

  s(index, "index");
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
GeneratedTransactionsIndex::GeneratedTransactionsIndex(bool _enabled) : lastGeneratedTxNumber(0), enabled(_enabled) {
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
bool GeneratedTransactionsIndex::add(const Block& block) {
  if (!enabled) {
    return false;
  }

  uint32_t blockHeight = boost::get<BaseInput>(block.baseTransaction.inputs.front()).blockIndex;

  if (index.size() != blockHeight) {
    return false;
  } 

  bool status = index.emplace(blockHeight, lastGeneratedTxNumber + block.transactionHashes.size() + 1).second; //Plus miner tx
  if (status) {
    lastGeneratedTxNumber += block.transactionHashes.size() + 1;
  }
  return status;
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
bool GeneratedTransactionsIndex::remove(const Block& block) {
  if (!enabled) {
    return false;
  }

  uint32_t blockHeight = boost::get<BaseInput>(block.baseTransaction.inputs.front()).blockIndex;

  if (blockHeight != index.size() - 1) {
    return false;
  }

  auto iter = index.find(blockHeight);
  assert(iter != index.end());
  index.erase(iter);

  if (blockHeight != 0) {
    iter = index.find(blockHeight - 1);
    assert(iter != index.end());
    lastGeneratedTxNumber = iter->second;
  } else {
    lastGeneratedTxNumber = 0;
  }
  
  return true;
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
bool GeneratedTransactionsIndex::find(uint32_t height, uint64_t& generatedTransactions) {
  if (!enabled) {
    throw std::runtime_error("Generated transactions index disabled.");
  }

  if (height > std::numeric_limits<uint32_t>::max()) {
    return false;
  }
  auto iter = index.find(height);
  if (iter == index.end()) {
    return false;
  }
  generatedTransactions = iter->second;
  return true;
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
void GeneratedTransactionsIndex::clear() {
  if (enabled) {
    index.clear();
  }
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
void GeneratedTransactionsIndex::serialize(ISerializer& s) {
  if (!enabled) {
    throw std::runtime_error("Generated transactions index disabled.");
  }

  s(index, "index");
  s(lastGeneratedTxNumber, "lastGeneratedTxNumber");
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
OrphanBlocksIndex::OrphanBlocksIndex(bool _enabled) : enabled(_enabled) {
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
bool OrphanBlocksIndex::add(const Block& block) {
  if (!enabled) {
    return false;
  }

  Crypto::Hash blockHash = get_block_hash(block);
  uint32_t blockHeight = boost::get<BaseInput>(block.baseTransaction.inputs.front()).blockIndex;
  index.emplace(blockHeight, blockHash);
  return true;
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
bool OrphanBlocksIndex::remove(const Block& block) {
  if (!enabled) {
    return false;
  }

  Crypto::Hash blockHash = get_block_hash(block);
  uint32_t blockHeight = boost::get<BaseInput>(block.baseTransaction.inputs.front()).blockIndex;
  auto range = index.equal_range(blockHeight);
  for (auto iter = range.first; iter != range.second; ++iter) {
    if (iter->second == blockHash) {
      index.erase(iter);
      return true;
    }
  }

  return false;
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
bool OrphanBlocksIndex::find(uint32_t height, std::vector<Crypto::Hash>& blockHashes) {
  if (!enabled) {
    throw std::runtime_error("Orphan blocks index disabled.");
  }

  if (height > std::numeric_limits<uint32_t>::max()) {
    return false;
  }
  bool found = false;
  auto range = index.equal_range(height);
  for (auto iter = range.first; iter != range.second; ++iter) {
    found = true;
    blockHashes.emplace_back(iter->second);
  }
  return found;
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
void OrphanBlocksIndex::clear() {
  if (enabled) {
    index.clear();
  }
}
//------------------------------------------------------------- Seperator Code -------------------------------------------------------------//
}
