('use strict');

var MonoLoadingDialog = function (ops) {
    this.target = ops && typeof ops.target === 'string' ? ops.target : null;

    var THIS = $(document.querySelector(this.target));
    if (THIS.length < 1) {
        return null;
    }

    this.title = ops && ops.title !== undefined ? ops.title : null;
    this.text = ops && ops.text !== undefined ? ops.text : '正在加载';
    this.dim = ops && ops.dim !== undefined ? Boolean(ops.dim) : true;
    this.onStart = ops && ops.onStart ? ops.onStart : null;
    this.onLoading = ops && ops.onLoading ? ops.onLoading : null;
    this.onClosed = ops && ops.onClosed ? ops.onClosed : null;

    if (this.onStart) {
        this.onStart();
    }

    THIS.addClass('dialog-loading');
    if (this.dim) {
        THIS.addClass('dim');
    } else {
        THIS.removeClass('dim');
    }

    var container = $('<div class="dialog-container"></div>');
    THIS.append(container);

    container.html(`
        <div class="dialog-content">
            <svg viewBox="0 0 16 16" height="32px" width="32px">
                <circle cx="8px" cy="8px" r="7px"></circle>
                <circle class="circle-run" cx="8px" cy="8px" r="7px"></circle>
            </svg>
        </div>
        <div class="dialog-footer">${this.text}</div>
    `);

    if (this.title) {
        container.prepend($(`<div class="dialog-header"><span>${this.title}</span></div>`));
    }

    var closeBtn = $(`<button class="btn-close" data-close-id="${this.target}" data-destroy></button>`);
    container.prepend(closeBtn);

    closeBtn.on('click', () => {
        return this.onClosed ? this.onClosed() : null;
    });

    THIS.ready(() => {
        setTimeout(() => {
            if (this.onLoading) {
                this.onLoading();
            }
            THIS[0].remove();
        }, 2500);
    });
};
