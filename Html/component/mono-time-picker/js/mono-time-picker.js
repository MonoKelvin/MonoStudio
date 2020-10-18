'user strict';

$.fn.MonoTimePicker = function (cfg) {
    this.hour = cfg && cfg.hour !== undefined ? Boolean(cfg.hour) : true;
    this.minute = cfg && cfg.minute !== undefined ? Boolean(cfg.minute) : true;
    this.second = cfg && cfg.second !== undefined ? Boolean(cfg.second) : true;
    this.initTime = cfg && cfg.initTime ? cfg.initTime : new Date();
    this.popInSitu = cfg && cfg.popInSitu !== undefined ? Boolean(cfg.popInSitu) : false;
    this.showUnit = cfg && cfg.showUnit !== undefined ? Boolean(cfg.showUnit) : false;
    this.onPicked = cfg && cfg.onPicked ? cfg.onPicked : null;
    this.onCancel = cfg && cfg.onCancel ? cfg.onCancel : null;

    this.hourValue = 0;
    this.minuteValue = 0;
    this.secondValue = 0;

    var t = $(this);

    var selectionGenerator = () => {
        let h = '';

        if (this.initTime && typeof this.initTime === 'string') {
            const strTime = this.initTime.split(':');
            this.initTime = new Date();
            this.initTime.setHours(strTime[0]);
            this.initTime.setMinutes(strTime[1]);
            this.initTime.setSeconds(strTime[2]);
        }

        if (this.hour) {
            this.hourValue = this.initTime.getHours();
            h += `<span id="sel-hour">${this.hourValue}`;
            if (this.showUnit) {
                h += ' 时';
            }
            h += '</span>';
        }
        if (this.minute) {
            this.minuteValue = this.initTime.getMinutes();
            h += `<span id="sel-minute">${this.minuteValue}`;
            if (this.showUnit) {
                h += ' 分';
            }
            h += '</span>';
        }
        if (this.second) {
            this.secondValue = this.initTime.getSeconds();
            h += `<span id="sel-second">${this.secondValue}`;
            if (this.showUnit) {
                h += ' 秒';
            }
            h += '</span>';
        }
        return h;
    };

    var headerGenerator = () => {
        let h = '';
        if (this.hour) {
            h += '<span>时</span>';
        }
        if (this.minute) {
            h += '<span>分</span>';
        }
        if (this.second) {
            h += '<span>秒</span>';
        }
        return h;
    };

    var timeHtmlGenerator = (count) => {
        let text = '';
        for (let i = 0; i < count; i++) {
            text += '<li>' + i + '</li>';
        }
        return text;
    };

    /**
     * 选择时间
     * @param {Element} item 要选择的时间
     */
    var selectTime = (item) => {
        let it = $(item);
        it.siblings().removeClass('mui-tp-selected-item');
        it.addClass('mui-tp-selected-item');

        let ip = $(item.parentNode);
        if (ip.hasClass('mui-tp-hour')) {
            this.hourValue = item.innerText;
        } else if (ip.hasClass('mui-tp-minute')) {
            this.minuteValue = item.innerText;
        } else if (ip.hasClass('mui-tp-second')) {
            this.secondValue = item.innerText;
        }
    };

    /**
     * 选择并滚动到对应的时间
     * @param {string} itemName 项目的名称
     * @param {string | number} value 验证的值，当value==item.innerText时才选中
     */
    var selectAndScrollTo = (listName, value) => {
        t.find('.' + listName)
            .children()
            .each(function () {
                if (value == this.innerText) {
                    $(this.parentNode).animate({ scrollTop: this.offsetTop }, 'fast');
                    selectTime(this);
                    return false;
                }
            });
    };

    /** 创建元素 */
    let html = `
        <div class="mui-tp-selection">
            ${selectionGenerator()}
        </div>
        <div class="mui-tp-container display-none">
            <div class="mui-tp-header">
                ${headerGenerator()}
            </div>
            <div class="mui-tp-time-panel">
                <ul class="mui-tp-time-list mui-tp-hour">
                    ${timeHtmlGenerator(24)}
                </ul>
                <ul class="mui-tp-time-list mui-tp-minute">
                    ${timeHtmlGenerator(60)}
                </ul>
                <ul class="mui-tp-time-list mui-tp-second">
                    ${timeHtmlGenerator(60)}
                </ul>
            </div>
            <div class="mui-tp-footer">
                <button id="sure" class="btn-primary">确认</button>
                <button id="cancel">取消</button>
            </div>
        </div>`;
    t.html(html);

    if (!this.hour) {
        $('.mui-tp-hour').remove();
    }
    if (!this.minute) {
        $('.mui-tp-minute').remove();
    }
    if (!this.second) {
        $('.mui-tp-second').remove();
    }
    let s = $('.mui-tp-selection').children();
    if (s.length <= 1) {
        s.addClass('no-border');
    }

    var container = t.find('.mui-tp-container');
    var timeList = container.find('.mui-tp-time-list');

    // 点击时间文本时
    timeList.children().click(function () {
        selectTime(this);
    });

    // 当打开时间选择器时
    t.find('.mui-tp-selection').click(() => {
        container.removeClass('display-none');

        if (!this.popInSitu) {
            container.css('top', t.innerHeight() + 2 + 'px');
        }

        if (this.hour) {
            selectAndScrollTo('mui-tp-hour', this.hourValue);
        }
        if (this.minute) {
            selectAndScrollTo('mui-tp-minute', this.minuteValue);
        }
        if (this.second) {
            selectAndScrollTo('mui-tp-second', this.secondValue);
        }

        container.mousedown(() => false);
        $(document).bind('mousedown', () => {
            container.addClass('display-none');
            $(document).unbind('mousedown');
        });
    });

    // 确定
    t.find('#sure').click(() => {
        if (this.hour) {
            $('.mui-tp-time-picker #sel-hour').text(this.hourValue + (this.showUnit ? ' 时' : ''));
        }
        if (this.minute) {
            $('.mui-tp-time-picker #sel-minute').text(this.minuteValue + (this.showUnit ? ' 分' : ''));
        }
        if (this.second) {
            $('.mui-tp-time-picker #sel-second').text(this.secondValue + (this.showUnit ? ' 秒' : ''));
        }
        container.addClass('display-none');
        if (this.onPicked) this.onPicked();
    });

    // 取消
    t.find('#cancel').click(() => {
        container.addClass('display-none');
        if (this.onCancel) this.onCancel();
    });

    return this;
};
