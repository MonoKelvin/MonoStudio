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
                // 立即设置一个默认样式，避免第一次打开时的偏移
                this.dropdownStyle = {
                    position: 'absolute',
                    top: '100%',
                    left: '0',
                    minWidth: '100%'
                };

                // 使用双重 requestAnimationFrame 确保布局稳定
                setTimeout(() => {
                    requestAnimationFrame(() => {
                        requestAnimationFrame(() => {
                            this.positionDropdown();
                        });
                    });
                }, 0);
            }
        },
            positionDropdown() {
            if (!this.$refs.rootRef || !this.$refs.dropdownRef) return;

            const rootRect = this.$refs.rootRef.getBoundingClientRect();
            const dropdownEl = this.$refs.dropdownRef;
            const dropdownRect = dropdownEl.getBoundingClientRect();

            // 如果下拉框尚未渲染出正确尺寸（宽/高为0），则稍后重试
            if (dropdownRect.width === 0 || dropdownRect.height === 0) {
                setTimeout(() => {
                this.positionDropdown();
                }, 10);
                return;
            }

            const viewportHeight = window.innerHeight;
            const viewportWidth = window.innerWidth;

            const style = {};

            // 垂直方向：默认向下偏移4px
            let top = rootRect.height + 4;
            const spaceBelow = viewportHeight - rootRect.bottom;
            const spaceAbove = rootRect.top;

            // 如果下方空间不足但上方空间足够，则改为向上弹出
            if (spaceBelow < dropdownRect.height && spaceAbove > dropdownRect.height) {
                top = -(dropdownRect.height + 4);
            } else if (spaceBelow < dropdownRect.height) {
                // 上下空间都不足，调整到正好不超出视口底部
                top = viewportHeight - rootRect.bottom - dropdownRect.height;
            }

            // 水平方向：默认左对齐
            let left = 0;
            const rootRightSpace = viewportWidth - rootRect.left;
            if (rootRightSpace < dropdownRect.width) {
                // 右侧空间不足，向左偏移，使其右边缘对齐触发器右边缘
                left = -(dropdownRect.width - rootRect.width);
            }

            style.top = `${top}px`;
            style.left = `${left}px`;
            style.position = 'absolute';
            style.minWidth = `${rootRect.width}px`;

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
    background: color-mix(in srgb, var(--bg-elevated) 90%, transparent);
    box-shadow: 0 8px 24px rgba(0, 0, 0, 0.2);
    padding: 6px;
    overflow: auto;
    clip-path: none;
    backdrop-filter: blur(14px);
    -webkit-backdrop-filter: blur(14px);
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
