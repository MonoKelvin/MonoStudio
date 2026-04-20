<template>
    <div class="base-tooltip-wrap" ref="wrapRef">
        <slot />
        <Teleport to="body">
            <Transition name="tooltip-fade">
                <div v-if="visible" class="base-tooltip" :style="tooltipStyle" ref="tooltipRef">
                    <slot name="content">{{ content }}</slot>
                    <div class="tooltip-arrow" :style="arrowStyle"></div>
                </div>
            </Transition>
        </Teleport>
    </div>
</template>

<script>
import { ref, computed, onMounted, onBeforeUnmount } from 'vue';

export default {
    name: 'BaseTooltip',
    props: {
        content: {
            type: String,
            default: ''
        },
        placement: {
            type: String,
            default: 'top',
            validator: (value) => ['top', 'bottom', 'left', 'right'].includes(value)
        },
        delay: {
            type: Number,
            default: 0
        }
    },
    emits: ['show', 'hide'],
    setup(props, { slots }) {
        const visible = ref(false);
        const wrapRef = ref(null);
        const tooltipRef = ref(null);
        let showTimer = null;

        const tooltipStyle = computed(() => {
            if (!wrapRef.value) return {};
            const rect = wrapRef.value.getBoundingClientRect();
            const scrollX = window.scrollX || window.pageXOffset;
            const scrollY = window.scrollY || window.pageYOffset;

            const style = {
                position: 'absolute',
                zIndex: 10000
            };

            switch (props.placement) {
                case 'top':
                    style.left = `${rect.left + scrollX + rect.width / 2}px`;
                    style.top = `${rect.top + scrollY - 8}px`;
                    style.transform = 'translate(-50%, -100%)';
                    break;
                case 'bottom':
                    style.left = `${rect.left + scrollX + rect.width / 2}px`;
                    style.top = `${rect.top + scrollY + rect.height + 8}px`;
                    style.transform = 'translate(-50%, 0)';
                    break;
                case 'left':
                    style.left = `${rect.left + scrollX - 8}px`;
                    style.top = `${rect.top + scrollY + rect.height / 2}px`;
                    style.transform = 'translate(-100%, -50%)';
                    break;
                case 'right':
                    style.left = `${rect.left + scrollX + rect.width + 8}px`;
                    style.top = `${rect.top + scrollY + rect.height / 2}px`;
                    style.transform = 'translate(0, -50%)';
                    break;
            }

            return style;
        });

        const arrowStyle = computed(() => {
            const style = {};
            switch (props.placement) {
                case 'top':
                    style.left = '50%';
                    style.top = '100%';
                    style.transform = 'translate(-50%, -50%) rotate(45deg)';
                    break;
                case 'bottom':
                    style.left = '50%';
                    style.bottom = '100%';
                    style.transform = 'translate(-50%, 50%) rotate(45deg)';
                    break;
                case 'left':
                    style.left = '100%';
                    style.top = '50%';
                    style.transform = 'translate(-50%, -50%) rotate(45deg)';
                    break;
                case 'right':
                    style.right = '100%';
                    style.top = '50%';
                    style.transform = 'translate(50%, -50%) rotate(45deg)';
                    break;
            }
            return style;
        });

        const show = () => {
            if (showTimer) clearTimeout(showTimer);
            showTimer = setTimeout(() => {
                visible.value = true;
            }, props.delay);
        };

        const hide = () => {
            if (showTimer) clearTimeout(showTimer);
            visible.value = false;
        };

        onMounted(() => {
            if (wrapRef.value) {
                wrapRef.value.addEventListener('mouseenter', show);
                wrapRef.value.addEventListener('mouseleave', hide);
            }
        });

        onBeforeUnmount(() => {
            if (wrapRef.value) {
                wrapRef.value.removeEventListener('mouseenter', show);
                wrapRef.value.removeEventListener('mouseleave', hide);
            }
        });

        return {
            visible,
            wrapRef,
            tooltipRef,
            tooltipStyle,
            arrowStyle
        };
    }
};
</script>

<style scoped>
.base-tooltip-wrap {
    display: inline-flex;
    align-items: center;
}

.base-tooltip {
    background: var(--bg-tooltip);
    color: var(--text-tooltip);
    padding: 8px 12px;
    border-radius: 6px;
    font-size: 12px;
    line-height: 1.5;
    max-width: 280px;
    box-shadow: var(--shadow-dropdown);
    pointer-events: none;
}

.tooltip-arrow {
    position: absolute;
    width: 8px;
    height: 8px;
    background: var(--bg-tooltip);
}

.tooltip-fade-enter-active,
.tooltip-fade-leave-active {
    transition: opacity 0.15s ease;
}

.tooltip-fade-enter-from,
.tooltip-fade-leave-to {
    opacity: 0;
}
</style>
