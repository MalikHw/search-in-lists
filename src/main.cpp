#include <Geode/Geode.hpp>
#include <Geode/binding/LevelBrowserLayer.hpp>
#include <Geode/binding/GJListLayer.hpp>
#include <Geode/binding/GJSearchObject.hpp>
#include <Geode/binding/GameLevelManager.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>

using namespace geode::prelude;

class $modify(MyLevelBrowserLayer, LevelBrowserLayer) {
    struct Fields {
        TextInput* m_searchBar = nullptr;
        CCMenuItemSpriteExtra* m_searchBtn = nullptr;
        CCMenu* m_searchBarMenu = nullptr;
    };
    void onQuickSearch(CCObject*) {
        if (!m_fields->m_searchBar) return;
        // detach IME first
        m_fields->m_searchBar->getInputNode()->detachWithIME();
        auto query = m_fields->m_searchBar->getString();
        if (query.empty()) return;
        // Build a search object with SearchType::0 (search by name)
        auto searchObj = GJSearchObject::create(SearchType::Search, query);
        auto newScene = LevelBrowserLayer::scene(searchObj);
        CCDirector::get()->pushScene(CCTransitionFade::create(0.5f, newScene));
    }
    bool init(GJSearchObject* obj) {
        if (!LevelBrowserLayer::init(obj)) return false;
        auto winSize = CCDirector::get()->getWinSize();
        auto list = m_list;
        if (!list) return true;
        auto listPos = list->getPosition();
        auto listSize = list->getContentSize();
        // make a small menu that sits just above list
        auto menu = CCMenu::create();
        menu->setContentSize({ listSize.width, 28.0f });
        // position
        menu->setPosition(ccp(
            listPos.x,
            listPos.y + listSize.height + 2.0f
        ));
        menu->setAnchorPoint(ccp(0, 0));
        menu->setID("quicksearch-bar-menu");
        this->addChild(menu, 10);
        // bg bar
        auto bg = CCLayerColor::create({ 0, 0, 0, 120 }, listSize.width, 28.0f);
        bg->setPosition(ccp(0, 0));
        bg->setID("quicksearch-bg");
        menu->addChild(bg);
        // search button
        auto searchSpr = CCSprite::createWithSpriteFrameName("gj_findBtn_001.png");
        searchSpr->setScale(0.65f);
        auto searchBtn = CCMenuItemSpriteExtra::create(
            searchSpr, this,
            menu_selector(MyLevelBrowserLayer::onQuickSearch)
        );
        searchBtn->setPosition(ccp(listSize.width - 16.0f, 14.0f));
        searchBtn->setID("quicksearch-button");
        menu->addChild(searchBtn);
        m_fields->m_searchBtn = searchBtn;
        // text input
        float inputWidth = listSize.width - 40.0f;
        auto input = TextInput::create(inputWidth, "Quick search levels...");
        input->setPosition(ccp(inputWidth / 2.0f + 4.0f, 14.0f));
        input->setTextAlign(TextInputAlign::Left);
        input->getInputNode()->setLabelPlaceholderScale(0.35f);
        input->getInputNode()->setMaxLabelScale(0.35f);
        auto bgSpr = input->getBGSprite();
        bgSpr->setContentSize({ inputWidth * 2.0f, 40.0f });
        bgSpr->setScale(0.5f);
        input->setID("quicksearch-input");
        menu->addChild(input);
        m_fields->m_searchBar = input;
        m_fields->m_searchBarMenu = menu;
        return true;
    }
    void keyDown(enumKeyCodes key, double ts) {
        if (key == KEY_Enter) {
            if (m_fields->m_searchBar) {
                // the input is active, treat enter as "search"
                onQuickSearch(nullptr);
                return;
            }
        }
        LevelBrowserLayer::keyDown(key, ts);
    }
};
