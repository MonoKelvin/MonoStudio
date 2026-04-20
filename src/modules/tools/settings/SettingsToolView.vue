<template>
    <div class="settings-expander">
        <aside class="settings-categories">
            <button v-for="group in settingGroups" :key="group.id" class="settings-nav-item"
                :class="{ active: activeGroup === group.id }" @click="activeGroup = group.id">
                <span>{{ group.icon }}</span>
                <div><strong>{{ group.label }}</strong><small>{{ group.desc }}</small></div>
            </button>
        </aside>
        <section class="settings-detail">
            <template v-if="activeGroup === 'personalization'">
                <PersonalizationSettings :theme-mode="themeMode" :theme-options="themeOptions"
                    :current-accent-color="currentAccentColor" @change-theme="$emit('change-theme', $event)"
                    @change-accent-color="$emit('change-accent-color', $event)" />
            </template>
            <template v-else-if="activeGroup === 'app'">
                <AppSettings :open-dev-tools="openDevTools" @change-dev-tools="$emit('change-dev-tools', $event)" @security-settings-changed="$emit('security-settings-changed')" />
            </template>
            <template v-else>
                <AboutSettings @open-sponsor-panel="showSponsorPanel = true"
                    @open-privacy-panel="showPrivacyPanel = true" />
            </template>
        </section>
    </div>

    <!-- 赞助面板 -->
    <SponsorPanel v-model:visible="showSponsorPanel" />

    <!-- 隐私政策面板 -->
    <PrivacyPanel v-model:visible="showPrivacyPanel" />
</template>

<script>
import PersonalizationSettings from './components/personalization/PersonalizationSettings.vue';
import AppSettings from './components/app/AppSettings.vue';
import AboutSettings from './components/about/AboutSettings.vue';
import SponsorPanel from './components/about/SponsorPanel.vue';
import PrivacyPanel from './components/about/PrivacyPanel.vue';

export default {
    name: 'SettingsToolView',
    components: {
        PersonalizationSettings,
        AppSettings,
        AboutSettings,
        SponsorPanel,
        PrivacyPanel
    },
    props: {
        themeMode: { type: String, default: 'system' },
        themeOptions: { type: Array, default: () => ([]) },
        currentAccentColor: { type: String, default: '#8b5cf6' },
        openDevTools: { type: Boolean, default: false }
    },
    emits: ['change-theme', 'change-accent-color', 'change-dev-tools'],
    data() {
        return {
            activeGroup: 'personalization',
            showSponsorPanel: false,
            showPrivacyPanel: false,
            settingGroups: [
                { id: 'personalization', label: '个性化', desc: '主题与外观', icon: '🎨' },
                { id: 'app', label: '应用', desc: '窗口与行为', icon: '🧩' },
                { id: 'about', label: '关于', desc: '产品与版本', icon: 'ℹ️' }
            ]
        };
    }
};
</script>
