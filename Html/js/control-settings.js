$(document).ready(function () {
    ('use strict');

    $('.switcher').each(function () {
        this.type = 'checkbox';
    });

    $('.btn-close').each(function () {
        var t = $(this);
        t.html(`<svg class="svg-icon" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 2048 2048" width="14" height="14">
                    <path d="M1115 1024l690 691-90 90-691-690-691 690-90-90 690-691-690-691 90-90 691 690 691-690 90 90-690 691z">
                    </path>
                </svg>`);
        var closeObj = $(t.attr('data-close-id'));
        if (closeObj.length > 0) {
            t.on('click', () => {
                const dis = t.attr('data-destroy');
                if (typeof dis === undefined || dis == false) {
                    closeObj.addClass('display-none');
                } else {
                    closeObj.remove();
                }
            });
        }
    });

    $('[data-menu]').each(function () {
        let mObj = $('#' + this.getAttribute('data-menu'));
        // 菜单存在
        if (mObj.length === 1) {
            const t = $(this);
            t.on('click', () => {
                mObj.css('left', this.offsetLeft + 'px').css('top', this.offsetTop + t.outerHeight() + 'px');
                mObj.removeClass('display-none');
                $(document).bind('mousedown', () => {
                    mObj.addClass('display-none');
                    $(document).unbind('mousedown');
                });
            });
        }
    });

    $('.menu').each(function () {
        // 阻止事件冒泡，防止鼠标按下菜单就关闭
        $(this).mousedown(() => false);
        $(this).click(() => {
            $(this).addClass('display-none');
        });
    });

    $('.menuitem').each(function () {
        var t = $(this);
        if (t.attr('disabled')) {
            t.addClass('disabled no-pointer-event');
        }

        let ctr = t.children();
        if (ctr.hasClass('menuitem-self-control')) {
            ctr.css('z-index', t.css('z-index') + 100);
            ctr.on('click', () => {
                return false;
            });
        }
    });

    $('.slider').each(function () {
        let t = $(this);
        this.max = t.attr('max') == undefined ? 100 : this.max;
        this.min = t.attr('min') == undefined ? 0 : this.min;
        if (t.attr('value') === undefined) {
            this.value = this.min;
        }

        t.attr('type', 'range');
        t.bind('input', () => {
            let v = Math.min(Math.max(this.min, this.value), this.max);
            v = ((v - this.min) / (this.max - this.min)) * 100;

            t.attr('value', v).css('background-image', `linear-gradient(to right, #006cbe ${v}%, #454545 ${v}%`);
        });
        t.trigger('input');
    });

    $('.btn-dropdown').each(function () {
        // 添加箭头
        let arrow = $(document.createElement('i'));
        arrow.addClass('fa fa-angle-down arrow-down');
        arrow.insertAfter(this);

        let curIndex = this.dataset.index;
        var opList = $(this).siblings('.option-list');

        if (curIndex === undefined || this.dataset.index < 0) {
            this.dataset.index = 0;
            curIndex = 0;
        } else {
            this.innerText = '';
        }

        // 弹出下拉框事件
        var popupOptionListFunc = () => {
            opList.removeClass('display-none');
            // 点击其他区域隐藏下拉框
            $(document).bind('mousedown', () => {
                opList.addClass('display-none');
                $(document).unbind('mousedown');
            });
            opList.css('min-width', $(this).outerWidth() + 'px');
        };
        $(this).click(popupOptionListFunc);
        arrow.click(popupOptionListFunc);
        opList.mousedown(() => false);

        // 点击下拉框选项
        opList.children().each((i, op) => {
            // 设置初始选项
            if (i == curIndex) {
                this.dataset.index = i;
                this.innerText = op.innerText;
                this.value = op.innerText;
            }

            $(op).on('click', () => {
                this.dataset.index = i;
                this.innerText = op.innerText;
                this.value = op.innerText;
                opList.addClass('display-none');
            });
        });
    });

    $.fn.MonoList = function (options) {
        this.selectable = options && options.selectable !== undefined ? Boolean(options.selectable) : true;
        this.multiSelect = options && options.multiSelect !== undefined ? Boolean(options.multiSelect) : false;
        this.draggable = options && options.draggable !== undefined ? Boolean(options.draggable) : false;
        this.delegate = options && options.delegate ? options.delegate : null;
        this.currentIndex = options && options.currentIndex !== undefined ? options.currentIndex : -1;
        this.onContextMenu = options && options.onContextMenu ? options.onContextMenu : null;
        this.onItemClicked = options && options.onItemClicked ? options.onItemClicked : null;
        this.onItemDoubleClicked = options && options.onItemDoubleClicked ? options.onItemDoubleClicked : null;

        // 只有delegate不为空设置item数量才有效
        this.itemCount = options && !isNaN(parseInt(options.itemCount)) ? Number(options.itemCount) : 0;

        // forcedSelect | repeatClick | noFocus
        this.deselectType = options && options.deselectType ? options.deselectType : 'repeatClick';

        /** Methods */

        this.updateIndexForView = function () {
            items.html(delegate.html().replace(/\${index}/, i));
        };

        this.getSelectedItems = function (isGetJqueryObj = false) {
            var items = new Set();
            if (isGetJqueryObj) {
                THIS.children('li, .list-item').each(function () {
                    if ($(this).hasClass('list-item-selected')) {
                        items.add($(this));
                    }
                });
            } else {
                THIS.children('li, .list-item').each(function () {
                    if ($(this).hasClass('list-item-selected')) {
                        items.add(this);
                    }
                });
            }
            return Array.from(items);
        };

        var THIS = $(this);
        var t = this;

        // Delegate
        var delegate = null;
        if (this.delegate[0] == '#') {
            delegate = $(this.delegate);
        } else {
            delegate = $('#' + this.delegate);
        }
        if (delegate && delegate.length > 0) {
            const html = delegate.html();
            for (let i = 0; i < this.itemCount; i++) {
                let item = $('<li></li>').html(html.replace(/\${index}/, i));
                THIS.append(item);
            }
        }

        // Set Current Index
        if (!isNaN(parseInt(this.currentIndex))) {
            if (this.currentIndex >= 0 && this.currentIndex < THIS.children.length) {
                THIS.children[this.currentIndex].addClass('list-item-selected');
            }
        }

        var items = THIS.children('li, .list-item');

        // Context Menu
        THIS.on('contextmenu', (e) => {
            if (this.onContextMenu) {
                e.preventDefault();
                this.onContextMenu(e);
            }
        });

        // Item Clicked Event
        items.on('click', function () {
            var item = $(this);

            do {
                if (t.multiSelect || t.deselectType === 'repeatClick') {
                    if (item.hasClass('list-item-selected')) {
                        item.removeClass('list-item-selected');
                        break;
                    }
                }

                if (!t.multiSelect) {
                    THIS.children('li, .list-item').removeClass('list-item-selected');
                }

                if (t.selectable) {
                    item.addClass('list-item-selected');
                }
            } while (0);

            if (t.onItemClicked) {
                t.onItemClicked(this);
            }
        });

        // Item Double Clicked Event
        items.on('dblclick', function () {
            if (t.onItemDoubleClicked) {
                t.onItemDoubleClicked(this);
            }
        });

        return this;
    };
});
