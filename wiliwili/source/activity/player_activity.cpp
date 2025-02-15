//
// Created by fang on 2022/7/10.
//

#include <borealis.hpp>
#include "view/video_view.hpp"
#include "view/video_card.hpp"
#include "view/user_info.hpp"
#include "activity/player_activity.hpp"
#include "view/grid_dropdown.hpp"
#include "fmt/format.h"
#include "utils/number_helper.hpp"

class DataSourceList
        : public RecyclingGridDataSource
{
public:
    DataSourceList(std::vector<std::string> result, ChangeIndexEvent cb):data(result),changeEpisodeEvent(cb){}

    RecyclingGridItem* cellForRow(RecyclingGrid* recycler, size_t index) override{
        GridRadioCell* item = (GridRadioCell*)recycler->dequeueReusableCell("Cell");

        auto r = this->data[index];
        item->title->setText(this->data[index]);
//        item->setSelected(index == dropdown->getSelected());

        return item;
    }

    size_t getItemCount() override{
        return data.size();
    }

    void onItemSelected(RecyclingGrid* recycler, size_t index) override{
        changeEpisodeEvent.fire(index);
    }

private:
    std::vector<std::string> data;
    ChangeIndexEvent changeEpisodeEvent;
};

class DataSourceUserUploadedVideoList
        : public RecyclingGridDataSource
{
public:
    DataSourceUserUploadedVideoList(bilibili::UserUploadedVideoListResult result, ChangeVideoEvent cb):list(result),changeVideoEvent(cb){

    }
    RecyclingGridItem* cellForRow(RecyclingGrid* recycler, size_t index){
        //从缓存列表中取出 或者 新生成一个表单项
        RecyclingGridItemVideoCard* item = (RecyclingGridItemVideoCard*)recycler->dequeueReusableCell("Cell");

        bilibili::UserUploadedVideoResult& r = this->list[index];
        item->setCard(r.pic+"@672w_378h_1c.jpg",r.title,r.author, r.created, r.play, r.video_review, r.length);
        return item;
    }

    size_t getItemCount() {
        return list.size();
    }

    void onItemSelected(RecyclingGrid* recycler, size_t index) {
        changeVideoEvent.fire(list[index]);
    }

    void appendData(const bilibili::UserUploadedVideoListResult& data){
        this->list.insert(this->list.end(), data.begin(), data.end());
    }

private:
    bilibili::UserUploadedVideoListResult list;
    ChangeVideoEvent changeVideoEvent;
};

class DataSourceCommentList
        : public RecyclingGridDataSource
{
public:
    DataSourceCommentList(bilibili::VideoCommentListResult result):dataList(result){

    }
    RecyclingGridItem* cellForRow(RecyclingGrid* recycler, size_t index) override{
        //从缓存列表中取出 或者 新生成一个表单项
        VideoComment* item = (VideoComment*)recycler->dequeueReusableCell("Cell");

        item->setData(this->dataList[index]);
        return item;
    }

    size_t getItemCount() override{
        return dataList.size();
    }

    void onItemSelected(RecyclingGrid* recycler, size_t index) override{

    }

    void appendData(const bilibili::VideoCommentListResult& data){
        this->dataList.insert(this->dataList.end(), data.begin(), data.end());
    }

    void clearData() override{
        this->dataList.clear();
    }

private:
    bilibili::VideoCommentListResult dataList;
};

PlayerActivity::PlayerActivity(std::string bvid){
    video_data.bvid = bvid;
    Logger::debug("create VideoDetailActivity2: {}", video_data.bvid);
}

void PlayerActivity::onContentAvailable() {
    this->videoTitleLabel->registerAction("查看简介", brls::ControllerButton::BUTTON_A,
                         [this](brls::View* view)-> bool {
                            auto dialog = new brls::Dialog(this->videoIntroLabel->getFullText());
                            dialog->addButton("ok", [](){});
                            dialog->open();
                            return true;
                        });

    // 视频评论
    recyclingGrid->registerCell("Cell", []() { return VideoComment::create(); });

    // 切换视频分P
    changePEvent.subscribe([this](int index){
        brls::Logger::debug("切换分区: {}", index);
        this->requestVideoUrl(videoDetailResult.bvid, videoDetailResult.pages[index].cid);
    });

    // 切换到其他视频
    changeVideoEvent.subscribe([this](bilibili::Video videoData){
        // 停止播放视频
        this->video->stop();

        // 先重置一下tabFrame的焦点，避免空指针问题
        // 第0个tab是评论页面，这个tab固定存在，所以不会产生空指针的问题
        this->tabFrame->focusTab(0);
        // 焦点放在video上
        brls::Application::giveFocus(this->video);

        // 清空无用的tab
        this->tabFrame->clearTab("分集");
        this->tabFrame->clearTab("投稿");

        // 清空评论
        this->recyclingGrid->showSkeleton(4);

        // 请求新视频的数据
        this->requestVideoInfo(videoData.bvid);
    });

    // 切换右侧Tab
    this->registerAction("上一项", brls::ControllerButton::BUTTON_LT,
                            [this](brls::View* view)-> bool {
                                tabFrame->focus2LastTab();
                                return true;
                            }, true);

    this->registerAction("下一项", brls::ControllerButton::BUTTON_RT,
                            [this](brls::View* view)-> bool {
                                tabFrame->focus2NextTab();
                                return true;
                            }, true);


    //todo: X键 刷新播放页

    this->recyclingGrid->onNextPage([this](){
        this->requestVideoComment(this->videoDetailResult.aid);
    });


    this->requestData(this->video_data);
}

void PlayerActivity::onVideoInfo(const bilibili::VideoDetailResult &result) {
    Logger::debug("[onVideoInfo] title:{} author:{}", result.title, result.owner.name);
    this->videoUserInfo->setUserInfo(result.owner.face+"@96w_96h_1c.jpg", result.owner.name,result.owner.name);
    this->videoTitleLabel->setText(result.title);
    this->video->setTitle(result.title);
    this->videoIntroLabel->setText(result.desc);
    this->videoInfoLabel->setText("BVID: " + result.bvid);
}

void PlayerActivity::onVideoPageListInfo(const bilibili::VideoDetailPageListResult &result) {
    for(const auto& i : result){
        Logger::debug("cid:{} title:{}", i.cid, i.part);
    }

    if(result.size() <= 1){
        return;
    }

    AutoSidebarItem* item = new AutoSidebarItem();
    item->setTabStyle(AutoTabBarStyle::ACCENT);
    item->setFontSize(18);
    item->setLabel("分集");
    this->tabFrame->addTab(item, [this, result, item](){
        auto container = new AttachedView();
        container->setMarginTop(12);
        auto grid = new RecyclingGrid();
        grid->setPadding(0, 40, 0, 20);
        grid->setGrow(1);
        grid->applyXMLAttribute("spanCount", "1");
        grid->applyXMLAttribute("itemSpace", "0");
        grid->applyXMLAttribute("itemHeight", "50");
        grid->registerCell("Cell", []() { return GridRadioCell::create(); });

        vector<string> items;
        for (uint i = 0; i < result.size(); ++i) {
            auto title = result[i].part;
            items.push_back(fmt::format("PV{} {}", i+1, title));
        }
        container->addView(grid);
        grid->setDataSource(new DataSourceList(items, changePEvent));
        item->setSubtitle(wiliwili::num2w(result.size()));
        return container;
    });
}

void PlayerActivity::onUploadedVideos(const bilibili::UserUploadedVideoResultWrapper& result) {
    for(const auto& i : result.list){
        Logger::debug("up videos: bvid:{} title:{}", i.bvid, i.title);
    }

    if(result.page.pn == 1){
        // 加载第一页，添加tab
        AutoSidebarItem* item = new AutoSidebarItem();
        item->setTabStyle(AutoTabBarStyle::ACCENT);
        item->setFontSize(18);
        item->setLabel("投稿");
        this->tabFrame->addTab(item, [this, result, item](){
            auto container = new AttachedView();
            container->setMarginTop(12);
            auto grid = new RecyclingGrid();
            grid->setPadding(0, 40, 0, 20);
            grid->setGrow(1);
            grid->applyXMLAttribute("spanCount", "1");
            grid->applyXMLAttribute("itemSpace", "20");
            grid->applyXMLAttribute("itemHeight", "250");
            grid->registerCell("Cell", []() { return RecyclingGridItemVideoCard::create(); });
            grid->onNextPage([this](){
                this->requestUploadedVideos(videoDetailResult.owner.mid);
            });
            item->setSubtitle(wiliwili::num2w(result.page.count));
            container->addView(grid);
            grid->setDataSource(new DataSourceUserUploadedVideoList(result.list, changeVideoEvent));
            return container;
        });
    } else {
        // 加载第N页
        auto tab = this->tabFrame->getTab("投稿");
        if(!tab) return;
        auto view = (AttachedView*)tab->getAttachedView();
        if(!view) return;
        auto grid = (RecyclingGrid*)view->getChildren()[0];
        DataSourceUserUploadedVideoList* datasource = (DataSourceUserUploadedVideoList *)grid->getDataSource();
        datasource->appendData(result.list);
        grid->notifyDataChanged();
    }

}

void PlayerActivity::onVideoPlayUrl(const bilibili::VideoUrlResult & result) {
    Logger::debug("quality: {}", result.quality);
    // todo: 将多个文件加入播放列表
    //todo: 播放失败时可以尝试备用播放链接
    for(const auto& i: result.durl){
        this->video->start(i.url);
        break;
    }
    Logger::debug("PlayerActivity::onVideoPlayUrl done");
}

void PlayerActivity::onCommentInfo(const bilibili::VideoCommentResultWrapper &result) {
    DataSourceCommentList* datasource = (DataSourceCommentList *)recyclingGrid->getDataSource();
    if(result.cursor.prev == 1){
        // 第一页评论
        //整合置顶评论
        vector<bilibili::VideoCommentResult> comments(result.top_replies);
        comments.insert(comments.end(), result.replies.begin(), result.replies.end());
        this->recyclingGrid->setDataSource(new DataSourceCommentList(comments));
        // 设置评论数量提示
        auto item = this->tabFrame->getTab("评论");
        if(item)
            item->setSubtitle(wiliwili::num2w(result.cursor.all_count));
    } else if(datasource){
        // 第N页评论
        datasource->appendData(result.replies);
        recyclingGrid->notifyDataChanged();
    }
}

void PlayerActivity::onRequestCommentError(const std::string &error){
    brls::sync([this, error](){
        this->recyclingGrid->setError(error);
    });
}

void PlayerActivity::onError(const std::string &error){
    brls::sync([error](){
        auto dialog = new brls::Dialog(error);
        dialog->setCancelable(false);
        dialog->addButton("OK", [](){
            brls::Application::popActivity();
        });
        dialog->open();
    });
}

PlayerActivity::~PlayerActivity() {
    Logger::debug("del PlayerActivity");
    this->video->stop();
}


/// season player

void PlayerSeasonActivity::onContentAvailable(){
    this->videoTitleLabel->registerClickAction([this](brls::View* view){
        //todo：注意线程安全
        Style style = Application::getStyle();

        Label* label = new Label();
        label->setText(this->videoIntroLabel->getFullText());
        label->setFontSize(style["brls/dialog/fontSize"]);
        label->setHorizontalAlign(HorizontalAlign::CENTER);
        label->setSingleLine(false);

        Box* box = new Box();
        box->addView(label);
        box->setAlignItems(AlignItems::CENTER);
        box->setJustifyContent(JustifyContent::CENTER);
        box->setPadding(style["brls/dialog/paddingTopBottom"], style["brls/dialog/paddingLeftRight"], style["brls/dialog/paddingTopBottom"], style["brls/dialog/paddingLeftRight"]);


        auto dialog = new brls::Dialog(box);
        dialog->addButton("ok", [](){});
        dialog->open();
        return true;
    });

    // 切换右侧Tab
    this->registerAction("上一项", brls::ControllerButton::BUTTON_LT,
                         [this](brls::View* view)-> bool {
                             tabFrame->focus2LastTab();
                             return true;
                         }, true);

    this->registerAction("下一项", brls::ControllerButton::BUTTON_RT,
                         [this](brls::View* view)-> bool {
                             tabFrame->focus2NextTab();
                             return true;
                         }, true);

    recyclingGrid->registerCell("Cell", []() { return VideoComment::create(); });
    recyclingGrid->onNextPage([this](){
        if(this->episodeResult.aid != 0)
            this->requestVideoComment(this->episodeResult.aid);
    });
    changeEpisodeEvent.subscribe([this](int index){
        this->changeEpisode(seasonInfo.episodes[index]);
    });
    this->requestSeasonInfo(this->season);
}

void PlayerSeasonActivity::onSeasonEpisodeInfo(const bilibili::SeasonEpisodeResult& result){
    auto title = result.long_title;
    if(title.empty()){
        title = result.title;
    }
    this->video->setTitle(this->seasonInfo.season_title +" - "+ title);
    this->videoInfoLabel->setText("BVID: " + result.bvid);
}

void PlayerSeasonActivity::onSeasonVideoInfo(const bilibili::SeasonResultWrapper& result){
    Logger::debug("[onSeasonVideoInfo] title:{} author:{}", result.season_title, result.up_info.uname);
    //todo 修改细节内容
    this->videoUserInfo->setUserInfo(result.up_info.avatar, result.up_info.uname,result.up_info.uname);
    this->videoTitleLabel->setText(result.season_title);
    this->videoIntroLabel->setText(result.evaluate);


    AutoSidebarItem* item = new AutoSidebarItem();
    item->setTabStyle(AutoTabBarStyle::ACCENT);
    item->setFontSize(18);
    item->setLabel("分集");
    this->tabFrame->addTab(item, [this, result, item](){
        auto container = new AttachedView();
        container->setMarginTop(12);
        auto grid = new RecyclingGrid();
        grid->setPadding(0, 40, 0, 20);
        grid->setGrow(1);
        grid->applyXMLAttribute("spanCount", "1");
        grid->applyXMLAttribute("itemSpace", "0");
        grid->applyXMLAttribute("itemHeight", "50");
        grid->registerCell("Cell", []() { return GridRadioCell::create(); });

        vector<string> items;
        for (uint i = 0; i < result.episodes.size(); ++i) {
            auto title = result.episodes[i].long_title;
            if(title.empty()){
                title = result.episodes[i].title;
            }
            items.push_back(fmt::format("PV{} {}", i+1, title));
        }
        container->addView(grid);
        grid->setDataSource(new DataSourceList(items, changeEpisodeEvent));
        item->setSubtitle(wiliwili::num2w(result.episodes.size()));
        return container;
    });
}