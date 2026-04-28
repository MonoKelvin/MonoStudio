<template>
  <div v-if="show && totalProcesses > 0" class="stats-bar">
    <div class="stat-item">
      <div class="stat-icon">💻</div>
      <div class="stat-content">
        <div class="stat-label">CPU使用率</div>
        <div class="stat-value">{{ cpuLoad?.toFixed(1) || '0' }}%</div>
        <div class="stat-bar">
          <div class="stat-fill cpu-fill" :style="{ width: `${Math.min(cpuLoad || 0, 100)}%` }"></div>
        </div>
      </div>
    </div>

    <div class="stat-item">
      <div class="stat-icon">🧠</div>
      <div class="stat-content">
        <div class="stat-label">内存使用</div>
        <div class="stat-value">{{ formatMemory(memoryUsed) }} / {{ formatMemory(memoryTotal) }}</div>
        <div class="stat-bar">
          <div class="stat-fill memory-fill" :style="{ width: `${getMemoryPercentageSystem()}%` }"></div>
        </div>
      </div>
    </div>

    <div class="stat-item">
      <div class="stat-icon">🔄</div>
      <div class="stat-content">
        <div class="stat-label">进程总数</div>
        <div class="stat-value">{{ totalProcesses }}</div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { formatMemory } from '../utils/processHelpers'

const props = defineProps({
  show: {
    type: Boolean,
    default: true
  },
  cpuLoad: {
    type: Number,
    default: 0
  },
  memoryUsed: {
    type: Number,
    default: 0
  },
  memoryTotal: {
    type: Number,
    default: 0
  },
  totalProcesses: {
    type: Number,
    default: 0
  }
})

const getMemoryPercentageSystem = () => {
  if (!props.memoryTotal) return 0
  return (props.memoryUsed / props.memoryTotal) * 100
}
</script>

<style scoped>
.stats-bar {
  display: flex;
  padding: 16px 20px;
  background: var(--bg-elevated);
  border-bottom: 1px solid var(--border-color);
  gap: 24px;
}

.stat-item {
  flex: 1;
  display: flex;
  align-items: center;
  gap: 12px;
}

.stat-icon {
  font-size: 20px;
  width: 32px;
  height: 32px;
  display: flex;
  align-items: center;
  justify-content: center;
  background: var(--bg-secondary);
  border-radius: 8px;
}

.stat-content {
  flex: 1;
}

.stat-label {
  font-size: 12px;
  color: var(--text-secondary);
  margin-bottom: 4px;
}

.stat-value {
  font-size: 14px;
  font-weight: 600;
  color: var(--text-primary);
  margin-bottom: 4px;
}

.stat-bar {
  width: 100%;
  height: 4px;
  background: var(--bg-tertiary);
  border-radius: 2px;
  overflow: hidden;
}

.stat-fill {
  height: 100%;
  transition: width 0.3s ease;
}

.stat-fill.cpu-fill {
  background: linear-gradient(90deg, var(--success) 0%, var(--warning) 70%, var(--danger) 100%);
}

.stat-fill.memory-fill {
  background: color-mix(in srgb, var(--accent-color) 60%, var(--bg-elevated) 40%);
}
</style>
