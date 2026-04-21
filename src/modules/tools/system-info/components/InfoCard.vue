<template>
    <div class="info-card" :class="{ 'card-highlight': highlighted }" ref="cardRef">
        <div class="card-icon" :class="iconClass">
            {{ icon }}
        </div>
        <div class="card-content" ref="contentRef">
            <div class="card-label">{{ label }}</div>
            <div class="card-value" :style="{ fontSize: dynamicFontSize + 'px' }">{{ value }}</div>
            <div v-if="subtitle" class="card-subtitle">{{ subtitle }}</div>
        </div>
    </div>
</template>

<script setup>
import { ref, watch, onMounted, nextTick, onBeforeUnmount } from 'vue';

const props = defineProps({
    icon: {
        type: String,
        required: true
    },
    label: {
        type: String,
        required: true
    },
    value: {
        type: String,
        required: true
    },
    subtitle: {
        type: String,
        default: ''
    },
    highlighted: {
        type: Boolean,
        default: false
    },
    iconClass: {
        type: String,
        default: ''
    }
});

const maxFontSize = 16;
const minFontSize = 12; // 增加最小字体大小

const dynamicFontSize = ref(maxFontSize);
const cardRef = ref(null);
const contentRef = ref(null);
let measureSpan = null;
let resizeObserver = null;

const initMeasureSpan = () => {
    if (typeof document !== 'undefined' && !measureSpan) {
        measureSpan = document.createElement('span');
        measureSpan.style.visibility = 'hidden';
        measureSpan.style.position = 'absolute';
        measureSpan.style.whiteSpace = 'nowrap';
        measureSpan.style.letterSpacing = 'normal';
        document.body.appendChild(measureSpan);
    }
};

const calculateFontSize = () => {
    if (!props.value || props.value.length === 0) {
        dynamicFontSize.value = maxFontSize;
        return;
    }

    // 获取内容区域的实际宽度
    let contentWidth = 180; // 默认宽度

    if (contentRef.value) {
        contentWidth = contentRef.value.offsetWidth;
    } else if (typeof window !== 'undefined') {
        // 回退方案：使用视口宽度估算
        contentWidth = Math.min(window.innerWidth * 0.25, 250);
    }

    // 确保内容宽度不会太小
    contentWidth = Math.max(contentWidth, 120);

    // 初始化测量元素
    initMeasureSpan();

    if (measureSpan && contentWidth > 0) {
        // 从大到小尝试，找到刚好能容纳两行的字体大小
        // 使用二分查找优化性能
        let low = minFontSize;
        let high = maxFontSize;
        let bestSize = minFontSize;

        // 增加内容宽度的估算，让字体更大
        const adjustedContentWidth = contentWidth * 1.05; // 增加5%的宽度

        while (low <= high) {
            const mid = Math.floor((low + high) / 2);
            measureSpan.style.font = `${mid}px/1.2 -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif`;
            measureSpan.textContent = props.value;
            const textWidth = measureSpan.offsetWidth;
            // 允许稍微超过两行，让字体更大
            const estimatedLines = textWidth / adjustedContentWidth;

            if (estimatedLines <= 2.2) { // 允许2.2行
                bestSize = mid;
                high = mid - 1;
            } else {
                low = mid + 1;
            }
        }

        // 增加1-2px，让字体更大
        dynamicFontSize.value = Math.min(maxFontSize, bestSize + 2);
    } else {
        // 回退方案：基于字符数量估算
        const avgCharWidth = maxFontSize * 0.45; // 更小的平均字符宽度，让字体更大
        const maxCharsPerLine = Math.floor(contentWidth / avgCharWidth);
        const maxCharsTwoLines = maxCharsPerLine * 2.2; // 允许更多字符

        if (props.value.length <= maxCharsTwoLines) {
            dynamicFontSize.value = maxFontSize;
        } else {
            const ratio = maxCharsTwoLines / props.value.length;
            // 增加1-2px，让字体更大
            dynamicFontSize.value = Math.max(minFontSize, Math.floor(maxFontSize * ratio) + 2);
        }
    }
};

watch(() => props.value, () => {
    nextTick(() => {
        calculateFontSize();
    });
}, { immediate: true });

onMounted(() => {
    nextTick(() => {
        calculateFontSize();

        // 监听卡片尺寸变化
        if (typeof ResizeObserver !== 'undefined' && cardRef.value) {
            resizeObserver = new ResizeObserver(() => {
                calculateFontSize();
            });
            resizeObserver.observe(cardRef.value);
        }

        // 也监听窗口大小变化
        window.addEventListener('resize', calculateFontSize);
    });
});

onBeforeUnmount(() => {
    if (resizeObserver) {
        resizeObserver.disconnect();
    }
    window.removeEventListener('resize', calculateFontSize);
    if (measureSpan && measureSpan.parentNode) {
        measureSpan.parentNode.removeChild(measureSpan);
    }
});
</script>

<style scoped>
.info-card {
    background-color: var(--surface-2, #ffffff);
    border: 1px solid var(--border-color, #e5e7eb);
    box-shadow: 0 1px 3px rgba(0, 0, 0, 0.05);
    border-radius: var(--radius-md, 8px);
    padding: 16px;
    display: flex;
    align-items: center;
    gap: 12px;
    transition: all 0.2s ease;
}

.info-card:hover {
    border: 1px solid var(--accent-color, #3b82f6);
    box-shadow: 0 2px 6px rgba(0, 0, 0, 0.1);
    transform: translateY(-1px);
    background-color: var(--surface-2, #ffffff);
}

.info-card.card-highlight {
    border: 1px solid rgba(59, 130, 246, 0.3);
    background-color: rgba(59, 130, 246, 0.05);
}

.card-icon {
    font-size: 24px;
    width: 48px;
    height: 48px;
    display: flex;
    align-items: center;
    justify-content: center;
    background-color: rgba(59, 130, 246, 0.1);
    border-radius: var(--radius-sm, 4px);
    color: var(--accent-color, #3b82f6);
    flex-shrink: 0;
    transition: all 0.2s ease;
}

.info-card.card-highlight .card-icon {
    background-color: rgba(59, 130, 246, 0.2);
    color: var(--accent-color, #3b82f6);
}

.card-content {
    flex: 1;
    min-width: 0;
}

.card-label {
    font-size: 12px;
    font-weight: 600;
    color: var(--text-secondary, #6b7280);
    margin-bottom: 4px;
    text-transform: uppercase;
    letter-spacing: 0.05em;
}

.card-value {
    font-size: 16px;
    font-weight: 400;
    color: var(--text-primary, #111827);
    margin-bottom: 2px;
    line-height: 1.2;
    word-break: break-word;
}

.card-subtitle {
    font-size: 12px;
    font-weight: 400;
    color: var(--text-muted, #9ca3af);
    line-height: 1.2;
}

/* 响应式设计 */
@media (max-width: 768px) {
    .info-card {
        padding: 20px;
        gap: 16px;
    }

    .card-icon {
        font-size: 28px;
        width: 56px;
        height: 56px;
    }

    .card-value {
        font-size: 18px;
    }

    .card-label {
        font-size: 13px;
    }

    .card-subtitle {
        font-size: 13px;
    }
}
</style>
