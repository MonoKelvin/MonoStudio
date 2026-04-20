<template>
  <div class="os-info">
    <div class="info-grid">
      <div class="info-item">
        <span class="label">类型:</span>
        <span class="value">{{ type }}</span>
      </div>
      <div class="info-item">
        <span class="label">版本:</span>
        <span class="value">{{ version }}</span>
      </div>
      <div class="info-item">
        <span class="label">架构:</span>
        <span class="value">{{ arch }}</span>
      </div>
      <div class="info-item">
        <span class="label">平台:</span>
        <span class="value">{{ platform }}</span>
      </div>
      <div class="info-item">
        <span class="label">发行版:</span>
        <span class="value">{{ release || '未知' }}</span>
      </div>
      <div class="info-item">
        <span class="label">主机名:</span>
        <span class="value">{{ hostname || '未知' }}</span>
      </div>
      <div class="info-item">
        <span class="label">运行时间:</span>
        <span class="value">{{ uptime || '未知' }}</span>
      </div>
      <div class="info-item">
        <span class="label">启动时间:</span>
        <span class="value">{{ bootTime || '未知' }}</span>
      </div>
    </div>
  </div>
</template>

<script setup>
defineProps({
  type: {
    type: String,
    default: '未知'
  },
  version: {
    type: String,
    default: '未知'
  },
  arch: {
    type: String,
    default: '未知'
  },
  platform: {
    type: String,
    default: '未知'
  },
  release: {
    type: String,
    default: '未知'
  },
  hostname: {
    type: String,
    default: '未知'
  },
  uptime: {
    type: String,
    default: '未知'
  },
  bootTime: {
    type: String,
    default: '未知'
  }
});
</script>

<style scoped>
.os-info {
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