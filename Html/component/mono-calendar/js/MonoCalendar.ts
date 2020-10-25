
class MonoCalendar {
    static lunarInfo = new Array(
        0x04bd8,
        0x04ae0,
        0x0a570,
        0x054d5,
        0x0d260,
        0x0d950,
        0x16554,
        0x056a0,
        0x09ad0,
        0x055d2,
        0x04ae0,
        0x0a5b6,
        0x0a4d0,
        0x0d250,
        0x1d255,
        0x0b540,
        0x0d6a0,
        0x0ada2,
        0x095b0,
        0x14977,
        0x04970,
        0x0a4b0,
        0x0b4b5,
        0x06a50,
        0x06d40,
        0x1ab54,
        0x02b60,
        0x09570,
        0x052f2,
        0x04970,
        0x06566,
        0x0d4a0,
        0x0ea50,
        0x06e95,
        0x05ad0,
        0x02b60,
        0x186e3,
        0x092e0,
        0x1c8d7,
        0x0c950,
        0x0d4a0,
        0x1d8a6,
        0x0b550,
        0x056a0,
        0x1a5b4,
        0x025d0,
        0x092d0,
        0x0d2b2,
        0x0a950,
        0x0b557,
        0x06ca0,
        0x0b550,
        0x15355,
        0x04da0,
        0x0a5d0,
        0x14573,
        0x052d0,
        0x0a9a8,
        0x0e950,
        0x06aa0,
        0x0aea6,
        0x0ab50,
        0x04b60,
        0x0aae4,
        0x0a570,
        0x05260,
        0x0f263,
        0x0d950,
        0x05b57,
        0x056a0,
        0x096d0,
        0x04dd5,
        0x04ad0,
        0x0a4d0,
        0x0d4d4,
        0x0d250,
        0x0d558,
        0x0b540,
        0x0b5a0,
        0x195a6,
        0x095b0,
        0x049b0,
        0x0a974,
        0x0a4b0,
        0x0b27a,
        0x06a50,
        0x06d40,
        0x0af46,
        0x0ab60,
        0x09570,
        0x04af5,
        0x04970,
        0x064b0,
        0x074a3,
        0x0ea50,
        0x06b58,
        0x055c0,
        0x0ab60,
        0x096d5,
        0x092e0,
        0x0c960,
        0x0d954,
        0x0d4a0,
        0x0da50,
        0x07552,
        0x056a0,
        0x0abb7,
        0x025d0,
        0x092d0,
        0x0cab5,
        0x0a950,
        0x0b4a0,
        0x0baa4,
        0x0ad50,
        0x055d9,
        0x04ba0,
        0x0a5b0,
        0x15176,
        0x052b0,
        0x0a930,
        0x07954,
        0x06aa0,
        0x0ad50,
        0x05b52,
        0x04b60,
        0x0a6e6,
        0x0a4e0,
        0x0d260,
        0x0ea65,
        0x0d530,
        0x05aa0,
        0x076a3,
        0x096d0,
        0x04bd7,
        0x04ad0,
        0x0a4d0,
        0x1d0b6,
        0x0d250,
        0x0d520,
        0x0dd45,
        0x0b5a0,
        0x056d0,
        0x055b2,
        0x049b0,
        0x0a577,
        0x0a4b0,
        0x0aa50,
        0x1b255,
        0x06d20,
        0x0ada0
    );
    static Animals = new Array('鼠', '牛', '虎', '兔', '龙', '蛇', '马', '羊', '猴', '鸡', '狗', '猪');
    static Gan = new Array('甲', '乙', '丙', '丁', '戊', '己', '庚', '辛', '壬', '癸');
    static Zhi = new Array('子', '丑', '寅', '卯', '辰', '巳', '午', '未', '申', '酉', '戌', '亥');
    static now = new Date();
    static SY = MonoCalendar.now.getFullYear();
    static SM = MonoCalendar.now.getMonth();
    static SD = MonoCalendar.now.getDate();

    inputEl: HTMLElement = null;
    el: HTMLElement = null;
    input: HTMLInputElement = null; //填充日期的input框
    dom: HTMLElement = null; //日期选择器
    today = {
        nowFullYear: 2020,
        nowMonth: 1,
        nowDay: 1,
        nowDays: 3,
        date: function () {
            return this.nowFullYear + '-' + this.nowMonth + '-' + this.nowDay;
        },
    };
    weeks: string[] = ['日', '一', '二', '三', '四', '五', '六'];
    yearDom: HTMLElement = null;
    weekDom: HTMLElement = null;
    monthDom: HTMLElement = null;
    currectYear: number; //当前选择的年份
    currectMonth: number;//当前选择的月份
    choiceDate: {
        year, month, date
    }; //选中的日期
    currectChoice: HTMLElement = null; //选中的日期dom
    isShow: boolean = false;// 默认不显示日期选择器，只有input框聚焦才显示
    todayDom: HTMLElement = null;// 今天的日期dom
    showDateDom: HTMLElement = null;
    btsDom: HTMLElement = null;
    day: number = 1;
    showMonth: any[];

    constructor(options) {
        options = options || {};

        const now = new Date();
        this.today = {
            nowFullYear: now.getFullYear(), // 年份
            nowMonth: now.getMonth() + 1,   // 月份
            nowDay: now.getDate(),          // 几号
            nowDays: now.getDay(),          // 星期几
            date: function () {
                return this.nowFullYear + '-' + this.nowMonth + '-' + this.nowDay;
            },
        };

        this.currectYear = this.today.nowFullYear;
        this.currectMonth = this.today.nowMonth;

        this.initInput();
        this.initDomBox();
        this.initDom();
    }

    initInput() {
        let _this = this;
        this.input = document.querySelector(this.inputEl.tagName); //填充日期的input框
        this.dom = document.createElement('div'); //日期选择器
        let parent = document.querySelector(this.el.tagName);

        parent.appendChild(this.dom);

        if (!this.isShow) {
            this.dom.style.display = 'none';
        }

        this.input.addEventListener('focus', function (e) {
            _this.isShow = true;
            if (_this.isShow) {
                _this.dom.style.display = 'block';
            }
        });

        document.addEventListener('click', function (e) {
            let target = e.target as Node;
            if (target.parentNode == null) {
                return;
            }

            while (target) {
                if (target == _this.input || target == _this.dom) {
                    return;
                }
                target = target.parentNode;
            }

            _this.isShow = false;
            if (!_this.isShow) {
                _this.dom.style.display = 'none';
            }
        });
    }

    initDomBox() {
        this.yearDom = document.createElement('div');
        this.weekDom = document.createElement('div');
        this.monthDom = document.createElement('div');
        this.monthDom.classList.add('mui-calendar-day-container');
        this.btsDom = document.createElement('div');
        this.btsDom.classList.add('mui-calendar-footer');
        this.dom.appendChild(this.yearDom);
        this.dom.appendChild(this.weekDom);
        this.dom.appendChild(this.monthDom);
        this.dom.appendChild(this.btsDom);
    }

    initDom() {
        this.dom.classList.add('mui-calendar');
        this.renderYearDom();
        this.renderWeekDom();
        this.renderMonthDom();
        this.initBtsDom();
    }

    initBtsDom() {
        let _this = this;
        let now = document.createElement('button'); //此刻
        let sure = document.createElement('button');
        now.classList.add('btn-panel');
        now.innerHTML = '今天';
        now.addEventListener('click', () => {
            _this.currectYear = _this.today.nowFullYear;
            _this.currectMonth = _this.today.nowMonth;
            _this.renderMonthDom(_this.showDateDom);
            _this.showCurrectDate(_this.showDateDom);
            if (_this.currectChoice) {
                _this.currectChoice.classList.remove('day-active');
            }
            _this.currectChoice = _this.todayDom;
            _this.currectChoice.classList.add('day-active');
            let this_year = Number(_this.todayDom.getAttribute('data-year'));
            let this_month = Number(_this.todayDom.getAttribute('data-month'));
            let val = _this.todayDom.innerHTML;
            _this.input.value = this_year + '-' + _this.addZero(this_month) + '-' + _this.addZero(val);
        });
        sure.classList.add('btn-primary');
        sure.innerHTML = '确定';
        sure.addEventListener('click', () => {
            _this.isShow = false;
            if (!_this.isShow) {
                _this.dom.style.display = 'none';
            }
        });
        this.btsDom.appendChild(now);
        this.btsDom.appendChild(sure);
    }

    showCurrectDate(dom) {
        dom.innerHTML = this.showDate();
        // var res = CalculateYearGanZhi(this.currectYear);
        // dom.title = res;
    }

    renderYearDom() {
        let _this = this;
        let yearDom = this.yearDom;
        yearDom.classList.add('calendar-header');

        let showDate = (this.showDateDom = document.createElement('div')); //显示当前选中的年月
        showDate.classList.add('header-title');
        this.showCurrectDate(showDate);
        let prevYear = document.createElement('button'); //上一年
        prevYear.innerHTML = `<svg class="rotate-180" xmlns="http://www.w3.org/2000/svg" viewBox="-384 -384 2816 2816" width="16" height="16">
                    <path d="M915 1955l930-931L915 93l90-90 1022 1021L1005 2045l-90-90z M115 1955l930-931L115 93l90-90 1022 1021L205 2045l-90-90z"></path></svg>`;
        prevYear.classList.add('btn-icon', 'btn-middle', 'change-date');
        prevYear.addEventListener('click', function () {
            _this.currectYear--;
            _this.currectChoice = null;
            _this.renderMonthDom();
            _this.showCurrectDate(showDate);
        });
        let prevmonth = document.createElement('button'); //上一月
        prevmonth.innerHTML = `<svg class="rotate-180" xmlns="http://www.w3.org/2000/svg" viewBox="-384 -384 2816 2816" width="16" height="16">
                    <path d="M515 1955l930-931L515 93l90-90 1022 1021L605 2045l-90-90z"></path></svg>`;
        prevmonth.classList.add('btn-icon', 'btn-middle', 'change-date');
        prevmonth.addEventListener('click', function () {
            _this.currectMonth--;
            _this.currectChoice = null;
            if (_this.currectMonth == 0) {
                _this.currectMonth = 12;
                _this.currectYear--;
            }
            _this.renderMonthDom();
            _this.showCurrectDate(showDate);
        });

        var nextYear = document.createElement('button'); //下一年
        nextYear.innerHTML = `<svg xmlns="http://www.w3.org/2000/svg" viewBox="-384 -384 2816 2816" width="16" height="16">
                    <path d="M915 1955l930-931L915 93l90-90 1022 1021L1005 2045l-90-90z M115 1955l930-931L115 93l90-90 1022 1021L205 2045l-90-90z"></path></svg>`;
        nextYear.classList.add('btn-icon', 'btn-middle', 'change-date');
        nextYear.addEventListener('click', function () {
            _this.currectYear++;
            _this.currectChoice = null;
            _this.renderMonthDom();
            _this.showCurrectDate(showDate);
        });
        var nextmonth = document.createElement('button'); //下一月
        nextmonth.innerHTML = `<svg xmlns="http://www.w3.org/2000/svg" viewBox="-384 -384 2816 2816" width="16" height="16">
                    <path d="M515 1955l930-931L515 93l90-90 1022 1021L605 2045l-90-90z"></path></svg>`;
        nextmonth.classList.add('btn-icon', 'btn-middle', 'change-date');
        nextmonth.addEventListener('click', function () {
            _this.currectMonth++;
            _this.currectChoice = null;
            if (_this.currectMonth == 13) {
                _this.currectMonth = 1;
                _this.currectYear++;
            }
            _this.renderMonthDom();
            _this.showCurrectDate(showDate);
        });
        yearDom.appendChild(prevYear);
        yearDom.appendChild(prevmonth);
        yearDom.appendChild(showDate);
        yearDom.appendChild(nextmonth);
        yearDom.appendChild(nextYear);
    }

    renderWeekDom() {
        var weekDom = this.weekDom;
        weekDom.classList.add('mui-calendar-week');
        var weeks = this.weeks;
        var len = weeks.length;
        var fg = document.createDocumentFragment();
        for (var i = 0; i < len; i++) {
            var dayDom = document.createElement('div');
            dayDom.classList.add('week-day');
            dayDom.innerHTML = weeks[i];
            fg.appendChild(dayDom);
        }
        weekDom.appendChild(fg);
    }

    renderMonthDom(self = null) {
        let _this = this;
        let monthDom = this.monthDom;
        let currectYear = this.currectYear; //当前选择的年份
        let currectMonth = this.currectMonth; //当前选择的月份
        let showMonth = this.month(currectYear, currectMonth);
        let fg = document.createDocumentFragment();
        let len = showMonth.length;

        for (var i = 0; i < len; i++) {
            let item = document.createElement('div');
            item.innerHTML = showMonth[i].day;
            item.classList.add('month-day');

            if (showMonth[i].month != this.currectMonth || showMonth[i].year != this.currectYear) {
                item.classList.add('not-this-month');
            }

            let nongli = this.solarDay(showMonth[i].year, showMonth[i].month - 1, showMonth[i].day);
            item.title = '农历' + nongli;
            if (showMonth[i].date == this.today.date()) {
                item.classList.add('today');
            }
            item.setAttribute('data-month', showMonth[i].month);
            item.setAttribute('data-year', showMonth[i].year);
            if (
                showMonth[i].date == this.choiceDate.date &&
                this.currectMonth == this.choiceDate.month &&
                this.currectYear == this.choiceDate.year
            ) {
                item.classList.add('day-active');
                this.currectChoice = item;
            }
            if (showMonth[i].date == this.today.date()) {
                this.todayDom = item;
            }
            item.addEventListener('click', function () {
                if (_this.currectChoice != null) {
                    _this.currectChoice.classList.remove('day-active');
                    _this.choiceDate = null;
                }
                let this_year = Number(this.getAttribute('data-year'));
                let this_month = Number(this.getAttribute('data-month'));

                let val = this.innerHTML;
                _this.input.value = this_year + '-' + _this.addZero(this_month) + '-' + _this.addZero(val);
                _this.choiceDate.date = this_year + '-' + this_month + '-' + val;
                _this.choiceDate.month = this_month;
                _this.choiceDate.year = this_year;
                _this.currectChoice = this;
                this.classList.add('day-active');
                if (this_month != _this.currectMonth) {
                    _this.currectMonth = this_month;
                    _this.currectYear = this_year;
                    _this.showCurrectDate(_this.showDateDom);
                    _this.renderMonthDom(true);
                }
            });
            fg.appendChild(item);
        }
        monthDom.innerHTML = '';
        monthDom.appendChild(fg);
        if (self) {
            _this.dom.style.display = 'block';
        }
    }

    month(fullYear, month) {
        let allDays = new Date(fullYear, month, 0).getDate();
        let weeks = this.weeks;
        let showMonth = (this.showMonth = []);
        let beforeDays = new Date(fullYear, month - 1, 1).getDay();
        beforeDays = beforeDays == 0 ? 7 : beforeDays;
        let lastDay = new Date(fullYear, month - 1, 0).getDate();
        let beginDay = lastDay - beforeDays + 1;

        // 上个月的几天补充进来
        for (var i = beginDay; i <= lastDay; i++) {
            showMonth.push({
                year: fullYear,
                month: month - 1,
                date: fullYear + '-' + (month - 1) + '-' + i,
                day: i,
            });
        }
        // 这个月的天数
        for (var i = 1; i <= allDays; i++) {
            var obj = {
                day: i,
                week: weeks[new Date(fullYear, month - 1, i).getDay()],
            };
            showMonth.push({
                year: fullYear,
                month: month,
                date: fullYear + '-' + month + '-' + i,
                day: i,
            });
        }
        // 下个月的几天补充进来
        var over = 7 * 6 - showMonth.length;
        for (var j = 1; j <= over; j++) {
            showMonth.push({
                year: fullYear,
                month: month + 1,
                date: fullYear + '-' + (month + 1) + '-' + j,
                day: j,
            });
        }
        return showMonth;
    }

    addZero(num) {
        num = String(num);
        if (num.length == 1) {
            num = 0 + num;
        }
        return num;
    }

    showDate() {
        return this.currectYear + '年' + this.addZero(this.currectMonth) + '月';
    }

    showFullDate(day) {
        return this.currectYear + '-' + this.addZero(this.currectMonth) + '-' + this.addZero(day);
    }

    choiceDateFn(day) {
        return this.currectYear + '-' + this.currectMonth + '-' + day;
    }

    cyclical(num) {
        num = parseInt(num);
        // console.log(num--)
        return MonoCalendar.Gan[num % 10] + MonoCalendar.Zhi[num % 12];
    } //==== 传入 offset 传回干支, 0=甲子
    //==== 传回农历 y年的总天数

    lYearDays(y) {
        var i, sum = 348;
        for (i = 0x8000; i > 0x8; i >>= 1) sum += MonoCalendar.lunarInfo[y - 1900] & i ? 1 : 0;
        return sum + this.leapDays(y);
    }

    //==== 传回农历 y年闰月的天数
    leapDays(y) {
        if (this.leapMonth(y)) {
            return MonoCalendar.lunarInfo[y - 1900] & 0x10000 ? 30 : 29;
        }
        else return 0;
    }

    //==== 传回农历 y年闰哪个月 1-12 , 没闰传回 0
    leapMonth(y) {
        return MonoCalendar.lunarInfo[y - 1900] & 0xf;
    }

    //====================================== 传回农历 y年m月的总天数
    monthDays(y, m) {
        return MonoCalendar.lunarInfo[y - 1900] & (0x10000 >> m) ? 30 : 29;
    }

    //==== 算出农历, 传入日期物件, 传回农历日期物件
    //     该物件属性有 .year .month .day .isLeap .yearCyl .dayCyl .monCyl
    Lunar(objDate: Date) {
        let i, temp = 0;
        let baseDate = new Date(1900, 0, 31);
        let offset = (objDate.getTime() - baseDate.getTime()) / 86400000;

        // let dayCyl = offset + 40;
        let monCyl = 14;

        for (i = 1900; i < 2050 && offset > 0; i++) {
            temp = this.lYearDays(i);
            offset -= temp;
            monCyl += 12;
        }
        if (offset < 0) {
            offset += temp;
            i--;
            monCyl -= 12;
        }

        let year = i;
        // let yearCyl = i - 1864;

        let leap = this.leapMonth(i); //闰哪个月
        let isLeap = false;

        for (i = 1; i < 13 && offset > 0; i++) {
            //闰月
            if (leap > 0 && i == leap + 1 && isLeap == false) {
                --i;
                isLeap = true;
                temp = this.leapDays(year);
            } else {
                temp = this.monthDays(year, i);
            }

            //解除闰月
            if (isLeap == true && i == leap + 1) isLeap = false;

            offset -= temp;
            if (isLeap == false) monCyl++;
        }

        if (offset == 0 && leap > 0 && i == leap + 1)
            if (isLeap) {
                isLeap = false;
            } else {
                isLeap = true;
                --i;
                --monCyl;
            }

        if (offset < 0) {
            offset += temp;
            --i;
            --monCyl;
        }

        this.month = i;
        this.day = offset + 1;
    }

    //==== 中文日期
    cDay(m, d) {
        d = parseInt(d);
        var nStr1 = new Array('日', '正', '二', '三', '四', '五', '六', '七', '八', '九', '十', '冬', '腊');
        var nStr2 = new Array('初', '十', '廿', '卅', '　');
        var nStr3 = new Array('日', '一', '二', '三', '四', '五', '六', '七', '八', '九', '十');
        var s;

        s = nStr1[m] + '月';

        switch (d) {
            case 10:
                s += '初十';
                break;
            case 20:
                s += '二十';
                break;
            case 30:
                s += '三十';
                break;
            default:
                s += nStr2[Math.floor(d / 10)];
                s += nStr3[d % 10];
        }
        return s;
    }

    solarDay(y, m, d) {
        var sDObj = new Date(y, m, d);
        var lDObj = new this.Lunar(sDObj);
        //农历BB'+(cld[d].isLeap?'闰 ':' ')+cld[d].lMonth+' 月 '+cld[d].lDay+' 日
        // var tt = cyclical(SY - 1900 + 36) + '年 ' + cDay(lDObj.month, lDObj.day);
        var tt = this.cDay(lDObj.month, lDObj.day);
        return tt;
    }
}
