// Copyright (c) 2018-2021 Pocketnet developers
// Distributed under the Apache 2.0 software license, see the accompanying
// https://www.apache.org/licenses/LICENSE-2.0

#include "pocketdb/web/SearchRpc.h"

namespace PocketWeb::PocketWebRpc
{
    string lower(string s)
    {
        transform(s.begin(), s.end(), s.begin(), [](char c) { return 'A' <= c && c <= 'Z' ? c ^ 32 : c; });
        return s;
    }

    UniValue Search(const JSONRPCRequest& request)
    {
        if (request.fHelp)
            throw runtime_error(
                "search \"keyword\", \"type\", topBlock, pageStart, pageSize, \"address\"\n"
                "\nSearch data in DB.\n"
                "\nArguments:\n"
                "1. \"keyword\"     (string) String for search\n"
                "2. \"type\"        (string, optional) posts, videolink, tags, users\n"
                "3. \"topBlock\"  (int, optional) Top block for search.\n"
                "4. \"pageStart\" (int, optional) Pagination start. Default 0\n"
                "5. \"pageSize\" (int, optional) Pagination count. Default 10\n"
                "5. \"address\"     (string, optional) Filter by address\n"
            );

        RPCTypeCheck(request.params, {UniValue::VSTR, UniValue::VSTR});

        SearchRequest searchRequest;

        // General params
        searchRequest.Keyword = HtmlUtils::UrlDecode(request.params[0].get_str());
        string type = lower(request.params[1].get_str());

        // Optional parameters
        // TopBlock
        if (request.params.size() > 2)
        {
            try
            {
                ParseInt32(request.params[2].get_str(), &searchRequest.TopBlock);
            }
            catch (...) { }
        }

        // PageStart
        if (request.params.size() > 3)
        {
            try
            {
                ParseInt32(request.params[3].get_str(), &searchRequest.PageStart);
            }
            catch (...) { }
        }

        // PageSize
        if (request.params.size() > 4)
        {
            try
            {
                ParseInt32(request.params[4].get_str(), &searchRequest.PageSize);
            }
            catch (...) { }
        }

        // Address
        if (request.params.size() > 5) {
            RPCTypeCheckArgument(request.params[5], UniValue::VSTR);
            CTxDestination dest = DecodeDestination(request.params[5].get_str());
            if (!IsValidDestination(dest)) {
                throw JSONRPCError(RPC_INVALID_ADDRESS_OR_KEY, string("Invalid Pocketcoin address: ") + request.params[5].get_str());
            }
            searchRequest.Address = request.params[5].get_str();
        }

        // -----------------------------------------------------------
        UniValue result(UniValue::VOBJ);
        
        // Search simple tags without join content data
        if (type == "tags")
        {
            UniValue data = request.DbConnection()->SearchRepoInst->SearchTags(searchRequest);
            result.pushKV("tags", UniValue(UniValue::VOBJ));
            result.At("tags").pushKV("data", data);
        }

        // Search posts in caption, message and urls
        if (type == "posts")
        {
            searchRequest.TxTypes = { CONTENT_POST, CONTENT_VIDEO };
            searchRequest.FieldTypes = {
                ContentFieldType_ContentPostCaption,
                ContentFieldType_ContentVideoCaption,
                ContentFieldType_ContentPostMessage,
                ContentFieldType_ContentVideoMessage,
                ContentFieldType_ContentPostUrl,
                ContentFieldType_ContentVideoUrl
            };

            // Search
            auto ids = request.DbConnection()->SearchRepoInst->SearchIds(searchRequest);
            
            // Get content data
            auto contents = request.DbConnection()->WebRpcRepoInst->GetContentsData(ids, searchRequest.Address);
            
            UniValue data(UniValue::VARR);
            for (const auto& content : contents)
                data.push_back(content.second);

            result.pushKV("posts", UniValue(UniValue::VOBJ));
            result.At("posts").pushKV("data", data);
        }

        // Get all videos with requested link
        if (type == "videolink")
        {
            searchRequest.TxTypes = { CONTENT_VIDEO };
            searchRequest.FieldTypes = { ContentFieldType_ContentVideoUrl };

            // Search
            auto ids = request.DbConnection()->SearchRepoInst->SearchIds(searchRequest);
            
            // Get content data
            auto contents = request.DbConnection()->WebRpcRepoInst->GetContentsData(ids, searchRequest.Address);

            UniValue data(UniValue::VARR);
            for (const auto& content : contents)
                data.push_back(content.second);

            result.pushKV("videos", UniValue(UniValue::VOBJ));
            result.At("videos").pushKV("data", data);
        }

        // Search users
        if (type == "users")
        {
            searchRequest.Address = "";
            searchRequest.TxTypes = { ACCOUNT_USER };
            searchRequest.FieldTypes = {
                ContentFieldType_AccountUserName,
                ContentFieldType_AccountUserAbout,
                ContentFieldType_AccountUserUrl
            };

            // Search
            auto ids = request.DbConnection()->SearchRepoInst->SearchIds(searchRequest);
            
            // Get accounts data
            auto accounts = request.DbConnection()->WebRpcRepoInst->GetAccountProfiles(ids, true, 1);

            UniValue data(UniValue::VARR);
            for (const auto& account : accounts)
                data.push_back(account.second);

            result.pushKV("users", UniValue(UniValue::VOBJ));
            result.At("users").pushKV("data", data);
        }
        
        // Send result
        
        return result;
    }
}