<template>
  <div class="info-card" :class="{ 'card-highlight': highlighted }">
    <div class="card-icon" :class="iconClass">
      {{ icon }}
    </div>
    <div class="card-content">
      <div class="card-label">{{ label }}</div>
      <div class="card-value" :style="{ fontSize: dynamicFontSize + 'px' }">{{ value }}</div>
      <div v-if="subtitle" class="card-subtitle">{{ subtitle }}</div>
    </div>
  </div>
</template>

<script setup>
import { ref, watch, onMounted, nextTick } from 'vue';

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
const minFontSize = 9;

const dynamicFontSize = ref(maxFontSize);
let measureSpan = null;

const initMeasureSpan = () => {
  if (typeof document !== 'undefined' && !measureSpan) {
    measureSpan = document.createElement('span');
    measureSpan.style.visibility = 'hidden';
    measureSpan.style.position = 'absolute';
    measureSpan.style.whiteSpace = 'nowrap';
    measureSpan.style.font = '16px/1.2 -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif';
    document.body.appendChild(measureSpan);
  }
};

const calculateFontSize = () => {
  if (!props.value || props.value.length === 0) {
    dynamicFontSize.value = maxFontSize;
    return;
  }
  
  // 获取卡片内容区域的估计宽度（基于grid布局和响应式设计）
  const cardWidth = 220; // minmax(220px, 1fr) 的最小值
  const contentPadding = 16; // card-content 左右padding
  const contentWidth = cardWidth - contentPadding;
  
  // 初始化测量元素
  initMeasureSpan();
  
  if (measureSpan) {
    // 从大到小尝试，找到刚好能容纳两行的字体大小
    for (let size = maxFontSize; size >= minFontSize; size--) {
      measureSpan.style.font = `${size}px/1.2 -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif`;
      measureSpan.textContent = props.value;
      const textWidth = measureSpan.offsetWidth;
      const estimatedLines = Math.ceil(textWidth / contentWidth);
      
      if (estimatedLines <= 2) {
        dynamicFontSize.value = size;
        return;
      }
    }
    // 如果最小字体还是超过两行，使用最小字体
    dynamicFontSize.value = minFontSize;
  } else {
    // 回退方案：基于字符数量估算
    const maxCharsPerLine = Math.floor(contentWidth / (maxFontSize * 0.5));
    const maxCharsTwoLines = maxCharsPerLine * 2;
    
    if (props.value.length <= maxCharsTwoLines) {
      dynamicFontSize.value = maxFontSize;
    } else {
      const ratio = maxCharsTwoLines / props.value.length;
      dynamicFontSize.value = Math.max(minFontSize, Math.floor(maxFontSize * ratio));
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
  });
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
  font-weight: 700;
  color: var(--text-primary, #111827);
  margin-bottom: 2px;
  line-height: 1.2;
  word-break: break-word;
}

.card-subtitle {
  font-size: 12px;
  font-weight: 400;
  color: var(--text-tertiary, #9ca3af);
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