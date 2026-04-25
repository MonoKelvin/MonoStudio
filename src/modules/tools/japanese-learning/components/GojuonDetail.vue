<template>
    <Teleport to="body">
        <Transition name="detail-fade">
            <div v-if="visible" class="detail-overlay" @click="handleOverlayClick">
                <Transition name="detail-scale">
                    <div v-if="visible" class="detail-container" @click.stop>
                        <div class="detail-header">
                            <h3 class="detail-title">假名详情</h3>
                            <button class="close-btn" @click="handleClose">
                                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                                    <path d="M18 6L6 18M6 6l12 12" />
                                </svg>
                            </button>
                        </div>
                        <div class="detail-body" v-if="char">
                            <div class="kana-container">
                                <div class="kana-main">
                                    <div class="kana-item">
                                        <span class="kana-label">平假名</span>
                                        <span class="kana-value hiragana">{{ char.hiragana }}</span>
                                    </div>
                                    <div class="kana-item">
                                        <span class="kana-label">片假名</span>
                                        <span class="kana-value katakana">{{ char.katakana }}</span>
                                    </div>
                                    <div class="kana-item">
                                        <span class="kana-label">罗马音</span>
                                        <span class="kana-value romaji-text">{{ char.romaji }}</span>
                                    </div>
                                </div>
                                <div class="kana-item kanji-source" v-if="char.kanji">
                                    <span class="kana-label">汉字来源</span>
                                    <span class="kana-value kanji-text">{{ char.kanji }}</span>
                                </div>
                            </div>

                            <div class="info-section" v-if="char.history">
                                <h4 class="section-title">历史知识</h4>
                                <div class="history-text">{{ char.history }}</div>
                            </div>

                            <div class="info-section" v-if="examples.length > 0">
                                <h4 class="section-title">示例单词</h4>
                                <div class="examples-list">
                                    <div class="example-item" v-for="(example, idx) in examples" :key="idx">
                                        <span class="example-word" v-html="highlightKana(example.word)"></span>
                                        <span class="example-kana" v-if="example.kana">{{ example.kana }}</span>
                                    </div>
                                </div>
                            </div>


                        </div>
                    </div>
                </Transition>
            </div>
        </Transition>
    </Teleport>
</template>

<script setup>
import { computed } from 'vue';

const props = defineProps({
    visible: {
        type: Boolean,
        default: false
    },
    char: {
        type: Object,
        default: null
    },
    examples: {
        type: Array,
        default: () => []
    }
});

const emit = defineEmits(['update:visible', 'close']);

const handleOverlayClick = () => {
    handleClose();
};

const handleClose = () => {
    emit('update:visible', false);
    emit('close');
};

const highlightKana = (word) => {
    if (!props.char) return word;
    const hiragana = props.char.hiragana;
    const katakana = props.char.katakana;
    const romaji = props.char.romaji;

    if (word.includes(hiragana)) {
        return word.replace(new RegExp(hiragana, 'g'), `<span class="highlight">${hiragana}</span>`);
    }
    if (word.includes(katakana)) {
        return word.replace(new RegExp(katakana, 'g'), `<span class="highlight">${katakana}</span>`);
    }
    return word;
};
</script>

<style scoped>
.detail-overlay {
    position: fixed;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background: rgba(0, 0, 0, 0.6);
    display: flex;
    align-items: center;
    justify-content: center;
    z-index: 9999;
}

.detail-container {
    background: var(--bg-elevated, #1e1e2e);
    border-radius: var(--radius-lg);
    min-width: 220px;
    max-width: 430px;
    max-height: 80vh;
    overflow: hidden;
    display: flex;
    flex-direction: column;
    box-shadow: 0 8px 32px rgba(0, 0, 0, 0.3);
}

.detail-header {
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding: var(--spacing-lg);
    border-bottom: 1px solid var(--border-color);
}

.detail-title {
    font-size: 18px;
    font-weight: 600;
    color: var(--text-primary);
    margin: 0;
}

.close-btn {
    background: transparent;
    border: none;
    padding: 4px;
    cursor: pointer;
    color: var(--text-muted);
    transition: color var(--transition-fast);
    display: flex;
    align-items: center;
    justify-content: center;
}

.close-btn:hover {
    color: var(--text-primary);
}

.close-btn svg {
    width: 20px;
    height: 20px;
}

.detail-body {
    padding: var(--spacing-lg);
    overflow-y: auto;
    display: flex;
    flex-direction: column;
    gap: var(--spacing-lg);
}

.kana-container {
    display: flex;
    flex-direction: row;
    justify-content: space-between;
    align-items: flex-start;
    gap: var(--spacing-md);
    padding: var(--spacing-md) 0;
    flex-wrap: wrap;
}

.kana-main {
    display: flex;
    flex-direction: row;
    justify-content: center;
    align-items: center;
    gap: var(--spacing-lg);
    flex: 1;
}

.kana-item {
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    gap: var(--spacing-xs);
    min-width: 60px;
}

.kanji-source {
    flex-shrink: 0;
    align-items: center;
}

.kanji-source .kana-value {
    font-size: 28px;
}

.kana-label {
    font-size: 12px;
    color: var(--text-muted);
    text-transform: uppercase;
}

.kana-value {
    font-weight: 700;
    line-height: 1;
}

.kana-value.hiragana,
.kana-value.katakana {
    font-size: 36px;
    color: var(--text-primary);
}

.kana-value.romaji-text {
    font-size: 24px;
    color: var(--text-primary);
}

.kana-value.kanji-text {
    font-size: 32px;
    color: var(--text-secondary);
}

.info-section {
    border-top: 1px solid var(--border-color);
    padding-top: var(--spacing-md);
}

.section-title {
    font-size: 14px;
    font-weight: 600;
    color: var(--text-secondary);
    margin: 0 0 var(--spacing-sm) 0;
}

.kanji-info {
    display: flex;
    align-items: center;
    gap: var(--spacing-md);
}

.kanji-char {
    font-size: 36px;
    font-weight: 700;
    color: var(--text-primary);
}

.examples-list {
    display: flex;
    flex-direction: column;
    gap: var(--spacing-sm);
}

.example-item {
    display: flex;
    align-items: center;
    gap: var(--spacing-md);
    padding: var(--spacing-sm);
    background: var(--bg-soft);
    border-radius: var(--radius-md);
}

.example-word {
    font-size: 18px;
    font-weight: 600;
    color: var(--text-primary);
    min-width: 80px;
}

.example-kana {
    font-size: 14px;
    color: var(--text-muted);
}

.highlight {
    color: var(--accent-primary);
    font-weight: 700;
}

.history-text {
    font-size: 14px;
    line-height: 1.6;
    color: var(--text-secondary);
}

/* 过渡动画 */
.detail-fade-enter-active,
.detail-fade-leave-active {
    transition: opacity 0.2s ease;
}

.detail-fade-enter-from,
.detail-fade-leave-to {
    opacity: 0;
}

.detail-scale-enter-active,
.detail-scale-leave-active {
    transition: transform 0.2s ease, opacity 0.2s ease;
}

.detail-scale-enter-from,
.detail-scale-leave-to {
    transform: scale(0.95);
    opacity: 0;
}

/* 响应式调整 */
@media (max-width: 480px) {
    .detail-container {
        min-width: auto;
        width: 95vw;
        margin: var(--spacing-md);
    }

    .kana-display {
        gap: var(--spacing-lg);
    }

    .kana-value {
        font-size: 36px;
    }

    .kana-value.romaji-text {
        font-size: 24px;
    }

    .example-item {
        flex-wrap: wrap;
    }

    .example-word,
    .example-romaji,
    .example-meaning {
        min-width: auto;
    }
}
</style>
