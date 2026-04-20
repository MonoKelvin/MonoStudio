<template>
    <div class="ui-spinbox" @wheel.prevent="onWheel">
        <input ref="inputRef" class="ui-spinbox-input" type="number" :min="min" :max="max" :step="step"
            :value="modelValue" @input="onInput" @blur="onBlur" v-bind="$attrs" />
        <div class="ui-spinbox-arrows">
            <div class="ui-spinbox-btn-up" @mousedown="startIncrement" @mouseup="stopIncrement"
                @mouseleave="stopIncrement" @touchstart="startIncrement" @touchend="stopIncrement">
                <svg width="10" height="6" viewBox="0 0 10 6">
                    <path d="M1 5L5 1L9 5" fill="none" stroke="currentColor" stroke-width="1.4" stroke-linecap="round"
                        stroke-linejoin="round" />
                </svg>
            </div>
            <div class="ui-spinbox-btn-down" @mousedown="startDecrement" @mouseup="stopDecrement"
                @mouseleave="stopDecrement" @touchstart="startDecrement" @touchend="stopDecrement">
                <svg width="10" height="6" viewBox="0 0 10 6">
                    <path d="M1 1L5 5L9 1" fill="none" stroke="currentColor" stroke-width="1.4" stroke-linecap="round"
                        stroke-linejoin="round" />
                </svg>
            </div>
        </div>
    </div>
</template>

<script>
export default {
    name: 'BaseSpinbox',
    inheritAttrs: false,
    props: {
        modelValue: {
            type: Number,
            default: 0
        },
        min: {
            type: Number,
            default: 0
        },
        max: {
            type: Number,
            default: 100
        },
        step: {
            type: Number,
            default: 1
        }
    },
    emits: ['update:modelValue'],
    data() {
        return {
            incrementTimer: null,
            decrementTimer: null,
            repeatInterval: 100
        };
    },
    methods: {
        clampValue(value) {
            return Math.max(this.min, Math.min(this.max, value));
        },
        onWheel(event) {
            if (event.deltaY < 0) {
                this.increment();
            } else {
                this.decrement();
            }
        },
        onInput(event) {
            let numValue = parseFloat(event.target.value);
            if (isNaN(numValue)) {
                this.$emit('update:modelValue', this.min);
            } else {
                let clamped = this.clampValue(numValue);
                this.$emit('update:modelValue', clamped);
            }
        },
        onBlur(event) {
            let numValue = parseFloat(event.target.value);
            let clamped;
            if (isNaN(numValue)) {
                clamped = this.min;
            } else {
                clamped = this.clampValue(numValue);
            }
            this.$emit('update:modelValue', clamped);
            event.target.value = clamped;
        },
        increment() {
            const newValue = this.clampValue(this.modelValue + this.step);
            this.$emit('update:modelValue', newValue);
        },
        decrement() {
            const newValue = this.clampValue(this.modelValue - this.step);
            this.$emit('update:modelValue', newValue);
        },
        startIncrement(event) {
            event.preventDefault();
            this.increment();
            this.incrementTimer = setInterval(() => {
                this.increment();
            }, this.repeatInterval);
        },
        stopIncrement() {
            if (this.incrementTimer) {
                clearInterval(this.incrementTimer);
                this.incrementTimer = null;
            }
        },
        startDecrement(event) {
            event.preventDefault();
            this.decrement();
            this.decrementTimer = setInterval(() => {
                this.decrement();
            }, this.repeatInterval);
        },
        stopDecrement() {
            if (this.decrementTimer) {
                clearInterval(this.decrementTimer);
                this.decrementTimer = null;
            }
        }
    }
};
</script>

<style scoped>
.ui-spinbox {
    position: relative;
    display: inline-flex;
    align-items: stretch;
    height: 30px;
    width: 80px;
    border-radius: 6px;
    background: var(--surface-2);
    box-shadow: inset 0 0 0 1px color-mix(in srgb, var(--border-color) 36%, transparent);
    transition: box-shadow 0.2s ease, background-color 0.2s ease;
}

.ui-spinbox:focus-within {
    outline: none;
    background: color-mix(in srgb, var(--surface-2) 86%, var(--accent-color) 14%);
    box-shadow: inset 0 0 0 1px color-mix(in srgb, var(--accent-color) 45%, transparent);
}

.ui-spinbox-input {
    flex: 1;
    min-width: 0;
    height: 100%;
    padding: 0 8px;
    border: 0;
    background: transparent;
    color: var(--text-primary);
    font-size: 13px;
    text-align: center;
    outline: none;
    box-shadow: none;
}

.ui-spinbox-input:focus {
    outline: none;
    box-shadow: none;
}

.ui-spinbox-input::-webkit-outer-spin-button,
.ui-spinbox-input::-webkit-inner-spin-button {
    -webkit-appearance: none;
    margin: 0;
}

.ui-spinbox-arrows {
    display: flex;
    flex-direction: column;
    justify-content: center;
    gap: 0;
    padding: 3px 4px 3px 2px;
    border-left: 1px solid color-mix(in srgb, var(--border-color) 30%, transparent);
}

.ui-spinbox-btn-up,
.ui-spinbox-btn-down {
    display: flex;
    align-items: center;
    justify-content: center;
    width: 20px;
    height: 12px;
    color: var(--text-secondary);
    cursor: pointer;
    transition: color 0.15s ease, transform 0.1s ease;
    line-height: 1;
}

.ui-spinbox-btn-up:hover,
.ui-spinbox-btn-down:hover {
    color: var(--text-primary);
}

.ui-spinbox-btn-up:active,
.ui-spinbox-btn-down:active {
    transform: scale(0.85);
}
</style>
