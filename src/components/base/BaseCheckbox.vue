<template>
    <label class="ui-checkbox-wrap" :class="{ 'ui-checkbox-radio': type === 'radio' }">
        <input class="ui-checkbox-input" :type="type" :name="name" :checked="modelValue === value"
            @change="handleChange" v-bind="$attrs" />
        <span class="ui-checkbox-box" :class="{ 'ui-checkbox-box-radio': type === 'radio' }">
            <img v-if="type === 'checkbox'" :src="checkIcon" alt="" aria-hidden="true" />
            <span v-else-if="type === 'radio'" class="ui-checkbox-radio-dot"></span>
        </span>
        <span v-if="$slots.default" class="ui-checkbox-label">
            <slot />
        </span>
    </label>
</template>

<script>
import checkIcon from '../../assets/icons/check.svg';

export default {
    name: 'BaseCheckbox',
    inheritAttrs: false,
    props: {
        type: {
            type: String,
            default: 'checkbox',
            validator: (value) => ['checkbox', 'radio'].includes(value)
        },
        modelValue: {
            type: [String, Number, Boolean],
            default: false
        },
        value: {
            type: [String, Number, Boolean],
            default: true
        },
        name: {
            type: String,
            default: ''
        }
    },
    data() {
        return {
            checkIcon
        };
    },
    methods: {
        handleChange(event) {
            if (this.type === 'checkbox') {
                this.$emit('update:modelValue', event.target.checked);
            } else if (this.type === 'radio') {
                if (event.target.checked) {
                    this.$emit('update:modelValue', this.value);
                }
            }
        }
    }
};
</script>

<style scoped>
.ui-checkbox-wrap {
    display: inline-flex;
    align-items: center;
    gap: 8px;
    cursor: pointer;
    user-select: none;
}

.ui-checkbox-input {
    position: absolute;
    opacity: 0;
    pointer-events: none;
}

.ui-checkbox-box {
    width: 18px;
    height: 18px;
    border-radius: 4px;
    background: var(--background-secondary);
    border: 2px solid var(--border-color);
    display: inline-flex;
    align-items: center;
    justify-content: center;
    transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
    position: relative;
    overflow: hidden;
}

.ui-checkbox-wrap:hover .ui-checkbox-box {
    border-color: var(--accent-color);
    background: color-mix(in srgb, var(--accent-color) 10%, var(--background-secondary));
    transform: scale(1.05);
}

.ui-checkbox-box-radio {
    border-radius: 50%;
}

.ui-checkbox-box img {
    width: 12px;
    height: 12px;
    opacity: 0;
    transition: all 0.4s cubic-bezier(0.4, 0, 0.2, 1);
    transform: scale(0.8) rotate(-180deg);
    filter: brightness(0) invert(1);
}

.ui-checkbox-radio-dot {
    width: 8px;
    height: 8px;
    border-radius: 50%;
    background: white;
    opacity: 0;
    transition: all 0.4s cubic-bezier(0.4, 0, 0.2, 1);
    transform: scale(0) translateZ(0);
}

.ui-checkbox-input:checked+.ui-checkbox-box {
    background: var(--accent-color);
    border-color: var(--accent-color);
    transform: scale(1);
}

.ui-checkbox-input:checked+.ui-checkbox-box img {
    opacity: 1;
    transform: scale(1) rotate(0deg);
    filter: brightness(0) invert(1);
}

.ui-checkbox-input:checked+.ui-checkbox-box .ui-checkbox-radio-dot {
    opacity: 1;
    transform: scale(1) translateZ(0);
    animation: radioPulse 0.8s ease-in-out;
}

.ui-checkbox-input:focus-visible+.ui-checkbox-box {
    box-shadow: 0 0 0 3px rgba(0, 120, 212, 0.3);
    transform: scale(1.05);
}

.ui-checkbox-label {
    font-size: 13px;
    color: var(--text-primary);
}

@keyframes radioPulse {
    0% {
        transform: scale(0) translateZ(0);
        opacity: 0;
    }

    30% {
        transform: scale(1.4) translateZ(0);
        opacity: 1;
    }

    50% {
        transform: scale(1.1) translateZ(0);
    }

    70% {
        transform: scale(1.2) translateZ(0);
    }

    100% {
        transform: scale(1) translateZ(0);
    }
}
</style>
