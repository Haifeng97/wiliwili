//
// Created by fang on 2022/7/10.
//

#include "view/video_card.hpp"
#include "view/svg_image.hpp"
#include "utils/number_helper.hpp"
#include "utils/image_helper.hpp"


/// 普通视频封面

RecyclingGridItemVideoCard::RecyclingGridItemVideoCard(){
    this->inflateFromXMLRes("xml/views/video_card.xml");
}

RecyclingGridItemVideoCard::~RecyclingGridItemVideoCard() {
    // 优先清空正在进行的图片请求
    ImageHelper::clear(this->picture);
}

void RecyclingGridItemVideoCard::setRCMDReason(std::string reason){
    this->boxRCMD->setVisibility(brls::Visibility::VISIBLE);
    this->labelRCMD->setText(reason);
    this->boxPic->setHeightPercentage(54.8);
}

void RecyclingGridItemVideoCard::setAchievement(std::string explain){
    this->boxAchievement->setVisibility(brls::Visibility::VISIBLE);
    this->labelAchievement->setText(explain);
    this->boxPic->setHeightPercentage(54.8);
}

void RecyclingGridItemVideoCard::prepareForReuse(){
    //准备显示该项
}

void RecyclingGridItemVideoCard::cacheForReuse(){
    //准备回收该项
    ImageHelper::clear(this->picture);
}

RecyclingGridItemVideoCard* RecyclingGridItemVideoCard::create(){
    return new RecyclingGridItemVideoCard();
}

void RecyclingGridItemVideoCard::setCard(std::string pic, std::string title, std::string username, int pubdate,
                                         int view_count, int danmaku, int duration){
    if(pubdate)
        this->labelUsername->setText(username + "·" +wiliwili::sec2date(pubdate));
    else
        this->labelUsername->setText(username);

    this->labelTitle->setIsWrapping(true);
    this->labelTitle->setText(title);
    ImageHelper::with(this)->load(pic)->into(this->picture);
    this->labelCount->setText(wiliwili::num2w(view_count));
    this->labelDanmaku->setText(wiliwili::num2w(danmaku));

    if(duration)
        this->labelDuration->setText(wiliwili::sec2Time(duration));
    else
        this->labelDuration->setText("");
}

void RecyclingGridItemVideoCard::setCard(std::string pic, std::string title, std::string username, int pubdate,
                                         int view_count, int danmaku, std::string rightBottomBadge){
    if(pubdate)
        this->labelUsername->setText(username + "·" +wiliwili::sec2date(pubdate));
    else
        this->labelUsername->setText(username);

    this->labelTitle->setIsWrapping(true);
    this->labelTitle->setText(title);
    ImageHelper::with(this)->load(pic)->into(this->picture);
    this->labelCount->setText(wiliwili::num2w(view_count));
    this->labelDanmaku->setText(wiliwili::num2w(danmaku));
    this->labelDuration->setText(rightBottomBadge);
}


/// 排行榜视频封面
/// 左上角有角标图案，从1开始自动添加序号


RecyclingGridItemRankVideoCard::RecyclingGridItemRankVideoCard(std::string res){
    this->inflateFromXMLRes(res);
}

RecyclingGridItemRankVideoCard::~RecyclingGridItemRankVideoCard(){
    // 优先清空正在进行的图片请求
    ImageHelper::clear(this->picture);
}

void RecyclingGridItemRankVideoCard::setCard(std::string pic, std::string title, std::string username, int pubdate,
             int view_count, int danmaku, int duration, int index){
    if(pubdate)
        this->labelUsername->setText(username + "·" +wiliwili::sec2date(pubdate));
    else
        this->labelUsername->setText(username);

    this->labelTitle->setIsWrapping(true);
    this->labelTitle->setText(title);
    ImageHelper::with(this)->load(pic)->into(this->picture);
    this->labelCount->setText(wiliwili::num2w(view_count));
    this->labelDanmaku->setText(wiliwili::num2w(danmaku));

    if(duration)
        this->labelDuration->setText(wiliwili::sec2Time(duration));
    else
        this->labelDuration->setText("");

    if(index < 10){
        this->labelIndex->setMarginLeft(4);
    } else{
        this->labelIndex->setMarginLeft(0);
    }

    this->labelIndex->setText(std::to_string(index));

    switch (index) {
        case 1:
            this->svgIndex->setImageFromSVGRes("svg/rate-crown-gold.svg");
            break;
        case 2:
            this->svgIndex->setImageFromSVGRes("svg/rate-crown-sliver.svg");
            break;
        case 3:
            this->svgIndex->setImageFromSVGRes("svg/rate-crown-copper.svg");
            break;
        default:
            this->svgIndex->setImageFromSVGRes("svg/rate-crown-iron.svg");
    }
}


void RecyclingGridItemRankVideoCard::prepareForReuse(){
    //准备显示该项
}

void RecyclingGridItemRankVideoCard::cacheForReuse(){
    //准备回收该项
    ImageHelper::clear(this->picture);
}

RecyclingGridItemRankVideoCard* RecyclingGridItemRankVideoCard::create(std::string res){
    return new RecyclingGridItemRankVideoCard(res);
}


/// 直播视频封面


RecyclingGridItemLiveVideoCard::RecyclingGridItemLiveVideoCard(){
    this->inflateFromXMLRes("xml/views/video_card_live.xml");
}

RecyclingGridItemLiveVideoCard::~RecyclingGridItemLiveVideoCard(){
    // 优先清空正在进行的图片请求
    ImageHelper::clear(this->picture);
}

void RecyclingGridItemLiveVideoCard::setCard(std::string pic, std::string title, std::string username,
                                             std::string area, int view_count){

    this->labelUsername->setText(username);
    this->labelTitle->setIsWrapping(false);
    this->labelTitle->setText(title);
    ImageHelper::with(this)->load(pic)->into(this->picture);
    this->labelCount->setText(wiliwili::num2w(view_count));
    this->labelDuration->setText(area);
}


void RecyclingGridItemLiveVideoCard::prepareForReuse(){
    //准备显示该项
}

void RecyclingGridItemLiveVideoCard::cacheForReuse(){
    //准备回收该项
    ImageHelper::clear(this->picture);
}

RecyclingGridItemLiveVideoCard* RecyclingGridItemLiveVideoCard::create(){
    return new RecyclingGridItemLiveVideoCard();
}



/// pgc video card
/// 支持预览图横竖切换的视频封面


RecyclingGridItemPGCVideoCard::RecyclingGridItemPGCVideoCard(bool vertical_cover): vertical_cover(vertical_cover){
    this->inflateFromXMLRes("xml/views/video_card_pgc.xml");
    if(!vertical_cover){
        this->boxPic->setHeightPercentage(70);
        this->boxBadgeBottom->setHeightPercentage(20);
    }
}

RecyclingGridItemPGCVideoCard::~RecyclingGridItemPGCVideoCard(){
    // 优先清空正在进行的图片请求
    ImageHelper::clear(this->picture);
}

bool RecyclingGridItemPGCVideoCard::isVertical(){
    return this->vertical_cover;
}

void RecyclingGridItemPGCVideoCard::setCard(std::string pic, std::string title, std::string username,
                                            std::string badge_top, std::string badge_bottom_left,
                                            std::string badge_bottom_right){

    this->labelUsername->setText(username);
    this->labelTitle->setIsWrapping(false);
    this->labelTitle->setText(title);
    ImageHelper::with(this)->load(pic)->into(this->picture);
    this->labelDuration->setText(badge_bottom_right);

    if(!badge_top.empty()){
        ImageHelper::with(this)->load(badge_top)->into(this->badgeTop);
    }

    if(!badge_bottom_left.empty()){
        ImageHelper::with(this)->load(badge_bottom_left)->into(this->badgeBottomLeft);
    }

}


void RecyclingGridItemPGCVideoCard::prepareForReuse(){
    //准备显示该项
}

void RecyclingGridItemPGCVideoCard::cacheForReuse(){
    //准备回收该项
    ImageHelper::clear(this->picture);
    ImageHelper::clear(this->badgeTop);
    ImageHelper::clear(this->badgeBottomLeft);
}

RecyclingGridItemPGCVideoCard* RecyclingGridItemPGCVideoCard::create(bool vertical_cover){
    return new RecyclingGridItemPGCVideoCard(vertical_cover);
}


/// PGC 查看更多卡片

RecyclingGridItemViewMoreCard::RecyclingGridItemViewMoreCard(bool vertical_cover):vertical_cover(vertical_cover){
    this->inflateFromXMLRes("xml/views/video_card_pgc_more.xml");
}

RecyclingGridItemViewMoreCard::~RecyclingGridItemViewMoreCard(){

}

void RecyclingGridItemViewMoreCard::prepareForReuse(){

}

void RecyclingGridItemViewMoreCard::cacheForReuse(){

}

bool RecyclingGridItemViewMoreCard::isVertical(){
    return this->vertical_cover;
}

RecyclingGridItem* RecyclingGridItemViewMoreCard::create(bool vertical_cover){
    return new RecyclingGridItemViewMoreCard(vertical_cover);
}


/// 历史记录 视频卡片

RecyclingGridItemHistoryVideoCard::RecyclingGridItemHistoryVideoCard(){
    this->inflateFromXMLRes("xml/views/video_card_history.xml");
}

RecyclingGridItemHistoryVideoCard::~RecyclingGridItemHistoryVideoCard() {
    // 优先清空正在进行的图片请求
    ImageHelper::clear(this->picture);
}

void RecyclingGridItemHistoryVideoCard::prepareForReuse(){
    //准备显示该项
}

void RecyclingGridItemHistoryVideoCard::cacheForReuse(){
    //准备回收该项
    ImageHelper::clear(this->picture);
}

RecyclingGridItemHistoryVideoCard* RecyclingGridItemHistoryVideoCard::create(){
    return new RecyclingGridItemHistoryVideoCard();
}

void RecyclingGridItemHistoryVideoCard::setCard(std::string pic, std::string title, std::string username,
                                                std::string leftBottomBadge, std::string rightBottomBadge,
                                                std::string rightTopBadge){
    this->labelUsername->setText(username);
    this->labelTitle->setIsWrapping(true);
    this->labelTitle->setText(title);
    ImageHelper::with(this)->load(pic)->into(this->picture);
    this->labelCount->setText(leftBottomBadge);
    this->labelDuration->setText(rightBottomBadge);
    this->labelRightTop->setText(rightTopBadge);
}


/// 收藏夹 卡片

RecyclingGridItemCollectionVideoCard::RecyclingGridItemCollectionVideoCard(){
    this->inflateFromXMLRes("xml/views/video_card_collection.xml");
}

RecyclingGridItemCollectionVideoCard::~RecyclingGridItemCollectionVideoCard() {
    // 优先清空正在进行的图片请求
    ImageHelper::clear(this->picture);
}

void RecyclingGridItemCollectionVideoCard::prepareForReuse(){
    //准备显示该项
}

void RecyclingGridItemCollectionVideoCard::cacheForReuse(){
    //准备回收该项
    ImageHelper::clear(this->picture);
}

RecyclingGridItemCollectionVideoCard* RecyclingGridItemCollectionVideoCard::create(){
    return new RecyclingGridItemCollectionVideoCard();
}

void RecyclingGridItemCollectionVideoCard::setCard(std::string pic, std::string title, std::string username,
                                                std::string leftBottomBadge, std::string rightBottomBadge){
    this->labelUsername->setText(username);
    this->labelTitle->setIsWrapping(true);
    this->labelTitle->setText(title);
    if(pic.empty()){
        brls::sync([this](){
            this->picture->setImageFromRes("pictures/playlistbg.png");
        });
    } else{
        ImageHelper::with(this)->load(pic)->into(this->picture);
    }
    this->labelCount->setText(leftBottomBadge);
    this->labelDuration->setText(rightBottomBadge);
}
