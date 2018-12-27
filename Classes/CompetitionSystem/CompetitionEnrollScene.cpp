﻿#include "CompetitionEnrollScene.h"
#include <array>
#include "../UICommon.h"
#include "../widget/AlertDialog.h"
#include "../widget/Toast.h"
#include "Competition.h"
#include "CompetitionRoundScene.h"

USING_NS_CC;

bool CompetitionEnrollScene::init(const std::shared_ptr<CompetitionData> &competitionData) {
    if (UNLIKELY(!BaseScene::initWithTitle(std::string(competitionData->name).append(__UTF8("报名表"))))) {
        return false;
    }

    _competitionData = competitionData;

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 表头
    Label *label = Label::createWithSystemFont(__UTF8("序号"), "Arail", 12);
    label->setColor(Color3B::BLACK);
    this->addChild(label);
    label->setPosition(Vec2(origin.x + 20.0f, origin.y + visibleSize.height - 50.0f));

    label = Label::createWithSystemFont(__UTF8("选手姓名"), "Arail", 12);
    label->setColor(Color3B::BLACK);
    this->addChild(label);
    label->setPosition(Vec2(origin.x + visibleSize.width * 0.25f + 15.0f, origin.y + visibleSize.height - 50.0f));

    label = Label::createWithSystemFont(__UTF8("序号"), "Arail", 12);
    label->setColor(Color3B::BLACK);
    this->addChild(label);
    label->setPosition(Vec2(origin.x + visibleSize.width * 0.5f + 20.0f, origin.y + visibleSize.height - 50.0f));

    label = Label::createWithSystemFont(__UTF8("选手姓名"), "Arail", 12);
    label->setColor(Color3B::BLACK);
    this->addChild(label);
    label->setPosition(Vec2(origin.x + visibleSize.width * 0.75f + 15.0f, origin.y + visibleSize.height - 50.0f));

    // 表格
    cw::TableView *tableView = cw::TableView::create();
    tableView->setContentSize(Size(visibleSize.width, visibleSize.height - 95.0f));
    tableView->setDelegate(this);
    tableView->setDirection(ui::ScrollView::Direction::VERTICAL);
    tableView->setVerticalFillOrder(cw::TableView::VerticalFillOrder::TOP_DOWN);

    tableView->setScrollBarPositionFromCorner(Vec2(5.0f, 2.0f));
    tableView->setScrollBarWidth(4.0f);
    tableView->setScrollBarOpacity(0x99);
    tableView->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    tableView->setPosition(Vec2(origin.x + visibleSize.width * 0.5f, origin.y + visibleSize.height * 0.5f - 15.0f));
    tableView->reloadData();
    this->addChild(tableView);
    _tableView = tableView;

    // 提交按钮
    ui::Button *button = UICommon::createButton();
    this->addChild(button);
    button->setScale9Enabled(true);
    button->setContentSize(Size(55.0f, 20.0f));
    button->setTitleFontSize(12);
    button->setTitleText(__UTF8("提交"));
    button->setPosition(Vec2(origin.x + visibleSize.width * 0.5f, origin.y + 15.0f));
    button->addClickEventListener(std::bind(&CompetitionEnrollScene::onSubmitButton, this, std::placeholders::_1));

    return true;
}

ssize_t CompetitionEnrollScene::numberOfCellsInTableView(cw::TableView *) {
    return _competitionData->players.size() >> 1;
}

float CompetitionEnrollScene::tableCellSizeForIndex(cw::TableView *, ssize_t) {
    return 30.0f;
}

cw::TableViewCell *CompetitionEnrollScene::tableCellAtIndex(cw::TableView *table, ssize_t idx) {
    typedef cw::TableViewCellEx<std::array<Label *, 4>, std::array<ui::Widget *, 2>, std::array<LayerColor *, 2> > CustomCell;
    CustomCell *cell = (CustomCell *)table->dequeueCell();

    Size visibleSize = Director::getInstance()->getVisibleSize();

    if (cell == nullptr) {
        cell = CustomCell::create();

        CustomCell::ExtDataType &ext = cell->getExtData();
        Label **labels = std::get<0>(ext).data();
        ui::Widget **widgets = std::get<1>(ext).data();
        LayerColor **layerColors = std::get<2>(ext).data();

        layerColors[0] = LayerColor::create(Color4B(0x10, 0x10, 0x10, 0x10), visibleSize.width, 30.0f);
        cell->addChild(layerColors[0]);

        layerColors[1] = LayerColor::create(Color4B(0xC0, 0xC0, 0xC0, 0x10), visibleSize.width, 30.0f);
        cell->addChild(layerColors[1]);

        Label *label = Label::createWithSystemFont("", "Arail", 12);
        label->setColor(Color3B::BLACK);
        cell->addChild(label);
        label->setPosition(Vec2(20.0f, 15.0f));
        labels[0] = label;

        label = Label::createWithSystemFont("", "Arail", 12);
        label->setColor(Color3B::BLACK);
        cell->addChild(label);
        label->setPosition(Vec2(visibleSize.width * 0.25f + 15.0f, 15.0f));
        labels[1] = label;

        label = Label::createWithSystemFont("", "Arail", 12);
        label->setColor(Color3B::BLACK);
        cell->addChild(label);
        label->setPosition(Vec2(visibleSize.width * 0.5f + 20.0f, 15.0f));
        labels[2] = label;

        label = Label::createWithSystemFont("", "Arail", 12);
        label->setColor(Color3B::BLACK);
        cell->addChild(label);
        label->setPosition(Vec2(visibleSize.width * 0.75f + 15.0f, 15.0f));
        labels[3] = label;

        ui::Widget *widget = ui::Widget::create();
        widget->setTouchEnabled(true);
        widget->setPosition(labels[1]->getPosition());
        widget->setContentSize(Size(visibleSize.width * 0.5f - 50.0f, 25.0f));
        cell->addChild(widget);
        widget->addClickEventListener(std::bind(&CompetitionEnrollScene::onNameWidget, this, std::placeholders::_1));
        widgets[0] = widget;

        widget = ui::Widget::create();
        widget->setTouchEnabled(true);
        widget->setPosition(labels[3]->getPosition());
        widget->setContentSize(Size(visibleSize.width * 0.5f - 50.0f, 25.0f));
        cell->addChild(widget);
        widget->addClickEventListener(std::bind(&CompetitionEnrollScene::onNameWidget, this, std::placeholders::_1));
        widgets[1] = widget;
    }

    const CustomCell::ExtDataType &ext = cell->getExtData();
    Label *const *labels = std::get<0>(ext).data();
    ui::Widget *const *widgets = std::get<1>(ext).data();
    LayerColor *const *layerColors = std::get<2>(ext).data();

    layerColors[0]->setVisible((idx & 1) == 0);
    layerColors[1]->setVisible((idx & 1) != 0);

    ssize_t idx0 = idx << 1;
    ssize_t idx1 = idx0 | 1;
    labels[0]->setString(std::to_string(idx0 + 1));
    widgets[0]->setUserData(reinterpret_cast<void *>(idx0));
    labels[2]->setString(std::to_string(idx1 + 1));
    widgets[1]->setUserData(reinterpret_cast<void *>(idx1));

    const std::string &name0 = _competitionData->players[idx0].name;
    const std::string &name1 = _competitionData->players[idx1].name;

    if (name0.empty()) {
        labels[1]->setColor(Color3B::GRAY);
        labels[1]->setString(__UTF8("选手姓名"));
    }
    else {
        labels[1]->setColor(Color3B::ORANGE);
        labels[1]->setString(name0);
    }
    cw::scaleLabelToFitWidth(labels[1], visibleSize.width * 0.5f - 50.0f);

    if (name1.empty()) {
        labels[3]->setColor(Color3B::GRAY);
        labels[3]->setString(__UTF8("选手姓名"));
    }
    else {
        labels[3]->setColor(Color3B::ORANGE);
        labels[3]->setString(name1);
    }
    cw::scaleLabelToFitWidth(labels[3], visibleSize.width * 0.5f - 50.0f);

    return cell;
}

void CompetitionEnrollScene::onSubmitButton(cocos2d::Ref *) {
#if 1  // 测试代码
    for (size_t i = 0, cnt = _competitionData->players.size(); i < cnt; ++i) {
        std::string &name = _competitionData->players.at(i).name;
        if (name.empty()) name = Common::format(__UTF8("第%") __UTF8(PRIzu) __UTF8("号参赛选手"), i + 1);
    }
#endif

    if (!_competitionData->isEnrollmentOver()) {
        Toast::makeText(this, __UTF8("请录入所有选手姓名"), Toast::LENGTH_LONG)->show();
        return;
    }

    _competitionData->startNewRound();
    _competitionData->writeToFile();
    Director::getInstance()->replaceScene(CompetitionRoundScene::create(_competitionData, 0));
}

void CompetitionEnrollScene::onNameWidget(cocos2d::Ref *sender) {
    ui::Widget *widget = (ui::Widget *)sender;
    size_t idx = reinterpret_cast<size_t>(widget->getUserData());

    ui::EditBox *editBox = UICommon::createEditBox(Size(120.0f, 20.0f));
    editBox->setInputMode(ui::EditBox::InputMode::SINGLE_LINE);
    editBox->setInputFlag(ui::EditBox::InputFlag::SENSITIVE);
    editBox->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
    editBox->setFontColor(Color3B::BLACK);
    editBox->setFontSize(12);
    editBox->setText(_competitionData->players[idx].name.c_str());
    editBox->setMaxLength(64);
    editBox->setPlaceholderFontColor(Color4B::GRAY);
    editBox->setPlaceHolder(__UTF8("输入选手姓名"));

    AlertDialog::Builder(this)
        .setTitle(Common::format(__UTF8("序号「%") __UTF8(PRIzu) __UTF8("」"), idx + 1))
        .setContentNode(editBox)
        .setNegativeButton(__UTF8("取消"), nullptr)
        .setPositiveButton(__UTF8("确定"), [this, editBox, idx](AlertDialog *, int) {
        _competitionData->players[idx].name = editBox->getText();
        _competitionData->writeToFile();
        _tableView->updateCellAtIndex(idx >> 1);
        return true;
    }).create()->show();

    auto editBoxStrong = makeRef(editBox);
    Director::getInstance()->getScheduler()->schedule([editBoxStrong](float) {
        editBoxStrong->openKeyboard();
    }, this, 0.0f, 0, 0.0f, false, "open_keyboard");
}
