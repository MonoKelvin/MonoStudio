$.fn.MonoColorPicker = function (cfg) {
    this.initColor = cfg && typeof cfg.initColor === 'string' ? cfg.initColor : '#000000';
    this.formatSchema = cfg && cfg.formatSchema ? cfg.formatSchema : 'hex';
    this.alphaChannel = cfg && cfg.alphaChannel !== undefined ? Boolean(cfg.alphaChannel) : false;
    this.lowerCase = cfg && cfg.lowerCase !== undefined ? Boolean(cfg.lowerCase) : true;
    this.onPicked = cfg ? cfg.onPicked : null;
    this.onCancel = cfg ? cfg.onCancel : null;
    this.onClosed = cfg ? cfg.onClosed : null;
    this.onValueChanged = cfg ? cfg.onValueChanged : null;
    this.r = 0;
    this.g = 0;
    this.b = 0;
    this.h = 0;
    this.s = 0;
    this.l = 0;
    this.a = 255;

    if ($.fn.MSCPCounter === undefined) {
        $.fn.MSCPCounter = 0;
    } else {
        $.fn.MSCPCounter++;
    }

    const MSCPCounter = $.fn.MSCPCounter;

    var t = $(this);
    var THIS = this;

    /**************/
    /** 初始化组件 */
    /*************/

    // Menu Button
    var menuBtn = $(`<button class="wscp-menu btn-icon" data-menu="wscp-menu-${MSCPCounter}"></button>`);
    menuBtn.html(`<svg class="svg-icon" width="16" height="16" viewBox="0 0 16 16" xmlns="http://www.w3.org/2000/svg">
                    <path fill-rule="evenodd" d="M6 2L0 8L6 14L8 12L10 14L16 8L10 2L8 4L6 2ZM7.29297 4.70715L6 3.41418L1.41431 8L6 12.5858L7.29297 11.2928L4 8L7.29297 4.70715ZM8 10.5858L5.41431 8L8 5.41418L10.5857 8L8 10.5858ZM8.70703 11.2928L12 8L8.70703 4.70715L10 3.41418L14.5857 8L10 12.5858L8.70703 11.2928Z"></path>
                </svg>`);

    // Header Text
    var headerText = $('<input class="header-text">');

    // Close Button
    var closeBtn = $(`<button class="btn-close" data-close-id="#${t.attr('id')}"></button>`);

    let header = $('<div class="wscp-header"></div>');
    header.append(menuBtn).append(headerText).append(closeBtn);

    // Color Panel
    let colorPanel = $('<div class="wscp-color-panel"></div>');
    colorPanel.html(
        '<div class="dummy"></div><div class="wscp-palette wscp-wheel-palette"><div class="picker no-pointer-event"></div></div>'
    );

    // Color Palette
    var palette = colorPanel.find('.wscp-wheel-palette');

    // Picker
    var picker = palette.find('.picker');

    var adjustment = $('<div class="wscp-adj-row"></div>');
    adjustment.html(`<div class="wscp-adj-column">
                <div class="wscp-adj-control">
                    <span>R</span>
                    <input class="r-input" maxlength="3" type="text" placeholder="R">
                </div>
                <div class="wscp-adj-control">
                    <span>G</span>
                    <input class="g-input" maxlength="3" type="text" placeholder="G">
                </div>
                <div class="wscp-adj-control">
                    <span>B</span>
                    <input class="b-input" maxlength="3" type="text" placeholder="B">
                </div>
                <div class="wscp-adj-control ${this.alphaChannel ? '' : 'display-none'}">
                    <span>A</span>
                    <input class="a-input" maxlength="3" type="text" placeholder="A">
                </div>
            </div>
            <div class="column-layout">
                <div class="wscp-adj-control">
                    <span>H</span>
                    <input class="h-input" maxlength="3" type="text" placeholder="H">
                </div>
                <div class="wscp-adj-control">
                    <span>S</span>
                    <input class="s-input" maxlength="3" type="text" placeholder="S">
                </div>
                <div class="wscp-adj-control">
                    <span>L</span>
                    <input class="l-input" maxlength="3" type="text" placeholder="L">
                </div>
            </div>`);

    // Slider and input
    var rInput = adjustment.find('.r-input');
    var gInput = adjustment.find('.g-input');
    var bInput = adjustment.find('.b-input');
    var hInput = adjustment.find('.h-input');
    var sInput = adjustment.find('.s-input');
    var lInput = adjustment.find('.l-input');
    var aInput = adjustment.find('.a-input');

    let footer = $('<div class="wscp-footer"></div>');
    var sureBtn = $('<button class="btn-primary">确定</button>');
    var cancelBtn = $('<button>取消</button>');
    footer.append(sureBtn).append(cancelBtn);

    // Menu
    let menu = $(`<div id="wscp-menu-${MSCPCounter}" class="menu display-none"></div>`);

    var mi_alphaChannel = $('<div class="menuitem"><span></span></div>');
    var mi_letterChange = $('<div class="menuitem"><span>切换大小写</span></div>');
    var mi_copyToHsl = $('<div class="menuitem"><span></span></div>');
    var mi_copyToCssHsl = $('<div class="menuitem"><span></span></div>');
    var mi_copyToRgb = $('<div class="menuitem"><span></span></div>');
    var mi_copyToCssRgb = $('<div class="menuitem"><span></span></div>');

    mi_alphaChannel.prepend(`
        <svg class="svg-icon" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 2048 2048" width="16" height="16">
            <path d="M1024 0q141 0 272 36t245 103 207 160 160 208 103 245 37 272q0 141-36 272t-103 245-160 207-208 160-245 103-272 37q-53 0-99-20t-81-55-55-81-21-100q0-49 9-85t24-67 31-56 31-52 23-56 10-68q0-52-20-99t-55-81-82-55-99-21q-38 0-67 9t-56 24-53 31-56 31-67 23-85 10q-53 0-99-20t-81-55-55-81-21-100q0-141 36-272t103-245 160-207 208-160T751 37t273-37zm0 1920q123 0 237-32t214-90 182-141 140-181 91-214 32-238q0-123-32-237t-90-214-141-182-181-140-214-91-238-32q-123 0-237 32t-214 90-182 141-140 181-91 214-32 238q0 27 10 50t27 40 41 28 50 10q38 0 67-9t56-24 52-31 55-31 67-23 87-10q80 0 150 30t122 82 82 122 30 150q0 49-9 86t-24 67-31 55-31 52-23 56-10 68q0 27 10 50t27 40 41 28 50 10zM512 640q27 0 50 10t40 27 28 41 10 50q0 27-10 50t-27 40-41 28-50 10q-27 0-50-10t-40-27-28-41-10-50q0-27 10-50t27-40 41-28 50-10zm384-256q27 0 50 10t40 27 28 41 10 50q0 27-10 50t-27 40-41 28-50 10q-27 0-50-10t-40-27-28-41-10-50q0-27 10-50t27-40 41-28 50-10zm512 384q-27 0-50-10t-40-27-28-41-10-50q0-27 10-50t27-40 41-28 50-10q27 0 50 10t40 27 28 41 10 50q0 27-10 50t-27 40-41 28-50 10zm128 256q27 0 50 10t40 27 28 41 10 50q0 27-10 50t-27 40-41 28-50 10q-27 0-50-10t-40-27-28-41-10-50q0-27 10-50t27-40 41-28 50-10zm-256 384q27 0 50 10t40 27 28 41 10 50q0 27-10 50t-27 40-41 28-50 10q-27 0-50-10t-40-27-28-41-10-50q0-27 10-50t27-40 41-28 50-10z">
            </path>
        </svg>
    `);
    mi_letterChange.prepend(
        $(
            `<svg class="svg-icon" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 2048 2048" width="14" height="14">
                <path d="M1024 1920q154 0 295-47t258-134 203-208 132-270l122 38q-50 167-149 304t-232 237-294 153-335 55q-137 0-267-34t-245-98-214-157-170-210v243H0v-512h512v128H196q59 117 146 211t196 161 231 103 255 37zM2048 256v512h-512V640h316q-59-117-146-211t-196-161-231-103-255-37q-154 0-295 47T471 309 268 517 136 787L14 749q50-166 149-304t232-237T689 55t335-55q137 0 267 34t245 98 214 157 170 210V256h128z">
                </path>
            </svg>`
        )
    );

    menu.append(mi_alphaChannel)
        .append(mi_letterChange)
        .append($('<div class="menuitem-separator"></div>'))
        .append(mi_copyToHsl)
        .append(mi_copyToCssHsl)
        .append(mi_copyToRgb)
        .append(mi_copyToCssRgb);

    t.append(header).append(colorPanel).append(adjustment).append(footer).append(menu);

    /*************/
    /** 初始化数据 */
    /*************/

    // 绑定输入函数
    function bindInputEvent(input, max = 255) {
        let i = input[0];

        onInputChanged(i, max);

        input.on('input', () => {
            onInputChanged(i, max);
        });

        input[0].onmousewheel = (e) => {
            const v = parseInt(i.value);
            // IE、Chrome...
            if (e.wheelDelta) {
                if (e.wheelDelta > 50) {
                    i.value = v + 1;
                    onInputChanged(i, max);
                } else if (e.wheelDelta < 50) {
                    i.value = v - 1;
                    onInputChanged(i, max);
                }
            } else if (e.detail) {
                // Firefox
                if (e.detail > 50) {
                    i.value = v + 1;
                    onInputChanged(i, max);
                } else if (e.detail < 50) {
                    i.value = v - 1;
                    onInputChanged(i, max);
                }
            }
            return false;
        };
    }

    // 设置标题文字
    function setHeaderText(text) {
        if (THIS.lowerCase) {
            headerText[0].value = text.toLocaleLowerCase();
        } else {
            headerText[0].value = text.toLocaleUpperCase();
        }
    }

    // 切换alpha通道的状态
    function switchAlphaChannel() {
        const aParent = aInput.parent();
        if (THIS.alphaChannel) {
            aParent.removeClass('display-none');
            mi_alphaChannel.find('span').text('关闭alpha通道');
            setHeaderText(rgb2Hex(`rgba(${THIS.r},${THIS.g},${THIS.b},${THIS.a})`));
        } else {
            aParent.addClass('display-none');
            mi_alphaChannel.find('span').text('开启alpha通道');
            setHeaderText(rgb2Hex(`rgb(${THIS.r},${THIS.g},${THIS.b})`));
        }

        updateMenuContent();
    }

    // 绑定复制菜单内容函数
    function bindCopyMenuContentEvent(menuItem) {
        menuItem.on('click', () => {
            let temp = document.createElement('input');
            temp.value = menuItem.children('span').text().substring(3);
            menuItem.append($(temp));
            temp.select();
            document.execCommand('copy');
            temp.remove();
        });
    }

    // 更新菜单内容
    function updateMenuContent() {
        // 复制组菜单
        let hsl = 'hsl';
        let cssHsl = 'hsl';
        let rgb = 'rgb';
        if (THIS.alphaChannel) {
            hsl += `a(${THIS.h},${THIS.s},${THIS.l},${THIS.a})`;
            cssHsl += `a(${THIS.h},${THIS.s}%,${THIS.l}%,${String(THIS.a / 255).substr(0, 5)})`;
            rgb += `a(${THIS.r},${THIS.g},${THIS.b},${THIS.a})`;

            let cssRgb = 'rgb';
            cssRgb += `a(${THIS.r},${THIS.g},${THIS.b},${String(THIS.a / 255).substr(0, 5)})`;
            mi_copyToCssRgb.removeClass('display-none');
            mi_copyToCssRgb.children('span').text('复制：' + cssRgb);
        } else {
            hsl += `(${THIS.h},${THIS.s},${THIS.l})`;
            cssHsl += `(${THIS.h},${THIS.s}%,${THIS.l}%)`;
            rgb += `(${THIS.r},${THIS.g},${THIS.b})`;
            mi_copyToCssRgb.addClass('display-none');
        }

        mi_copyToHsl.children('span').text('复制：' + hsl);
        mi_copyToCssHsl.children('span').text('复制：' + cssHsl);
        mi_copyToRgb.children('span').text('复制：' + rgb);
    }

    [rInput[0].value, gInput[0].value, bInput[0].value, aInput[0].value] = hex2Rgb(this.initColor, 'array');
    if (rInput[0].value == 0 && gInput[0].value == 0 && bInput[0].value == 0) {
        let hex = rgb2Hex(this.initColor);
        rInput[0].value = parseInt(hex.substr(1, 2), 16);
        gInput[0].value = parseInt(hex.substr(3, 2), 16);
        bInput[0].value = parseInt(hex.substr(5, 2), 16);
        if (this.alphaChannel) aInput[0].value = parseInt(hex.substr(7, 2), 16);
    }

    /*************/
    /** 事件相关  */
    /*************/

    // 移动控件
    // this.click((e) => {
    //     const ox = e.clientX + e.offsetX,
    //         oy = e.clientY + e.offsetY;
    //     console.log(ox, oy);
    // });

    // 当输入改变时，约束颜色分量值，并更新对应的区域
    function onInputChanged(jsInput, max = 255) {
        let v = parseInt(jsInput.value);

        if (v.length > 3) {
            v = v.substring(0, 3);
        } else if (v > max) {
            v = max;
        } else if (v < 0) {
            v = 0;
        } else if (isNaN(v)) {
            v = 0;
        }

        jsInput.value = isNaN(v) ? 0 : v;
        if (THIS.onValueChanged) {
            THIS.onValueChanged();
        }

        let rgb = 'rgb';
        if (THIS.alphaChannel) {
            rgb += 'a';
        }

        if (jsInput == rInput[0] || jsInput == gInput[0] || jsInput == bInput[0] || jsInput == aInput[0]) {
            THIS.r = rInput[0].value;
            THIS.g = gInput[0].value;
            THIS.b = bInput[0].value;
            THIS.a = aInput[0].value;
            [THIS.h, THIS.s, THIS.l] = rgbToHsl(THIS.r, THIS.g, THIS.b);

            hInput[0].value = THIS.h;
            sInput[0].value = THIS.s;
            lInput[0].value = THIS.l;

            rgb += `(${THIS.r},${THIS.g},${THIS.b}${THIS.alphaChannel ? ',' + THIS.a : ''})`;
        } else if (jsInput == hInput[0] || jsInput == sInput[0] || jsInput == lInput[0] || jsInput == aInput[0]) {
            THIS.h = hInput[0].value;
            THIS.s = sInput[0].value;
            THIS.l = lInput[0].value;
            THIS.a = aInput[0].value;
            [THIS.r, THIS.g, THIS.b] = hslToRgb(THIS.h, THIS.s, THIS.l);

            rInput[0].value = THIS.r;
            gInput[0].value = THIS.g;
            bInput[0].value = THIS.b;

            rgb += `(${THIS.r},${THIS.g},${THIS.b}${THIS.alphaChannel ? ',' + THIS.a : ''})`;
        }

        headerText.css('background', `rgba(${THIS.r},${THIS.g},${THIS.b},${parseFloat(THIS.a) / 255})`);
        setHeaderText(rgb2Hex(rgb));

        updateMenuContent();
    }

    bindInputEvent(rInput);
    bindInputEvent(gInput);
    bindInputEvent(bInput);
    bindInputEvent(aInput);
    bindInputEvent(hInput, 360);
    bindInputEvent(sInput, 100);
    bindInputEvent(lInput, 100);

    bindCopyMenuContentEvent(mi_copyToHsl);
    bindCopyMenuContentEvent(mi_copyToCssHsl);
    bindCopyMenuContentEvent(mi_copyToRgb);
    bindCopyMenuContentEvent(mi_copyToCssRgb);

    headerText.click(() => {
        headerText.select();
        // document.execCommand('copy');
    });
    headerText.on('input', () => {
        [THIS.r, THIS.g, THIS.b, THIS.a] = hex2Rgb(headerText[0].value, 'array');
        rInput[0].value = THIS.r;
        gInput[0].value = THIS.g;
        bInput[0].value = THIS.b;
        aInput[0].value = THIS.a;

        [THIS.h, THIS.s, THIS.l] = rgbToHsl(THIS.r, THIS.g, THIS.b);
        hInput[0].value = THIS.h;
        sInput[0].value = THIS.s;
        lInput[0].value = THIS.l;
    });
    cancelBtn.click(() => {
        this.addClass('display-none');
        return this.onCancel ? this.onCancel() : null;
    });
    closeBtn.on('click', () => {
        return this.onClosed ? this.onCancel() : null;
    });
    sureBtn.click(() => {
        this.addClass('display-none');
        return this.onPicked ? this.onPicked() : null;
    });
    mi_letterChange.on('click', () => {
        THIS.lowerCase = !THIS.lowerCase;
        setHeaderText(headerText[0].value);
    });
    mi_alphaChannel.on('click', () => {
        THIS.alphaChannel = !THIS.alphaChannel;
        switchAlphaChannel();
    });
    switchAlphaChannel();

    palette.on('mousedown', (e) => {
        let hw = picker.outerWidth() / 2,
            hh = picker.outerHeight() / 2;

        picker.css('left', e.offsetX - hw + 'px');
        picker.css('top', e.offsetY - hh + 'px');

        palette.bind('mousemove', (evt) => {
            picker.css('left', evt.offsetX - hw + 'px');
            picker.css('top', evt.offsetY - hh + 'px');
        });

        $(document).bind('mouseup', () => {
            palette.unbind('mousemove');
            $(document).unbind('mouseup');
        });
    });

    /************/
    /** Helper  */
    /************/

    // 16进制颜色格式转换为RGB格式
    function hex2Rgb(hex, format) {
        let rgb = [];

        if (/^\#[0-9A-F]{3,4}$/i.test(hex)) {
            // 是否为#3、4位十六进制数
            let sixHex = '#';
            hex.replace(/[0-9A-F]/gi, function (kw) {
                sixHex += kw + kw;
            });
            hex = sixHex;
        }

        if (/^#[0-9A-F]{6}$/i.test(hex) || /^#[0-9A-F]{8}$/i.test(hex)) {
            hex.replace(/[0-9A-F]{2}/gi, function (kw) {
                rgb.push(parseInt('0x' + kw));
            });

            if (format === 'array') {
                if (rgb.length < 4) {
                    rgb.push(255);
                }
                return rgb;
            }

            return `rgb(${rgb.join(',')})`;
        }

        if (format === 'array') return [0, 0, 0, 255];
        return 'rgba(0,0,0,255)';
    }

    // RGB格式转16进制
    function rgb2Hex(rgb) {
        if (/^rgb\a?\((\d{1,3}\,){2,3}\d{1,3}\)$/i.test(rgb)) {
            var hex = '#';
            rgb.replace(/\d{1,4}/g, function (kw) {
                kw = parseInt(kw).toString(16);
                kw = kw.length < 2 ? 0 + kw : kw;
                hex += kw;
            });
            return hex;
        }

        return '#000000ff';
    }

    /**
     * HSL颜色值转换为RGB.
     * 换算公式改编自 http://en.wikipedia.org/wiki/HSL_color_space.
     *
     * @param {Number} h 色相
     * @param {Number} s 饱和度
     * @param {Number} l 亮度
     * @param {String} format 输入格式，默认'std'：
     *    'float': 浮点格式，h,s,l 所有输入范围为 0-1
     *    'std': 使用css的标准格式，h在0-360，s、l在 0-100
     * @return {Array} RGB色值数值，在 [0, 255]之间
     */
    function hslToRgb(h, s, l, format) {
        var r, g, b;

        switch (format) {
            case 'std':
                break;
            default:
                h /= 360;
                s /= 100;
                l /= 100;
                break;
        }

        if (s == 0) {
            r = g = b = l; // achromatic
        } else {
            var hue2rgb = function hue2rgb(p, q, t) {
                if (t < 0) t += 1;
                if (t > 1) t -= 1;
                if (t < 1 / 6) return p + (q - p) * 6 * t;
                if (t < 1 / 2) return q;
                if (t < 2 / 3) return p + (q - p) * (2 / 3 - t) * 6;
                return p;
            };

            var q = l < 0.5 ? l * (1 + s) : l + s - l * s;
            var p = 2 * l - q;
            r = hue2rgb(p, q, h + 1 / 3);
            g = hue2rgb(p, q, h);
            b = hue2rgb(p, q, h - 1 / 3);
        }

        return [Math.round(r * 255), Math.round(g * 255), Math.round(b * 255)];
    }

    /**
     * RGB 颜色值转换为 HSL.
     * 转换公式参考自 http://en.wikipedia.org/wiki/HSL_color_space.
     * r, g, 和 b 需要在 [0, 255] 范围内
     *
     * @param {Number} r 红色色值
     * @param {Number} g 绿色色值
     * @param {Number} b 蓝色色值
     * @param {string} format 返回参数，默认为'format':
     *     'array': [h, s, l] - 范围0-1
     *     'css': hsl(h, s%, l%)
     *     'format': [h, s, l] - 格式化后h为0-360，s、l为0-100
     * @return {any} 返回 hsl 格式
     */
    function rgbToHsl(r, g, b, format) {
        r /= 255;
        g /= 255;
        b /= 255;
        var max = Math.max(r, g, b),
            min = Math.min(r, g, b);
        var h,
            s,
            l = (max + min) / 2;

        if (max == min) {
            h = s = 0; // achromatic
        } else {
            var d = max - min;
            s = l > 0.5 ? d / (2 - max - min) : d / (max + min);
            switch (max) {
                case r:
                    h = (g - b) / d + (g < b ? 6 : 0);
                    break;
                case g:
                    h = (b - r) / d + 2;
                    break;
                case b:
                    h = (r - g) / d + 4;
                    break;
            }
            h /= 6;
        }

        switch (format) {
            case 'css':
                return `hsl(${parseInt(h * 360)},${parseInt(s * 100)}%,${parseInt(l * 100)}%)`;
            case 'array':
                return [h, s, l];
        }

        return [parseInt(h * 360), parseInt(s * 100), parseInt(l * 100)];
    }

    /*function rgbToHsv(r, g, b) {
        r = r / 255;
        g = g / 255;
        b = b / 255;
        var h, s, v;
        var min = Math.min(r, g, b);
        var max = (v = Math.max(r, g, b));
        var difference = max - min;

        if (max == min) {
            h = 0;
        } else {
            switch (max) {
                case r:
                    h = (g - b) / difference + (g < b ? 6 : 0);
                    break;
                case g:
                    h = 2.0 + (b - r) / difference;
                    break;
                case b:
                    h = 4.0 + (r - g) / difference;
                    break;
            }
            h = Math.round(h * 60);
        }
        if (max == 0) {
            s = 0;
        } else {
            s = 1 - min / max;
        }
        s = Math.round(s * 100);
        v = Math.round(v * 100);
        return [h, s, v];
    }*/

    return this;
};
