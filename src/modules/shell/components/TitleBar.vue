<template>
    <header class="window-titlebar">
        <div class="window-drag-region" @dblclick="$emit('toggle-maximize')">
            <div class="brand">
                <span class="brand-logo">◆</span>
                <div class="brand-meta">
                    <strong>MonoStudio</strong>
                    <small>Desktop Toolbox</small>
                </div>
            </div>
        </div>
        <div class="titlebar-tools no-drag">
            <div class="window-controls">
                <button class="win-btn theme-toggle" v-tooltip:top="tooltipText" @click="toggleTheme">
                    <img :src="themeIcon" alt="" class="win-icon" />
                </button>
                <button class="win-btn pin" :class="{ active: isPinned }" v-tooltip:top="'置顶'"
                    @click="$emit('toggle-pin')">
                    <img :src="pinIcon" alt="" class="win-icon" />
                </button>
                <div class="titlebar-separator"></div>
                <button class="win-btn" v-tooltip:top="'最小化'" @click="$emit('minimize')">
                    <img :src="minimizeIcon" alt="" class="win-icon" />
                </button>
                <button class="win-btn" v-tooltip:top="isMaximized ? '恢复' : '最大化'" @click="$emit('toggle-maximize')">
                    <img :src="maximizeIcon" alt="" class="win-icon" />
                </button>
                <button class="win-btn close" v-tooltip:top="'关闭'" @click="$emit('close')">
                    <img :src="closeIcon" alt="" class="win-icon" />
                </button>
            </div>
        </div>
    </header>
</template>

<script>
import { ref, computed, onMounted } from 'vue';
import { useTheme } from '../useTheme';
import minimizeIcon from '../../../assets/icons/win-minimize.svg';
import maximizeIcon from '../../../assets/icons/win-maximize.svg';
import restoreIcon from '../../../assets/icons/win-restore.svg';
import closeIcon from '../../../assets/icons/win-close.svg';
import pinIcon from '../../../assets/icons/pin.svg';
import themeLightIcon from '../../../assets/icons/theme-light.svg';
import themeDarkIcon from '../../../assets/icons/theme-dark.svg';

export default {
    name: 'TitleBar',
    props: {
        isMaximized: {
            type: Boolean,
            default: false
        },
        isPinned: {
            type: Boolean,
            default: false
        }
    },
    emits: ['change-theme'],
    setup(props, { emit }) {
        const { currentTheme, setTheme } = useTheme();
        const systemThemeMedia = ref(null);

        onMounted(() => {
            systemThemeMedia.value = window.matchMedia('(prefers-color-scheme: dark)');
        });

        const getResolvedTheme = () => {
            if (currentTheme.value === 'system') {
                return systemThemeMedia.value?.matches ? 'dark' : 'light';
            }
            return currentTheme.value;
        };

        const toggleTheme = () => {
            const resolvedTheme = getResolvedTheme();
            const newTheme = resolvedTheme === 'dark' ? 'light' : 'dark';
            setTheme(newTheme);
            emit('change-theme', newTheme);
        };

        const tooltipText = computed(() => {
            const resolvedTheme = getResolvedTheme();
            return resolvedTheme === 'dark' ? '切换到浅色主题' : '切换到深色主题';
        });

        const themeIcon = computed(() => {
            const resolvedTheme = getResolvedTheme();
            return resolvedTheme === 'dark' ? themeLightIcon : themeDarkIcon;
        });

        return {
            currentTheme,
            toggleTheme,
            tooltipText,
            themeIcon
        };
    },
    data() {
        return {
            minimizeIcon,
            closeIcon,
            pinIcon
        };
    },
    computed: {
        maximizeIcon() {
            return this.isMaximized ? restoreIcon : maximizeIcon;
        }
    }
};
</script>

<style scoped>
.titlebar-separator {
    width: 1px;
    height: 16px;
    background-color: rgba(0, 0, 0, 0.1);
    margin: 0 8px;
    align-self: center;
}

[data-theme="dark"] .titlebar-separator {
    background-color: rgba(255, 255, 255, 0.1);
}
</style>
