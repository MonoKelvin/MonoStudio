<template>
    <aside class="sidebar app-panel" :class="{ collapsed }">
        <div v-if="!collapsed" class="search-box app-panel-section">
            <div class="search-shell">
                <span class="search-prefix-icon" aria-hidden="true">
                    <img :src="searchIcon" alt="" />
                </span>
                <BaseInput :value="searchQuery" type="text" class="search-input search-input-with-prefix"
                    placeholder="搜索工具" @input="$emit('update-search', $event)" />
                <BaseButton v-if="searchQuery" class="search-clear-btn" size="sm" v-tooltip:top="'清空搜索'"
                    @click="clearSearch">
                    ✕
                </BaseButton>
            </div>
        </div>

        <nav class="tool-tree app-panel-section">
            <template v-if="Object.keys(toolsByCategory).length > 0">
                <div v-for="(categoryTools, category) in toolsByCategory" :key="category" class="tool-category">
                    <div v-if="!collapsed" class="category-title">
                        {{ category }}
                        <img v-if="category === '个人生活'" class="category-lock-icon"
                            :src="personalLifeUnlocked ? unlockIcon : lockIcon" alt="" />
                    </div>
                    <div v-if="collapsed" class="category-divider"></div>
                    <BaseButton v-for="tool in categoryTools" :key="tool.id" class="tree-item"
                        :class="{ active: isToolActive(tool) }" v-tooltip:left="collapsed ? tool.name : ''"
                        @click="$emit('select-tool', tool.id)">
                        <img class="item-icon-svg" :src="iconSrc(tool.id)" alt="" />
                        <span v-if="!collapsed" class="item-main">
                            {{ tool.name }}
                        </span>
                    </BaseButton>
                </div>
            </template>
            <div v-else-if="!collapsed" class="empty-state">未找到匹配工具</div>
        </nav>
        <div class="sidebar-footer app-panel-section">
            <BaseButton class="tree-item settings-entry" :class="{ active: selectedTool === settingsEntry.id }"
                v-tooltip:left="collapsed ? settingsEntry.name : ''" @click="$emit('select-tool', settingsEntry.id)">
                <img class="item-icon-svg" :src="iconSrc(settingsEntry.id)" alt="" />
                <span v-if="!collapsed" class="item-main">
                    {{ settingsEntry.name }}
                </span>
            </BaseButton>
        </div>
    </aside>
</template>

<script>
import iconDesktop from '../../../assets/icons/desktop.svg';
import iconGuid from '../../../assets/icons/guid.svg';
import iconImage from '../../../assets/icons/image.svg';
import iconSearch from '../../../assets/icons/search-20.svg';
import iconSettings from '../../../assets/icons/settings.svg';
import iconSystemInfo from '../../../assets/icons/system-info.svg';
import iconNetworkTools from '../../../assets/icons/network-tools.svg';
import iconProcessManager from '../../../assets/icons/process-manager.svg';
import iconJsonFormatter from '../../../assets/icons/json-formatter.svg';
import iconBase64Encoder from '../../../assets/icons/base64-encoder.svg';
import iconRegexTester from '../../../assets/icons/regex-tester.svg';
import iconVideoConverter from '../../../assets/icons/video-converter.svg';
import iconAudioTools from '../../../assets/icons/audio-tools.svg';
import iconScreenCapture from '../../../assets/icons/screen-capture.svg';
import iconPasswordGenerator from '../../../assets/icons/password-generator.svg';
import iconUnitConverter from '../../../assets/icons/unit-converter.svg';
import iconTimeTools from '../../../assets/icons/time-tools.svg';
import iconFileHasher from '../../../assets/icons/file-hasher.svg';
import iconInspiration from '../../../assets/icons/inspiration.svg';
import iconWorkNotes from '../../../assets/icons/work-notes.svg';
import iconMusings from '../../../assets/icons/musings.svg';
import lockIcon from '../../../assets/icons/lock.svg';
import unlockIcon from '../../../assets/icons/unlock.svg';
import searchIcon from '../../../assets/icons/search-16.svg';
import { hideTooltip } from '../../../directives/tooltip.js';

const icons = {
    'desktop-icon': iconDesktop,
    'file-search': iconSearch,
    'system-info': iconSystemInfo,
    'network-tools': iconNetworkTools,
    'process-manager': iconProcessManager,
    'guid-generator': iconGuid,
    'json-formatter': iconJsonFormatter,
    'base64-encoder': iconBase64Encoder,
    'regex-tester': iconRegexTester,
    'image-converter': iconImage,
    'video-converter': iconVideoConverter,
    'audio-tools': iconAudioTools,
    'screen-capture': iconScreenCapture,
    'password-generator': iconPasswordGenerator,
    'unit-converter': iconUnitConverter,
    'time-tools': iconTimeTools,
    'file-hasher': iconFileHasher,
    'inspiration': iconInspiration,
    'work-notes': iconWorkNotes,
    'musings': iconMusings,
    settings: iconSettings
};

export default {
    name: 'SidebarNav',
    props: {
        searchQuery: { type: String, required: true },
        toolsByCategory: { type: Object, required: true },
        selectedTool: { type: String, required: true },
        previousTool: { type: String, required: true },
        pendingPersonalLifeTool: { type: String, default: null },
        settingsEntry: { type: Object, required: true },
        personalLifeGate: { type: Object, required: true },
        personalLifeUnlocked: { type: Boolean, default: false },
        collapsed: { type: Boolean, default: false }
    },
    methods: {
        iconSrc(id) {
            return icons[id] || icons['file-search'];
        },
        clearSearch() {
            hideTooltip();
            this.$emit('update-search', '');
        },
        isToolActive(tool) {
            if (tool.id === this.selectedTool) return true;
            if (tool.category === '个人生活' && !this.personalLifeUnlocked) {
                if (this.selectedTool === this.personalLifeGate.id) {
                    return tool.id === this.pendingPersonalLifeTool;
                }
            }
            return false;
        }
    },
    data() {
        return {
            searchIcon,
            lockIcon,
            unlockIcon
        };
    }
};
</script>

<style scoped>
.category-lock-icon {
    width: 14px;
    height: 14px;
    margin-left: 8px;
    vertical-align: middle;
    margin-top: -1px;
    opacity: 0.7;
    filter: var(--icon-action-filter);
}

.sidebar {
    user-select: none;
    -webkit-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
}

.category-divider {
    width: 20px;
    height: 1px;
    background-color: var(--text-tertiary);
    margin: 6px auto;
    opacity: 0.3;
}

.sidebar.collapsed .tool-category {
    display: flex;
    flex-direction: column;
    align-items: center;
}
</style>
