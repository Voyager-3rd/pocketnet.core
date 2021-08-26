// Copyright (c) 2018-2021 Pocketnet developers
// Distributed under the Apache 2.0 software license, see the accompanying
// https://www.apache.org/licenses/LICENSE-2.0

#ifndef POCKETHELPERS_TRANSACTIONHELPER_H
#define POCKETHELPERS_TRANSACTIONHELPER_H

#include <string>
#include <key_io.h>
#include <boost/algorithm/string.hpp>
#include <numeric>
#include "script/standard.h"
#include "primitives/transaction.h"

#include "pocketdb/models/base/ReturnDtoModels.h"
#include "pocketdb/models/base/PocketTypes.h"
#include "pocketdb/models/dto/Blocking.h"
#include "pocketdb/models/dto/BlockingCancel.h"
#include "pocketdb/models/dto/Coinbase.h"
#include "pocketdb/models/dto/Coinstake.h"
#include "pocketdb/models/dto/Default.h"
#include "pocketdb/models/dto/Post.h"
#include "pocketdb/models/dto/Video.h"
#include "pocketdb/models/dto/Comment.h"
#include "pocketdb/models/dto/CommentEdit.h"
#include "pocketdb/models/dto/CommentDelete.h"
#include "pocketdb/models/dto/Subscribe.h"
#include "pocketdb/models/dto/SubscribeCancel.h"
#include "pocketdb/models/dto/SubscribePrivate.h"
#include "pocketdb/models/dto/Complain.h"
#include "pocketdb/models/dto/User.h"
#include "pocketdb/models/dto/ScoreContent.h"
#include "pocketdb/models/dto/ScoreComment.h"

namespace PocketHelpers
{
    using namespace std;
    using namespace PocketTx;

    // Accumulate transactions in block
    typedef shared_ptr<PocketTx::Transaction> PTransactionRef;
    typedef vector<PTransactionRef> PocketBlock;
    typedef shared_ptr<PocketBlock> PocketBlockRef;

    class TransactionHelper
    {
    public:
        static txnouttype ScriptType(const CScript& scriptPubKey);

        static std::string ExtractDestination(const CScript& scriptPubKey);

        static tuple<bool, string> GetPocketAuthorAddress(const CTransactionRef& tx);

        static PocketTxType ConvertOpReturnToType(const string& op);

        static string ParseAsmType(const CTransactionRef& tx, vector<string>& vasm);

        static PocketTxType ParseType(const CTransactionRef& tx, vector<string>& vasm);

        static PocketTxType ParseType(const CTransactionRef& tx);

        static string ConvertToReindexerTable(const Transaction& transaction);

        static bool IsPocketSupportedTransaction(const CTransactionRef& tx, PocketTxType& txType);

        static bool IsPocketSupportedTransaction(const CTransactionRef& tx);

        static bool IsPocketTransaction(const CTransactionRef& tx, PocketTxType& txType);

        static bool IsPocketTransaction(const CTransactionRef& tx);

        static bool IsPocketTransaction(const CTransaction& tx);

        static tuple<bool, shared_ptr<ScoreDataDto>> ParseScore(const CTransactionRef& tx);

        static PTransactionRef CreateInstance(PocketTxType txType, const std::string& txHash, uint32_t nTime);
    };
}

#endif // POCKETHELPERS_TRANSACTIONHELPER_H