﻿#include "DatePicker.h"
#include "CheckBoxScale9.h"
#include "../UICommon.h"
#include "../UIColors.h"
#include "../utils/common.h"

USING_NS_CC;

#define C4B_BLUE cocos2d::Color4B(44, 121, 178, 255)

#define LOWER_BOUND 1900
#define UPPER_BOUND 2099

static const char *weekTexts[] = { __UTF8("日"), __UTF8("一"), __UTF8("二"), __UTF8("三"), __UTF8("四"), __UTF8("五"), __UTF8("六") };

bool DatePicker::init(const Date *date, Callback &&callback) {
    if (!Layer::init()) {
        return false;
    }

    _callback.swap(callback);

    time_t now = time(nullptr);
    struct tm ret = *localtime(&now);
    _today.year = ret.tm_year + 1900;
    _today.month = ret.tm_mon + 1;
    _today.day = ret.tm_mday;
    if (date == nullptr) {
        memcpy(&_picked, &_today, sizeof(_picked));
    }
    else {
        memcpy(&_picked, date, sizeof(_picked));
    }

    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 监听返回键
    EventListenerKeyboard *keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event *event) {
        if (keyCode == EventKeyboard::KeyCode::KEY_BACK) {
            event->stopPropagation();
            onNegativeButton(nullptr);
        }
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    // 遮罩
    this->addChild(LayerColor::create(Color4B(0, 0, 0, 127)));

    // 30*7+6*2+2*5=232
    const float totalWidth = 232.0f;

    // 背景
    LayerColor *background = LayerColor::create(Color4B(255, 255, 255, 245));
    this->addChild(background);
    background->setIgnoreAnchorPointForPosition(false);
    background->setPosition(Vec2(origin.x + visibleSize.width * 0.5f, origin.y + visibleSize.height * 0.5f));

    float totalHeight = 0.0f;
    ui::Button *button = ui::Button::create("source_material/btn_square_disabled.png", "source_material/btn_square_selected.png");
    background->addChild(button);
    button->setScale9Enabled(true);
    button->setContentSize(Size(totalWidth * 0.5f, 30.0f));
    button->setTitleFontSize(14);
    button->setTitleText(__UTF8("取消"));
    button->setPosition(Vec2(totalWidth * 0.25f, 15.0f));
    button->addClickEventListener(std::bind(&DatePicker::onNegativeButton, this, std::placeholders::_1));

    button = UICommon::createButton();
    background->addChild(button);
    button->setScale9Enabled(true);
    button->setContentSize(Size(totalWidth * 0.5f, 30.0f));
    button->setTitleFontSize(14);
    button->setTitleText(__UTF8("确定"));
    button->setPosition(Vec2(totalWidth * 0.75f, 15.0f));
    button->addClickEventListener(std::bind(&DatePicker::onPositiveButton, this, std::placeholders::_1));

    totalHeight += 30.0f;

    const std::string backGround = "source_material/btn_square_normal.png";
    const std::string backGroundSelected = "source_material/btn_square_selected.png";
    const std::string cross = "source_material/btn_square_highlighted.png";
    const std::string noImage = "";

    // 30*6+5*2+15+2*5=215
    const float containerHeight = 215.0f;
    const Size containerSize = Size(totalWidth, containerHeight);

    // 日
    Node *container = Node::create();
    container->setContentSize(containerSize);

    for (int i = 0; i < 7; ++i) {
        Label *label = Label::createWithSystemFont(weekTexts[i], "Arial", 12);
        label->setTextColor(C4B_GRAY);
        container->addChild(label);
        label->setPosition(Vec2(20.0f + i * 32.0f, containerHeight - 10.0f));
    }

    for (int i = 0; i < 42; ++i) {
        div_t ret = div(i, 7);
        CheckBoxScale9 *checkBox = CheckBoxScale9::create();
        checkBox->loadTextures(backGround, backGroundSelected, cross, noImage, noImage);
        checkBox->setZoomScale(0.0f);
        checkBox->ignoreContentAdaptWithSize(false);
        checkBox->setContentSize(Size(30.0f, 30.0f));
        checkBox->setTag(i);
        checkBox->addEventListener(std::bind(&DatePicker::onDayBox, this, std::placeholders::_1, std::placeholders::_2));
        container->addChild(checkBox);
        checkBox->setPosition(Vec2(20.0f + ret.rem * 32.0f, containerHeight - 35.0f - ret.quot * 32.0f));
        _dayBoxes[i] = checkBox;

        Label *label = Label::createWithSystemFont("", "Arial", 12);
        label->setTextColor(C4B_GRAY);
        checkBox->addChild(label);
        label->setPosition(Vec2(15.0f, 15.0f));
        checkBox->setUserData(label);
    }

    background->addChild(container);
    container->setIgnoreAnchorPointForPosition(false);
    container->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    container->setPosition(Vec2(totalWidth * 0.5f, 35.0f));
    _daysContainer = container;

    // 月
    container = Node::create();
    container->setContentSize(containerSize);

    static const char *monthTexts[] = {
        __UTF8("1月"), __UTF8("2月"), __UTF8("3月"), __UTF8("4月"), __UTF8("5月"), __UTF8("6月"),
        __UTF8("7月"), __UTF8("8月"), __UTF8("9月"), __UTF8("10月"), __UTF8("11月"), __UTF8("12月")
    };
    for (int i = 0; i < 12; ++i) {
        div_t ret = div(i, 4);
        CheckBoxScale9 *checkBox = CheckBoxScale9::create();
        checkBox->loadTextures(backGround, backGroundSelected, cross, noImage, noImage);
        checkBox->setZoomScale(0.0f);
        checkBox->ignoreContentAdaptWithSize(false);
        checkBox->setContentSize(Size(54.0f, 54.0f));
        checkBox->setTag(i);
        checkBox->addEventListener(std::bind(&DatePicker::onMonthBox, this, std::placeholders::_1, std::placeholders::_2));
        container->addChild(checkBox);
        checkBox->setPosition(Vec2(32.0f + ret.rem * 56.0f, containerHeight - 32.0f - ret.quot * 56.0f));
        _monthBoxes[i] = checkBox;

        Label *label = Label::createWithSystemFont(monthTexts[i], "Arial", 12);
        label->setTextColor(C4B_GRAY);
        checkBox->addChild(label);
        label->setPosition(Vec2(27.0f, 27.0f));
        checkBox->setUserData(label);
    }

    background->addChild(container);
    container->setIgnoreAnchorPointForPosition(false);
    container->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    container->setPosition(Vec2(totalWidth * 0.5f, 35.0f));
    _monthsContainer = container;

    // 年
    container = Node::create();
    container->setContentSize(containerSize);

    for (int i = 0; i < 10; ++i) {
        div_t ret = div(i, 4);
        CheckBoxScale9 *checkBox = CheckBoxScale9::create();
        checkBox->loadTextures(backGround, backGroundSelected, cross, noImage, noImage);
        checkBox->setZoomScale(0.0f);
        checkBox->ignoreContentAdaptWithSize(false);
        checkBox->setContentSize(Size(54.0f, 54.0f));
        checkBox->setTag(i);
        checkBox->addEventListener(std::bind(&DatePicker::onYearBox, this, std::placeholders::_1, std::placeholders::_2));
        container->addChild(checkBox);
        checkBox->setPosition(Vec2(32.0f + ret.rem * 56.0f, containerHeight - 32.0f - ret.quot * 56.0f));
        _yearBoxes[i] = checkBox;

        Label *label = Label::createWithSystemFont("", "Arial", 12);
        label->setTextColor(C4B_GRAY);
        checkBox->addChild(label);
        label->setPosition(Vec2(27.0f, 27.0f));
        checkBox->setUserData(label);
    }

    background->addChild(container);
    container->setIgnoreAnchorPointForPosition(false);
    container->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    container->setPosition(Vec2(totalWidth * 0.5f, 35.0f));
    _yearsContainer = container;

    totalHeight += containerSize.height + 10.0f;

    // 用来切换的按钮
    button = ui::Button::create("source_material/btn_square_normal.png", "source_material/btn_square_selected.png");
    button->setScale9Enabled(true);
    button->setContentSize(Size(80.0f, 20.0f));
    button->setTitleColor(C3B_GRAY);
    button->setTitleFontSize(12);
    button->addClickEventListener(std::bind(&DatePicker::onSwitchButton, this, std::placeholders::_1));
    background->addChild(button);
    button->setPosition(Vec2(45.0f, totalHeight + 15.0f));
    _switchButton = button;

    // 上下按钮
    button = ui::Button::create("icon/left-circle.png");
    background->addChild(button);
    button->setScale(20 / button->getContentSize().width);
    button->setPosition(Vec2(totalWidth - 52.0f, totalHeight + 15.0f));
    button->setColor(Color3B(51, 204, 255));
    button->setRotation(90.0f);
    button->addClickEventListener(std::bind(&DatePicker::onUpButton, this, std::placeholders::_1));

    button = ui::Button::create("icon/left-circle.png");
    background->addChild(button);
    button->setScale(20 / button->getContentSize().width);
    button->setPosition(Vec2(totalWidth - 20.0f, totalHeight + 15.0f));
    button->setColor(Color3B(51, 204, 255));
    button->setRotation(-90.0f);
    button->addClickEventListener(std::bind(&DatePicker::onDownButton, this, std::placeholders::_1));

    totalHeight += 25.0f;

    // 上方背景及所选日期显示
    LayerColor *bg2 = LayerColor::create(C4B_BLUE_THEME, totalWidth, 30.0f);
    background->addChild(bg2);
    bg2->setPosition(Vec2(0.0f, totalHeight + 5.0f));

    Label *label = Label::createWithSystemFont("", "Arial", 16);
    background->addChild(label);
    label->setPosition(Vec2(totalWidth * 0.5f, totalHeight + 20.0f));
    _resultLabel = label;
    _resultLabel = label;
    totalHeight += 35.0f;

    background->setContentSize(Size(totalWidth, totalHeight));
    if (totalWidth > visibleSize.width * 0.8f || totalHeight > visibleSize.height * 0.8f) {
        const float scale = std::min(visibleSize.width * 0.8f / totalWidth, visibleSize.height * 0.8f / totalHeight);
        background->setScale(scale);
    }

    // 触摸监听
    EventListenerTouchOneByOne *touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = [this](Touch *touch, Event *event) {
        return true;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    _touchListener = touchListener;

    setupDayContainer();

    return true;
}

static bool IsLeapYear(int year) {
    return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);
}

static int DaysForMonth(int year, int month) {
    static int daysForMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    int days = daysForMonth[month];
    if (month == 2 && IsLeapYear(year)) {
        ++days;
    }
    return days;
}

static int CaculateWeekDay(int y, int m, int d) {
    // 基姆拉尔森计算公式
    if (m == 1 || m == 2) {
        m += 12;
        --y;
    }
    return (1 + d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400) % 7;
}

static void adjustDate(DatePicker::Date *date) {
    int maxDays = DaysForMonth(date->year, date->month);
    date->day = std::min(date->day, maxDays);
}

void DatePicker::refreshResultLabel() {
    _resultLabel->setString(Common::format(__UTF8("%d年%d月%d日 星期%s"),
        _picked.year, _picked.month, _picked.day, weekTexts[CaculateWeekDay(_picked.year, _picked.month, _picked.day)]));
}

void DatePicker::setupDayContainer() {
    adjustDate(&_picked);
    refreshResultLabel();

    _state = PICK_STATE::DAY;
    _daysContainer->setVisible(true);
    _monthsContainer->setVisible(false);
    _yearsContainer->setVisible(false);
    _switchButton->setTitleText(Common::format(__UTF8("%d年%d月"), _picked.year, _picked.month));

    // 本月的1号是星期几，那么前面的天数属于上个月
    int weekday = CaculateWeekDay(_picked.year, _picked.month, 1);
    for (int i = 0; i < weekday; ++i) {
        _dayBoxes[i]->setVisible(false);
    }
    _dayOffset = weekday;

    // 本月的天数
    int days = DaysForMonth(_picked.year, _picked.month);
    char str[32];
    for (int i = 0; i < days; ++i) {
        ui::CheckBox *checkBox = _dayBoxes[weekday + i];
        checkBox->setVisible(true);
        checkBox->setSelected(false);

        snprintf(str, sizeof(str), "%d", i + 1);
        Label *label = (Label *)checkBox->getUserData();
        label->setString(str);
        label->setTextColor(C4B_GRAY);
    }

    // 超出的属于下个月
    for (int i = weekday + days; i < 42; ++i) {
        _dayBoxes[i]->setVisible(false);
    }

    // 高亮今天
    if (_picked.year == _today.year && _picked.month == _today.month) {
        ((Label *)_dayBoxes[weekday + _today.day - 1]->getUserData())->setTextColor(C4B_BLUE);
    }
    _dayBoxes[_dayOffset + _picked.day - 1]->setSelected(true);
}

void DatePicker::setupMonthContainer() {
    adjustDate(&_picked);
    refreshResultLabel();

    _state = PICK_STATE::MONTH;
    _daysContainer->setVisible(false);
    _monthsContainer->setVisible(true);
    _yearsContainer->setVisible(false);
    _switchButton->setTitleText(Common::format(__UTF8("%d年"), _picked.year));

    if (_picked.year == _today.year) {
        ((Label *)_monthBoxes[_today.month - 1]->getUserData())->setTextColor(C4B_BLUE);
    }
    else {
        ((Label *)_monthBoxes[_today.month - 1]->getUserData())->setTextColor(C4B_GRAY);
    }

    for (int i = 0; i < 12; ++i) {
        _monthBoxes[i]->setSelected(_picked.month - 1 == i);
    }
}

void DatePicker::setupYearContainer() {
    _state = PICK_STATE::YEAR;
    _daysContainer->setVisible(false);
    _monthsContainer->setVisible(false);
    _yearsContainer->setVisible(true);

    _yearBoxes[_picked.year % 10]->setSelected(false);

    char str[64];

    snprintf(str, sizeof(str), "%d - %d", _decadeStart, _decadeStart + 9);
    _switchButton->setTitleText(str);

    for (int i = 0; i < 10; ++i) {
        ui::CheckBox *checkBox = _yearBoxes[i];
        Label *label = (Label *)checkBox->getUserData();
        int yy = _decadeStart + i;
        snprintf(str, sizeof(str), "%d", yy);
        label->setString(str);

        if (yy == _today.year) {
            label->setTextColor(C4B_BLUE);
        }
        else {
            label->setTextColor(C4B_GRAY);
        }

        checkBox->setSelected(yy == _picked.year);
    }
}

void DatePicker::onSwitchButton(cocos2d::Ref *) {
    switch (_state) {
    case PICK_STATE::DAY:
        setupMonthContainer();
        _state = PICK_STATE::MONTH;
        break;
    case PICK_STATE::MONTH:
        _monthsContainer->setVisible(false);
        _yearsContainer->setVisible(true);
        _decadeStart = _picked.year / 10 * 10;
        setupYearContainer();
        _state = PICK_STATE::YEAR;
        break;
    case PICK_STATE::YEAR:
        break;
    default:
        break;
    }
}

void DatePicker::onUpButton(cocos2d::Ref *) {
    switch (_state) {
    case PICK_STATE::DAY:
        if (_picked.month > 1) {
            --_picked.month;
            setupDayContainer();
        }
        else if (_picked.year > LOWER_BOUND) {
            --_picked.year;
            _picked.month = 12;
            setupDayContainer();
        }
        break;
    case PICK_STATE::MONTH:
        if (_picked.year > LOWER_BOUND) {
            --_picked.year;
            setupMonthContainer();
        }
        break;
    case PICK_STATE::YEAR:
        if (_decadeStart - 10 >= LOWER_BOUND) {
            _decadeStart -= 10;
            setupYearContainer();
        }
        break;
    default:
        break;
    }
}

void DatePicker::onDownButton(cocos2d::Ref *) {
    switch (_state) {
    case PICK_STATE::DAY:
        if (_picked.month < 12) {
            ++_picked.month;
            setupDayContainer();
        }
        else if (_picked.year < UPPER_BOUND) {
            ++_picked.year;
            _picked.month = 1;
            setupDayContainer();
        }
        break;
    case PICK_STATE::MONTH:
        if (_picked.year < UPPER_BOUND) {
            ++_picked.year;
            setupMonthContainer();
        }
        break;
    case PICK_STATE::YEAR:
        if (_decadeStart + 10 <= UPPER_BOUND) {
            _decadeStart += 10;
            setupYearContainer();
        }
        break;
    default:
        break;
    }
}

void DatePicker::onDayBox(cocos2d::Ref *sender, cocos2d::ui::CheckBox::EventType event) {
    ui::CheckBox *checkBox = (ui::CheckBox *)sender;
    if (event == ui::CheckBox::EventType::UNSELECTED) {
        checkBox->setSelected(true);
    }
    else {
        int day = checkBox->getTag() + 1 - _dayOffset;
        if (day != _picked.day) {
            _dayBoxes[_dayOffset + _picked.day - 1]->setSelected(false);
            _picked.day = day;
            refreshResultLabel();
        }
    }
}

void DatePicker::onMonthBox(cocos2d::Ref *sender, cocos2d::ui::CheckBox::EventType event) {
    ui::CheckBox *checkBox = (ui::CheckBox *)sender;
    if (event == ui::CheckBox::EventType::UNSELECTED) {
        checkBox->setSelected(true);
    }
    else {
        int month = checkBox->getTag() + 1;
        _monthBoxes[_picked.month - 1]->setSelected(false);
        _picked.month = month;
    }
    setupDayContainer();
}

void DatePicker::onYearBox(cocos2d::Ref *sender, cocos2d::ui::CheckBox::EventType event) {
    ui::CheckBox *checkBox = (ui::CheckBox *)sender;
    if (event == ui::CheckBox::EventType::UNSELECTED) {
        checkBox->setSelected(true);
    }
    else {
        int year = checkBox->getTag() + _decadeStart;
        _yearBoxes[_picked.year % 10]->setSelected(false);
        _picked.year = year;
    }
    setupMonthContainer();
}

void DatePicker::onPositiveButton(cocos2d::Ref *) {
    this->retain();
    _callback(this, true);
    this->release();
    this->dismiss();
}

void DatePicker::onNegativeButton(cocos2d::Ref *) {
    this->retain();
    _callback(this, false);
    this->release();
    this->dismiss();
}