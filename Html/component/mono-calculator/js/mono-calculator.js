$.fn.MonoCalculator = function (options) {
    var THIS = $(this);

    var expression = THIS.find('.wscalc-expression');
    var input = THIS.find('.wscalc-input');
    var funcDelete = THIS.find('.func-delete');
    var funcClear = THIS.find('.func-clear');
    var funcEqual = THIS.find('.func-equal');

    var funcs = THIS.find('.func');
    funcs.click(function () {
        const v = input[0].value;
        const t = $(this).attr('data-value');
        if (v == '0') {
            if (t == '0') return;
            if (/[1-9]/i.test(parseInt(t))) {
                input[0].value = t;
                return;
            }
        }
        input[0].value = String(input[0].value) + t;
        input.attr('title', input[0].value);
    });

    funcDelete.click(() => {
        const v = input[0].value;
        if (v.length > 1) input[0].value = v.substring(0, v.length - 1);
        else input[0].value = 0;
    });

    funcClear.click(() => (input[0].value = 0));

    funcEqual.click(() => {
        const v = input[0].value;
        let result = 0;

        expression[0].value = v;
        expression.attr('title', v);

        try {
            result = eval(v);
            if (result === undefined) throw '';
        } catch (err) {
            console.log(err);
            result = 0;
        }

        input[0].value = result;
        input.attr('title', input[0].value);
    });
};
