<template>
    <BasePanel class="japanese-learning-panel">
        <div class="japanese-learning-container">
            <div class="main-tabs">
                <button class="main-tab" :class="{ active: activeMainTab === 'gojuon' }" @click="activeMainTab = 'gojuon'">
                    <span class="tab-icon">あ</span>
                    <span class="tab-label">五十音图</span>
                </button>
                <button class="main-tab" :class="{ active: activeMainTab === 'vocabulary' }"
                    @click="activeMainTab = 'vocabulary'">
                    <span class="tab-icon">詞</span>
                    <span class="tab-label">常用单词</span>
                </button>
            </div>

            <div class="content-area">
                <Transition name="fade" mode="out-in">
                    <GojuonView v-if="activeMainTab === 'gojuon'" key="gojuon" />
                    <VocabularyView v-else key="vocabulary" />
                </Transition>
            </div>
        </div>
    </BasePanel>
</template>

<script setup>
import { ref } from 'vue';
import BasePanel from '../../../components/base/BasePanel.vue';
import GojuonView from './components/GojuonView.vue';
import VocabularyView from './components/VocabularyView.vue';

const activeMainTab = ref('gojuon');
</script>

<style scoped>
.japanese-learning-panel {
    height: 100%;
    display: flex;
    flex-direction: column;
}

.japanese-learning-container {
    display: flex;
    flex-direction: column;
    height: 100%;
    overflow: hidden;
}

.main-tabs {
    display: flex;
    gap: var(--spacing-sm);
    padding: var(--spacing-md) var(--spacing-lg);
    background: var(--bg-soft);
    border-bottom: 1px solid var(--border-color);
    border-radius: var(--radius-xl) var(--radius-xl) 0 0;
    flex-shrink: 0;
}

.main-tab {
    display: flex;
    align-items: center;
    gap: var(--spacing-sm);
    padding: var(--spacing-sm) var(--spacing-lg);
    border: none;
    background: transparent;
    color: var(--text-secondary);
    font-size: var(--font-size-md);
    cursor: pointer;
    border-radius: var(--radius-md);
    transition: all var(--transition-fast);
}

.main-tab:hover {
    background: var(--bg-elevated);
    color: var(--text-primary);
}

.main-tab.active {
    background: var(--bg-elevated);
    color: var(--accent-color);
    font-weight: 500;
    box-shadow: var(--shadow-card);
}

.tab-icon {
    font-size: var(--font-size-lg);
    font-weight: 600;
}

.tab-label {
    font-weight: 500;
}

.content-area {
    flex: 1;
    overflow: hidden;
}

.fade-enter-active,
.fade-leave-active {
    transition: opacity 0.2s ease;
}

.fade-enter-from,
.fade-leave-to {
    opacity: 0;
}
</style>
