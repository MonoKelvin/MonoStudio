<template>
  <div class="hardware-info">
    <div class="info-grid">
      <div class="info-item">
        <span class="label">CPU:</span>
        <span class="value">{{ cpu }}</span>
      </div>
      <div class="info-item">
        <span class="label">CPU核心数:</span>
        <span class="value">{{ cpuCores || '未知' }}</span>
      </div>
      <div class="info-item">
        <span class="label">内存:</span>
        <span class="value">{{ memory }}</span>
      </div>
      <div class="info-item">
        <span class="label">可用内存:</span>
        <span class="value">{{ freeMemory || '未知' }}</span>
      </div>
      <div class="info-item">
        <span class="label">电池状态:</span>
        <span class="value">{{ battery }}</span>
      </div>
      <div class="info-item">
        <span class="label">屏幕分辨率:</span>
        <span class="value">{{ screen }}</span>
      </div>
      <div class="info-item">
        <span class="label">屏幕像素密度:</span>
        <span class="value">{{ pixelDensity }}</span>
      </div>
      <div class="info-item">
        <span class="label">颜色深度:</span>
        <span class="value">{{ colorDepth }}</span>
      </div>
    </div>
  </div>
</template>

<script setup>
defineProps({
  cpu: {
    type: String,
    default: '未知'
  },
  cpuCores: {
    type: [String, Number],
    default: '未知'
  },
  memory: {
    type: String,
    default: '未知'
  },
  freeMemory: {
    type: String,
    default: '未知'
  },
  battery: {
    type: String,
    default: '未知'
  },
  screen: {
    type: String,
    default: '未知'
  },
  pixelDensity: {
    type: String,
    default: '未知'
  },
  colorDepth: {
    type: String,
    default: '未知'
  }
});
</script>

<style scoped>
.hardware-info {
  width: 100%;
}

.info-grid {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
  gap: 12px;
}

.info-item {
  padding: 12px;
  background-color: var(--surface-2, #ffffff);
  border: 1px solid var(--border-color, #e5e7eb);
  border-radius: var(--radius-md, 8px);
  display: flex;
  justify-content: space-between;
  align-items: center;
  transition: all 0.2s ease;
}

.info-item:hover {
  border-color: var(--accent-color, #3b82f6);
  box-shadow: 0 1px 3px rgba(0, 0, 0, 0.05);
}

.info-item .label {
  font-size: 14px;
  font-weight: 500;
  color: var(--text-primary, #111827);
}

.info-item .value {
  font-size: 14px;
  color: var(--text-secondary, #6b7280);
  font-weight: 400;
  text-align: right;
  flex: 1;
  margin-left: 12px;
  word-break: break-word;
}

/* 响应式设计 */
@media (max-width: 768px) {
  .info-grid {
    grid-template-columns: 1fr;
    gap: 16px;
  }
  
  .info-item {
    padding: 14px;
  }
  
  .info-item .label {
    font-size: 15px;
  }
  
  .info-item .value {
    font-size: 15px;
  }
}
</style>