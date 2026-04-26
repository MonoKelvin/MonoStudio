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
                                    <div class="kana-boxes" :class="{ multiple: kanaChars.length > 1 }">
                                        <div class="kana-highlight" v-for="(char, idx) in kanaChars" :key="idx">
                                            <canvas :ref="el => setCanvasRef(el, idx)" class="kana-canvas" width="72"
                                                height="72"></canvas>
                                            <span class="kana-text">{{ char }}</span>
                                        </div>
                                    </div>
                                </div>
                                <div class="kana-info">
                                    <table class="info-table">
                                        <tr v-if="activeKana !== 'hiragana'">
                                            <td class="info-label">平假名</td>
                                            <td class="info-value">{{ char.hiragana }}</td>
                                        </tr>
                                        <tr v-if="activeKana !== 'katakana'">
                                            <td class="info-label">片假名</td>
                                            <td class="info-value">{{ char.katakana }}</td>
                                        </tr>
                                        <tr>
                                            <td class="info-label">罗马音</td>
                                            <td class="info-value">{{ char.romaji }}</td>
                                        </tr>
                                        <tr v-if="char.kanji">
                                            <td class="info-label">汉字源</td>
                                            <td class="info-value">{{ char.kanji }}</td>
                                        </tr>
                                    </table>
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
                                        <span class="example-meaning">{{ example.meaning }}</span>
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
import { computed, ref, watch, onMounted } from 'vue';

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
    },
    activeKana: {
        type: String,
        default: 'hiragana'
    }
});

const emit = defineEmits(['update:visible', 'close']);

const canvasRefs = ref([]);

const setCanvasRef = (el, idx) => {
    if (el) {
        canvasRefs.value[idx] = el;
    }
};

const drawMiziGrid = (canvas) => {
    if (!canvas) return;
    const ctx = canvas.getContext('2d');
    const width = canvas.width;
    const height = canvas.height;

    const style = getComputedStyle(document.documentElement);
    const strokeColor = style.getPropertyValue('--text-muted').trim();

    ctx.clearRect(0, 0, width, height);
    ctx.strokeStyle = strokeColor;
    ctx.globalAlpha = 0.3;
    ctx.lineWidth = 1.7;
    ctx.setLineDash([5, 5]);

    ctx.beginPath();
    ctx.moveTo(width / 2, 0);
    ctx.lineTo(width / 2, height);
    ctx.stroke();

    ctx.beginPath();
    ctx.moveTo(0, height / 2);
    ctx.lineTo(width, height / 2);
    ctx.stroke();

    // 如果要开启米字格，取消注释掉以下内容
    // ctx.beginPath();
    // ctx.moveTo(0, 0);
    // ctx.lineTo(width, height);
    // ctx.stroke();

    // ctx.beginPath();
    // ctx.moveTo(width, 0);
    // ctx.lineTo(0, height);
    // ctx.stroke();
};

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

const kanaChars = computed(() => {
    if (!props.char) return [];
    const kana = props.activeKana === 'hiragana' ? props.char.hiragana : props.char.katakana;
    return kana.split('');
});

watch([() => props.visible, kanaChars], ([visible]) => {
    if (visible) {
        canvasRefs.value = [];
        setTimeout(() => {
            canvasRefs.value.forEach(canvas => drawMiziGrid(canvas));
        }, 50);
    }
}, { immediate: true });
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
    padding: var(--spacing-md) var(--spacing-lg);
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
    align-items: center;
    gap: var(--spacing-lg);
}

.kana-main {
    flex-shrink: 0;
}

.kana-boxes {
    display: flex;
    gap: 4px;
}

.kana-boxes.multiple {
    gap: 8px;
}

.kana-highlight {
    display: inline-flex;
    align-items: center;
    justify-content: center;
    width: 72px;
    height: 72px;
    background: var(--bg-soft);
    border-radius: var(--radius-lg);
    position: relative;
    overflow: hidden;
}

.kana-canvas {
    position: absolute;
    top: 0;
    left: 0;
    pointer-events: none;
}

.kana-text {
    font-size: 40px;
    font-weight: 700;
    color: var(--text-primary);
    z-index: 1;
}

.kana-info {
    flex: 1;
    display: flex;
    justify-content: flex-start;
}

.info-table {
    border-collapse: collapse;
}

.info-label {
    font-size: 14px;
    color: var(--text-muted);
    text-align: right;
    white-space: nowrap;
    padding-right: var(--spacing-sm);
}

.info-value {
    font-size: 16px;
    font-weight: 500;
    color: var(--text-secondary);
    text-align: left;
    padding-left: var(--spacing-xs);
}

.kana-value-cell .romaji-text {
    font-size: 18px;
    font-weight: 500;
    color: var(--text-secondary);
}

.info-section {
    border-top: 1px solid var(--border-color);
    padding-top: var(--spacing-md);
}

.section-title {
    font-size: 16px;
    font-weight: 600;
    color: var(--text-muted);
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
    gap: var(--spacing-xs);
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
    color: var(--accent-primary);
    min-width: 80px;
    margin-left: var(--spacing-md);
}

.example-kana {
    font-size: 14px;
    color: var(--text-muted);
}

.example-meaning {
    font-size: 14px;
    color: var(--text-primary);
    margin-left: auto;
    margin-right: var(--spacing-md);
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
