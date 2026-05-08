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

        if (m_searchObject->isLevelSearchObject()) {
            // Online search: update the query on the existing search object and trigger onSearch
            m_searchObject->m_searchQuery = query;
            m_searchObject->m_page = 0;
            this->onSearch(nullptr);
        } else {
            // Local/saved/editor search: reload in-place with same type, new query
            auto searchObj = GJSearchObject::create(m_searchObject->m_searchType, query);
            this->loadPage(searchObj);
        }
    }
    bool init(GJSearchObject* obj) {
        if (!LevelBrowserLayer::init(obj)) return false;
        auto list = m_list;
        if (!list) return true;

        // GJListLayer has anchor (0.5, 0), so left edge = pos.x - width/2
        float lx = list->getPositionX() - list->getContentSize().width / 2.0f;
        float ly = list->getPositionY() + list->getContentSize().height;
        float lw = list->getContentSize().width;
        float barHeight = 28.0f;

        // make a small menu that sits just above the list top border
        auto menu = CCMenu::create();
        menu->setContentSize({ lw, barHeight });
        menu->setPosition(ccp(lx, ly + 2.0f));
        menu->setAnchorPoint(ccp(0, 0));
        menu->setID("quicksearch-bar-menu");
        this->addChild(menu, 10);

        // bg bar
        auto bg = CCLayerColor::create({ 0, 0, 0, 120 }, lw, barHeight);
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
        searchBtn->setPosition(ccp(lw - 16.0f, barHeight / 2.0f));
        searchBtn->setID("quicksearch-button");
        menu->addChild(searchBtn);
        m_fields->m_searchBtn = searchBtn;

        // text input
        float inputWidth = lw - 40.0f;
        auto input = TextInput::create(inputWidth, "Quick search levels...");
        input->setPosition(ccp(inputWidth / 2.0f + 4.0f, barHeight / 2.0f));
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
