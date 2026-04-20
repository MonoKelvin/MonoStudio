<template>
    <div class="app-shell app-theme">
        <GlobalTooltip />
        <TitleBar :is-maximized="isMaximized" :is-pinned="isPinned" @minimize="minimizeWindow"
            @toggle-maximize="toggleMaximizeWindow" @toggle-pin="togglePinWindow" @close="closeWindow"
            @change-theme="setThemeAndSave" />
        <div class="workspace shell-grid" :class="{ 'is-resizing': isResizingSidebar }" :style="shellGridStyle">
            <SidebarNav :search-query="searchQuery" :tools-by-category="toolsByCategory" :selected-tool="selectedTool"
                :previous-tool="previousTool" :settings-entry="settingsEntry" :personal-life-gate="personalLifeGate"
                :personal-life-unlocked="personalLifeUnlocked" :collapsed="isSidebarCollapsed"
                @update-search="searchQuery = $event" @select-tool="selectTool" style="width: 100%; min-width: 0;" />
            <div class="sidebar-resizer" @mousedown="startSidebarResize($event)" @dblclick="resetSidebarWidth" />
            <ToolWorkspace :current-tool="currentTool" :theme-mode="currentTheme" :theme-options="themes"
                :current-accent-color="currentAccentColor" :open-dev-tools="settings.openDevTools"
                @change-theme="setThemeAndSave" @change-accent-color="setAccentColorAndSave"
                @change-dev-tools="setDevToolsAndSave" @unlock-personal-life="unlockPersonalLife" />
        </div>
        <div v-if="contextMenu.visible" class="custom-context-menu"
            :style="{ left: `${contextMenu.x}px`, top: `${contextMenu.y}px` }" @mousedown.stop>
            <button v-for="item in contextMenuItems" :key="item.key" class="context-menu-item" :disabled="!item.enabled"
                @click="runContextAction(item.key)">
                {{ item.label }}
            </button>
        </div>
    </div>
</template>

<script setup>
import { computed, onBeforeUnmount, onMounted, ref, watch } from 'vue';
import TitleBar from './modules/shell/components/TitleBar.vue';
import SidebarNav from './modules/shell/components/SidebarNav.vue';
import ToolWorkspace from './modules/shell/components/ToolWorkspace.vue';
import { useWorkbench } from './modules/shell/useWorkbench';
import { APP_CONFIG } from './config/appConfig';
import { useSettings } from './composables/useSettings';

const COLLAPSED_WIDTH = APP_CONFIG.SIDEBAR.COLLAPSED_WIDTH;
const COLLAPSE_THRESHOLD = APP_CONFIG.SIDEBAR.COLLAPSE_THRESHOLD;
const EXPAND_THRESHOLD = APP_CONFIG.SIDEBAR.EXPAND_THRESHOLD;
const MIN_WIDTH = APP_CONFIG.SIDEBAR.MIN_WIDTH;
const MAX_WIDTH = APP_CONFIG.SIDEBAR.MAX_WIDTH;
const DEFAULT_WIDTH = APP_CONFIG.SIDEBAR.DEFAULT_WIDTH;
const RESIZER_WIDTH = APP_CONFIG.SIDEBAR.RESIZER_WIDTH;

const isResizingSidebar = ref(false);
const sidebarWidth = ref(DEFAULT_WIDTH);
const resizeStartX = ref(0);
const resizeStartWidth = ref(DEFAULT_WIDTH);
const contextMenu = ref({ visible: false, x: 0, y: 0, target: null });
const isMaximized = ref(false);
const isPinned = ref(false);

const { settings, updateSetting, initialize } = useSettings();

const {
    searchQuery,
    selectedTool,
    previousTool,
    currentTheme,
    currentAccentColor,
    themes,
    settingsEntry,
    personalLifeGate,
    personalLifeUnlocked,
    filteredTools,
    toolsByCategory,
    currentTool,
    setTheme,
    setAccentColor,
    setSelectedTool,
    unlockPersonalLife
} = useWorkbench();

const isSidebarCollapsed = computed(() => sidebarWidth.value <= COLLAPSED_WIDTH);

const shellGridStyle = computed(() => ({
    gridTemplateColumns: `${sidebarWidth.value}px ${RESIZER_WIDTH}px minmax(0, 1fr)`
}));

function selectTool(toolId) {
    setSelectedTool(toolId);
    updateSettings('selectedTool', toolId);
}

function setThemeAndSave(theme) {
    setTheme(theme);
    updateSettings('theme', theme);
}

function setAccentColorAndSave(color) {
    setAccentColor(color);
    updateSettings('accentColor', color);
}

function setDevToolsAndSave(value) {
    updateSetting('openDevTools', value);
    // 通知主进程打开或关闭 DevTools
    if (window.electronAPI?.devtools?.toggle) {
        window.electronAPI.devtools.toggle(value);
    }
}

async function updateSettings(key, value) {
    if (window.electronAPI?.settings?.save) {
        try {
            const current = await window.electronAPI.settings.get();
            current[key] = value;
            await window.electronAPI.settings.save(current);
        } catch (e) {
            console.error('Failed to save setting:', e);
        }
    }
}

function onSidebarResize(event) {
    if (!isResizingSidebar.value) return;

    // 阻止默认行为和事件冒泡
    event.preventDefault();
    event.stopPropagation();

    // 计算新的侧边栏宽度
    const sidebarEl = document.querySelector('.sidebar');
    const containerRect = document.querySelector('.shell-grid').getBoundingClientRect();
    const newWidth = event.clientX - containerRect.left;

    // 限制宽度范围
    if (newWidth <= COLLAPSE_THRESHOLD) {
        sidebarWidth.value = COLLAPSED_WIDTH;
    } else if (sidebarWidth.value <= COLLAPSED_WIDTH && newWidth < EXPAND_THRESHOLD) {
        sidebarWidth.value = COLLAPSED_WIDTH;
    } else {
        sidebarWidth.value = Math.max(MIN_WIDTH, Math.min(MAX_WIDTH, newWidth));
    }
}

function stopSidebarResize() {
    isResizingSidebar.value = false;
    window.removeEventListener('mousemove', onSidebarResize);
    window.removeEventListener('mouseup', stopSidebarResize);
    document.body.style.userSelect = '';
    document.body.style.cursor = '';
    updateSettings('sidebarWidth', sidebarWidth.value);
}

function startSidebarResize(e) {
    const event = e || window.event;
    // 阻止默认行为和事件冒泡
    if (event) {
        event.preventDefault();
        event.stopPropagation();
    }

    isResizingSidebar.value = true;
    window.addEventListener('mousemove', onSidebarResize);
    window.addEventListener('mouseup', stopSidebarResize);
    document.body.style.userSelect = 'none';
    document.body.style.cursor = 'col-resize';
}

function resetSidebarWidth() {
    sidebarWidth.value = DEFAULT_WIDTH;
    updateSettings('sidebarWidth', DEFAULT_WIDTH);
}

onBeforeUnmount(() => {
    stopSidebarResize();
});

function minimizeWindow() {
    if (window.monoWindow?.minimize) window.monoWindow.minimize();
}

function toggleMaximizeWindow() {
    if (window.monoWindow?.toggleMaximize) window.monoWindow.toggleMaximize();
    isMaximized.value = !isMaximized.value;
}

function togglePinWindow() {
    if (window.monoWindow?.setAlwaysOnTop) window.monoWindow.setAlwaysOnTop(!isPinned.value);
    isPinned.value = !isPinned.value;
}

function closeWindow() {
    if (window.monoWindow?.close) window.monoWindow.close();
}

function canUseTextContextMenu(target) {
    if (!target) return false;
    const el = target.closest('input, textarea, [contenteditable="true"]');
    if (!el) return false;
    if ('disabled' in el && el.disabled) return false;
    if ('readOnly' in el && el.readOnly) return false;
    return true;
}

const contextMenuItems = computed(() => {
    const target = contextMenu.value.target;
    const hasTarget = Boolean(target);
    const selectedText = window.getSelection()?.toString() || '';
    const hasSelection = selectedText.length > 0;
    const isInputLike = target && ('selectionStart' in target || target.tagName === 'TEXTAREA');
    const inputSelection =
        isInputLike && target.selectionStart !== null && target.selectionEnd !== null
            ? target.selectionEnd > target.selectionStart
            : false;
    return [
        { key: 'undo', label: '撤销', enabled: hasTarget },
        { key: 'redo', label: '重做', enabled: hasTarget },
        { key: 'cut', label: '剪切', enabled: hasTarget && (inputSelection || hasSelection) },
        { key: 'copy', label: '复制', enabled: hasTarget && (inputSelection || hasSelection) },
        { key: 'paste', label: '粘贴', enabled: hasTarget },
        { key: 'delete', label: '删除', enabled: hasTarget && (inputSelection || hasSelection) },
        { key: 'selectAll', label: '全选', enabled: hasTarget }
    ];
});

async function runContextAction(action) {
    const target = contextMenu.value.target;
    if (!target) return;
    target.focus?.();
    if (action === 'paste' && navigator.clipboard?.readText) {
        try {
            const text = await navigator.clipboard.readText();
            document.execCommand('insertText', false, text);
            contextMenu.value.visible = false;
            return;
        } catch {
            // fallback to execCommand
        }
    }
    const commandMap = {
        undo: 'undo',
        redo: 'redo',
        cut: 'cut',
        copy: 'copy',
        paste: 'paste',
        delete: 'delete',
        selectAll: 'selectAll'
    };
    document.execCommand(commandMap[action]);
    contextMenu.value.visible = false;
}

function hideContextMenu() {
    contextMenu.value.visible = false;
}

function onGlobalContextMenu(event) {
    if (!canUseTextContextMenu(event.target)) {
        hideContextMenu();
        return;
    }
    event.preventDefault();
    const menuWidth = APP_CONFIG.CONTEXT_MENU.WIDTH;
    const menuHeight = APP_CONFIG.CONTEXT_MENU.HEIGHT;
    const x = Math.min(event.clientX, window.innerWidth - menuWidth - APP_CONFIG.CONTEXT_MENU.MARGIN);
    const y = Math.min(event.clientY, window.innerHeight - menuHeight - APP_CONFIG.CONTEXT_MENU.MARGIN);
    contextMenu.value = {
        visible: true,
        x: Math.max(APP_CONFIG.CONTEXT_MENU.MARGIN, x),
        y: Math.max(APP_CONFIG.CONTEXT_MENU.MARGIN, y),
        target: event.target.closest('input, textarea, [contenteditable="true"]')
    };
}

async function initializeApp() {
    // 初始化设置
    await initialize();

    if (window.electronAPI?.settings?.get) {
        try {
            const settings = await window.electronAPI.settings.get();

            if (settings.selectedTool) {
                setSelectedTool(settings.selectedTool);
            }
            if (settings.theme) {
                setTheme(settings.theme);
            }
            if (settings.accentColor) {
                setAccentColor(settings.accentColor);
            }
            if (settings.sidebarWidth) {
                sidebarWidth.value = settings.sidebarWidth;
            }
            if (settings.openDevTools !== undefined) {
                updateSetting('openDevTools', settings.openDevTools);
            }
        } catch (e) {
            console.error('Failed to load settings:', e);
        }
    }
}

onMounted(() => {
    initializeApp();

    window.addEventListener('contextmenu', onGlobalContextMenu);
    window.addEventListener('click', hideContextMenu);
    window.addEventListener('scroll', hideContextMenu, true);
});

onBeforeUnmount(() => {
    window.removeEventListener('contextmenu', onGlobalContextMenu);
    window.removeEventListener('click', hideContextMenu);
    window.removeEventListener('scroll', hideContextMenu, true);
});
</script>
