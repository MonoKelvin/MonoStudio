<template>
  <div class="network-info">
    <div class="info-grid">
      <div class="info-item">
        <span class="label">在线状态:</span>
        <span class="value status-indicator" :class="{ 'status-online': online, 'status-offline': !online }">
          <span class="status-dot"></span>
          {{ online ? '在线' : '离线' }}
        </span>
      </div>
      <div class="info-item">
        <span class="label">连接类型:</span>
        <span class="value">{{ connectionType || '未知' }}</span>
      </div>
      <div class="info-item">
        <span class="label">本地IP地址:</span>
        <span class="value">{{ localIp || '未知' }}</span>
      </div>
      <div class="info-item">
        <span class="label">公网IP地址:</span>
        <span class="value">{{ publicIp || '未知' }}</span>
      </div>
      <div class="info-item">
        <span class="label">网络延迟:</span>
        <span class="value">{{ ping || '未知' }}</span>
      </div>
      <div class="info-item">
        <span class="label">DNS服务器:</span>
        <span class="value">{{ dnsServers || '未知' }}</span>
      </div>
    </div>
  </div>
</template>

<script setup>
defineProps({
  online: {
    type: Boolean,
    default: false
  },
  connectionType: {
    type: String,
    default: ''
  },
  localIp: {
    type: String,
    default: ''
  },
  publicIp: {
    type: String,
    default: ''
  },
  ping: {
    type: String,
    default: ''
  },
  dnsServers: {
    type: String,
    default: ''
  }
});
</script>

<style scoped>
.network-info {
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
}

.status-indicator {
  display: flex;
  align-items: center;
  gap: 6px;
}

.status-dot {
  width: 8px;
  height: 8px;
  border-radius: 50%;
  transition: all 0.2s ease;
}

.status-online .status-dot {
  background-color: var(--success-color, #22c55e);
  box-shadow: 0 0 0 2px rgba(34, 197, 94, 0.2);
  animation: pulse 2s infinite;
}

.status-offline .status-dot {
  background-color: var(--error-color, #ef4444);
  box-shadow: 0 0 0 2px rgba(239, 68, 68, 0.2);
}

@keyframes pulse {
  0%, 100% {
    opacity: 1;
    transform: scale(1);
  }
  50% {
    opacity: 0.7;
    transform: scale(1.1);
  }
}

.status-online {
  color: var(--success-color, #22c55e);
  font-weight: 600;
  display: flex;
  align-items: center;
  gap: 4px;
}

.status-offline {
  color: var(--error-color, #ef4444);
  font-weight: 600;
  display: flex;
  align-items: center;
  gap: 4px;
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