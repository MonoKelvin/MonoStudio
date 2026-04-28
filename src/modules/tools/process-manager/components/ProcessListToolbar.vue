<template>
  <div class="controls-header">
    <div class="left-controls">
      <div class="refresh-section">
        <BaseButton variant="primary" size="sm" :loading="isRefreshing" @click="handleRefresh"
            class="refresh-btn">
          <span class="refresh-icon" :class="{ rotating: isRefreshing }">↻</span>
          刷新
        </BaseButton>
        <BaseSwitch :model-value="autoRefreshEnabled" size="sm" @update:model-value="emit('update:autoRefreshEnabled', $event)"
            class="auto-refresh-switch">
          <span class="switch-label">{{ autoRefreshEnabled ? '自动刷新' : '手动' }}</span>
        </BaseSwitch>
      </div>

      <div class="search-section">
        <div class="search-wrapper">
          <span class="search-icon">🔍</span>
          <BaseInput ref="searchInputRef" :model-value="searchQuery" placeholder="搜索进程名称、PID或路径..."
            @update:model-value="emit('update:searchQuery', $event)" class="search-input" />
        </div>
      </div>

      <div class="column-toggle-section" ref="columnMenuRef">
        <BaseButton variant="default" size="sm" @click.stop="toggleColumnMenu" class="column-toggle-btn">
          <span>📋</span>
          列显示
        </BaseButton>
        <Transition name="fade">
          <div v-if="showColumnMenu" class="column-menu" @click.stop>
            <div v-for="column in columns" :key="column.key" class="column-menu-item">
              <BaseSwitch v-model="column.visible" size="sm" @change="handleColumnToggle(column)">
                <span class="column-name">{{ column.title }}</span>
              </BaseSwitch>
            </div>
          </div>
        </Transition>
      </div>
    </div>

    <div class="right-controls">
      <BaseButton variant="default" size="sm" @click="toggleStats" class="stats-toggle">
        <span>📊</span>
        {{ showStats ? '隐藏统计' : '显示统计' }}
      </BaseButton>
    </div>
  </div>

  <!-- 快捷键提示 -->
  <div class="shortcut-hint-bar">
    <span class="shortcut-hint">
      <span class="shortcut-key">Ctrl+R</span> 刷新
    </span>
    <span class="shortcut-hint">
      <span class="shortcut-key">Ctrl+F</span> 搜索
    </span>
    <span class="shortcut-hint">
      <span class="shortcut-key">ESC</span> 关闭
    </span>
  </div>
</template>

<script setup>
import { ref, computed, onMounted, onUnmounted } from 'vue'
import BaseButton from '../../../../components/base/BaseButton.vue'
import BaseInput from '../../../../components/base/BaseInput.vue'
import BaseSwitch from '../../../../components/base/BaseSwitch.vue'

const props = defineProps({
  isRefreshing: {
    type: Boolean,
    default: false
  },
  autoRefreshEnabled: {
    type: Boolean,
    default: true
  },
  searchQuery: {
    type: String,
    default: ''
  },
  columns: {
    type: Array,
    default: () => []
  },
  showStats: {
    type: Boolean,
    default: true
  }
})

const emit = defineEmits(['refresh', 'toggle-auto-refresh', 'search', 'toggle-stats', 'toggle-column-menu', 'column-toggle', 'update:autoRefreshEnabled', 'update:searchQuery'])

const showColumnMenu = ref(false)
const searchInputRef = ref(null)
const columnMenuRef = ref(null)

// 双向绑定计算属性
const autoRefreshEnabledModel = computed({
  get: () => props.autoRefreshEnabled,
  set: (value) => emit('update:autoRefreshEnabled', value)
})

const searchQueryModel = computed({
  get: () => props.searchQuery,
  set: (value) => emit('update:searchQuery', value)
})

const handleClickOutside = (event) => {
  if (columnMenuRef.value && !columnMenuRef.value.contains(event.target)) {
    showColumnMenu.value = false
  }
}

onMounted(() => {
  document.addEventListener('click', handleClickOutside)
})

onUnmounted(() => {
  document.removeEventListener('click', handleClickOutside)
})

const handleRefresh = () => {
  emit('refresh')
}

const handleAutoRefreshChange = () => {
  emit('toggle-auto-refresh')
}

const handleSearch = (value) => {
  emit('search', value)
}

const toggleStats = () => {
  emit('toggle-stats')
}

const toggleColumnMenu = () => {
  showColumnMenu.value = !showColumnMenu.value
  emit('toggle-column-menu')
}

const handleColumnToggle = (column) => {
  column.visible = !column.visible
  showColumnMenu.value = false
  emit('column-toggle', column)
}

// 暴露方法
defineExpose({
  focusSearch: () => {
    searchInputRef.value?.focus()
  }
})
</script>

<style scoped>
.controls-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 16px 20px;
  background: var(--bg-elevated);
  border-bottom: 1px solid var(--border-color);
  gap: 16px;
}

.left-controls {
  display: flex;
  align-items: center;
  gap: 16px;
  flex: 1;
}

.refresh-section {
  display: flex;
  align-items: center;
  gap: 8px;
}

.refresh-btn {
  display: flex;
  align-items: center;
  gap: 6px;
  min-width: 80px;
}

.refresh-icon {
  font-size: 14px;
  transition: transform 0.3s ease;
}

.refresh-icon.rotating {
  animation: rotate 1s linear infinite;
}

@keyframes rotate {
  from {
    transform: rotate(0deg);
  }
  to {
    transform: rotate(360deg);
  }
}

.auto-refresh-switch {
  display: flex;
  align-items: center;
  gap: 6px;
}

.switch-label {
  font-size: 13px;
  color: var(--text-secondary);
}

.search-section {
  flex: 1;
  max-width: 400px;
}

.search-input {
  width: 100%;
}

.search-wrapper {
  position: relative;
  display: flex;
  align-items: center;
}

.search-icon {
  position: absolute;
  left: 10px;
  font-size: 14px;
  z-index: 1;
  pointer-events: none;
}

.search-wrapper .search-input {
  padding-left: 32px;
}

.column-toggle-section {
  position: relative;
}

.column-toggle-btn {
  display: flex;
  align-items: center;
  gap: 6px;
}

.column-menu {
  position: absolute;
  top: 100%;
  left: 0;
  margin-top: 4px;
  background: var(--bg-elevated);
  border: 1px solid var(--border-color);
  border-radius: 8px;
  box-shadow: 0 4px 12px rgba(0, 0, 0, 0.15);
  padding: 8px 0;
  z-index: 100;
  min-width: 160px;
}

.column-menu-item {
  display: flex;
  align-items: center;
  gap: 8px;
  padding: 8px 16px;
  cursor: pointer;
  transition: background-color 0.2s ease;
}

.column-menu-item:hover {
  background: color-mix(in srgb, var(--bg-secondary) 50%, var(--bg-elevated) 50%);
}

.column-name {
  font-size: 14px;
  color: var(--text-primary);
}

.right-controls {
  display: flex;
  align-items: center;
  gap: 8px;
}

.stats-toggle {
  display: flex;
  align-items: center;
  gap: 6px;
}

/* 快捷键提示 */
.shortcut-hint-bar {
  display: flex;
  justify-content: center;
  align-items: center;
  gap: 24px;
  padding: 8px 20px;
  background: var(--bg-tertiary);
  border-bottom: 1px solid var(--border-color);
  font-size: 12px;
}

.shortcut-hint {
  display: flex;
  align-items: center;
  gap: 4px;
}

.shortcut-key {
  display: inline-flex;
  align-items: center;
  gap: 2px;
  padding: 2px 6px;
  background: var(--bg-tertiary);
  border-radius: 3px;
  font-family: monospace;
  font-size: 10px;
}

/* 淡入淡出动画 */
.fade-enter-active,
.fade-leave-active {
  transition: opacity 0.2s ease, transform 0.2s ease;
}

.fade-enter-from,
.fade-leave-to {
  opacity: 0;
  transform: translateY(-4px);
}
</style>
