//
// Created by fang on 2022/8/3.
//

// register this fragment in main.cpp
//#include "fragment/search_cinema.hpp"
//    brls::Application::registerXMLView("SearchCinema", SearchCinema::create);
// <brls:View xml=@res/xml/fragment/search_cinema.xml

#pragma once

#include <atomic>
#include <memory>
#include <borealis.hpp>
#include "view/recycling_grid.hpp"

class SearchCinema : public brls::Box {

public:
    SearchCinema();

    ~SearchCinema();

    static View *create();

    void requestSearch(const std::string& key);

    void _requestSearch(const std::string& key);

private:
    BRLS_BIND(RecyclingGrid, recyclingGrid, "search/cinema/recyclingGrid");

    uint requestIndex = 1;

};