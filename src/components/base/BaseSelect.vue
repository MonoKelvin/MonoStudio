<template>
    <div class="ui-select-wrap" ref="rootRef">
        <button type="button" class="ui-select-trigger" :class="{ open: isOpen }" @click="toggleOpen">
            <span class="ui-select-value">{{ selectedLabel }}</span>
            <span class="ui-select-arrow" aria-hidden="true">
                <img :src="arrowIcon" alt="" />
            </span>
        </button>

        <transition name="ui-select-fade">
            <div v-if="isOpen" class="ui-select-dropdown" ref="dropdownRef" :style="dropdownStyle">
                <button v-for="item in options" :key="item.value" type="button" class="ui-select-option"
                    :class="{ active: item.value === modelValue }" @click="selectOption(item.value)"
                    :title="item.description">
                    {{ item.label }}
                </button>
            </div>
        </transition>
    </div>
</template>

<script>
import arrowIcon from '../../assets/icons/chevron-down.svg';

export default {
    name: 'BaseSelect',
    props: {
        modelValue: {
            type: String,
            default: ''
        },
        options: {
            type: Array,
            required: true
        }
    },
    emits: ['update:modelValue'],
    data() {
        return {
            isOpen: false,
            arrowIcon,
            dropdownStyle: {}
        };
    },
    computed: {
        selectedLabel() {
            const found = this.options.find((item) => item.value === this.modelValue);
            return found ? found.label : '';
        }
    },
    mounted() {
        document.addEventListener('mousedown', this.handleOutsideClick);
    },
    beforeUnmount() {
        document.removeEventListener('mousedown', this.handleOutsideClick);
    },
    methods: {
        toggleOpen() {
            this.isOpen = !this.isOpen;
            if (this.isOpen) {
                this.$nextTick(() => {
                    this.positionDropdown();
                });
            }
        },
        positionDropdown() {
            if (!this.$refs.rootRef || !this.$refs.dropdownRef) return;

            const rootRect = this.$refs.rootRef.getBoundingClientRect();
            const dropdownRect = this.$refs.dropdownRef.getBoundingClientRect();
            const viewportHeight = window.innerHeight;
            const viewportWidth = window.innerWidth;

            const style = {};

            // 计算垂直位置
            let top = rootRect.height + 4;
            if (rootRect.bottom + dropdownRect.height > viewportHeight) {
                top = - (dropdownRect.height + 4);
            }

            // 计算水平位置
            let left = 0;
            if (rootRect.left + dropdownRect.width > viewportWidth) {
                left = viewportWidth - rootRect.left - dropdownRect.width;
            }

            style.top = `${top}px`;
            style.left = `${left}px`;
            style.position = 'absolute';

            this.dropdownStyle = style;
        },
        selectOption(value) {
            this.$emit('update:modelValue', value);
            this.isOpen = false;
        },
        handleOutsideClick(event) {
            if (!this.$refs.rootRef) return;
            if (!this.$refs.rootRef.contains(event.target)) {
                this.isOpen = false;
            }
        }
    }
};
</script>

<style scoped>
:root {
    --accent-color: var(--accent-primary);
}

.ui-select-wrap {
    position: relative;
    display: inline-block;
    width: 100%;
    z-index: 1;
}

.ui-select-trigger {
    box-sizing: border-box;
    font: inherit;
    line-height: 1;
    width: 100%;
    height: 34px;
    display: flex;
    align-items: center;
    justify-content: space-between;
    gap: 8px;
    border: 0;
    border-radius: 6px;
    padding: 0 11px;
    color: var(--text-primary);
    background: var(--surface-2);
    box-shadow: inset 0 0 0 1px color-mix(in srgb, var(--border-color) 36%, transparent);
    transition: box-shadow 0.2s ease, background-color 0.2s ease, color 0.2s ease;
    cursor: pointer;
    text-align: left;
}

.ui-select-trigger:hover {
    background: color-mix(in srgb, var(--surface-2) 86%, var(--accent-color) 14%);
    box-shadow: 0 0 0 2px color-mix(in srgb, var(--accent-color) 45%, transparent);
}

.ui-select-trigger:hover .ui-select-arrow {
    animation: arrowShake 0.5s ease-in-out;
}

.ui-select-trigger:focus-visible,
.ui-select-trigger.open {
    outline: none;
    background: color-mix(in srgb, var(--surface-2) 86%, var(--accent-color) 14%);
    box-shadow: 0 0 0 1px color-mix(in srgb, var(--accent-color) 45%, transparent);
}

.ui-select-value {
    flex: 1;
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
    text-align: left;
}

.ui-select-arrow {
    color: var(--text-secondary);
    display: inline-flex;
    transform: rotate(0deg);
    transition: transform 0.18s ease, color 0.2s ease;
    flex-shrink: 0;
}

.ui-select-trigger.open .ui-select-arrow {
    transform: rotate(-90deg);
    color: var(--text-primary);
    animation: none;
}

.ui-select-arrow img {
    width: 14px;
    height: 14px;
    filter: brightness(0) invert(0.65);
}

/* 动画效果 */
@keyframes arrowShake {

    0%,
    100% {
        transform: translateY(0) rotate(0deg);
    }

    25% {
        transform: translateY(-1px) rotate(0deg);
    }

    75% {
        transform: translateY(1px) rotate(0deg);
    }
}

.ui-select-dropdown {
    z-index: 9999;
    min-width: 100%;
    width: 100%;
    max-width: 320px;
    max-height: 300px;
    border: 0;
    border-radius: 8px;
    background: var(--bg-elevated);
    box-shadow: 0 8px 24px rgba(0, 0, 0, 0.2);
    padding: 6px;
    overflow: auto;
    clip-path: none;
}

.ui-select-option {
    display: block;
    width: 100%;
    border: none;
    border-radius: 6px;
    padding: 8px 12px;
    margin: 2px 0;
    text-align: left;
    font: inherit;
    font-size: 13px;
    line-height: 1.5;
    color: var(--text-primary);
    background: transparent;
    cursor: pointer;
    transition: background-color 0.15s ease, color 0.15s ease;
}

.ui-select-option:hover {
    background: var(--hover-bg);
}

.ui-select-option.active {
    background: var(--accent-color);
    color: #ffffff;
}

.ui-select-fade-enter-active,
.ui-select-fade-leave-active {
    transition: none;
}

.ui-select-fade-enter-from,
.ui-select-fade-leave-to {
    opacity: 0;
}
</style>
