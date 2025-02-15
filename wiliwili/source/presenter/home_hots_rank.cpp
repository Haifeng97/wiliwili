//
// Created by fang on 2022/7/10.
//

#include "bilibili.h"
#include "presenter/home_hots_rank.hpp"

vector<string> HomeHotsRankRequest::getRankList(){
    vector<string> res;
    for(auto it: rankList){
        res.push_back(it.key);
    }
    return res;
}

void HomeHotsRankRequest::requestData(size_t index) {
    if(index >= rankList.size()) return;
    auto item = rankList[index];
    if(item.type){
        this->requestHotsRankPGCVideoList(item.id);
    } else {
        this->requestHotsRankVideoList(item.id, item.extra);
    }

}

void HomeHotsRankRequest::requestHotsRankVideoList(int rid, string type) {
    bilibili::BilibiliClient::get_hots_rank(rid, type,
                                            [this](auto result, auto note){
                                                this->onHotsRankList(result, note);
                                            }, [this](const std::string &error) {
                this->onError(error);
            });
}

void HomeHotsRankRequest::requestHotsRankPGCVideoList(int season_type, int day) {
    bilibili::BilibiliClient::get_hots_rank_pgc(season_type, day,
                                                [this](auto result, auto explain){
                                                    this->onHotsRankPGCList(result, explain);
                                                }, [this](const std::string &error) {
                this->onError(error);
            });
}