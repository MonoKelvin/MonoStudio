<template>
  <div class="table-row" :class="rowClasses" @dblclick="handleDoubleClick" @click="handleClick"
      @mouseenter="handleRowHover">
    <!-- 进程名称 -->
    <div v-if="column.visible && column.key === 'name'" class="row-cell name-cell">
      <div class="process-info">
        <div class="process-header">
          <ProcessIcon :name="process.name" />
          <div class="process-name">
            <span class="name">{{ process.name }}</span>
            <span v-if="isSystemProcess(process)" class="system-badge">系统</span>
          </div>
        </div>
        <div v-if="process.path" class="process-path" :title="process.path">
          {{ truncatePath(process.path) }}
        </div>
      </div>
    </div>

    <!-- PID -->
    <div v-if="column.visible && column.key === 'pid'" class="row-cell pid-cell">
      <span class="pid">{{ process.pid }}</span>
    </div>

    <!-- 内存 -->
    <div v-if="column.visible && column.key === 'memory'" class="row-cell memory-cell">
      <div class="memory-info">
        <div class="memory-bar">
          <div class="memory-fill" :style="{ width: `${getMemoryPercentage(process)}%` }"></div>
        </div>
        <div class="memory-text">{{ formatMemory(process.memory) }}</div>
      </div>
    </div>

    <!-- CPU -->
    <div v-if="column.visible && column.key === 'cpu'" class="row-cell cpu-cell">
      <div class="cpu-info">
        <div class="cpu-bar">
          <div class="cpu-fill" :style="{ width: `${process.cpu}%` }"></div>
        </div>
        <div class="cpu-text">{{ process.cpu.toFixed(1) }}%</div>
      </div>
    </div>

    <!-- 状态 -->
    <div v-if="column.visible && column.key === 'status'" class="row-cell status-cell">
      <span class="status-badge" :class="getStatusClass(process)">
        {{ getStatusText(process) }}
      </span>
    </div>

    <!-- 操作 -->
    <div v-if="column.visible && column.key === 'actions'" class="row-cell actions-cell">
      <div class="action-buttons">
        <Transition name="fade">
          <div v-if="canSuspendProcess(process)" class="suspend-buttons">
            <BaseButton variant="warning" size="sm" @click.stop="handleSuspendResume(process)"
                :class="{ 'btn-suspended': process.isSuspended }"
                :title="process.isSuspended ? '恢复进程' : '暂停进程'">
              {{ process.isSuspended ? '▶' : '⏸' }}
            </BaseButton>
          </div>
        </Transition>
        <BaseButton variant="default" size="sm" @click.stop="showContextMenu($event, process)"
            class="context-menu-btn" title="更多选项">
          ⋮
        </BaseButton>
      </div>
      <BaseButton variant="danger" size="sm" @click.stop="handleTerminate(process)"
          :disabled="isSystemProcess(process)" class="action-btn" title="结束进程">
        ✕
      </BaseButton>
    </div>
  </div>
</template>

<script setup>
import { computed } from 'vue'
import BaseButton from '../../../../components/base/BaseButton.vue'
import ProcessIcon from './ProcessIcon.vue'
import { isSystemProcess, canSuspendProcess, formatMemory, getMemoryPercentage, truncatePath, getStatusClass, getStatusText } from '../utils/processHelpers'

const props = defineProps({
  process: {
    type: Object,
    required: true
  },
  column: {
    type: Object,
    required: true
  },
  selectedPid: {
    type: Number,
    default: null
  },
  hoveredPid: {
    type: Number,
    default: null
  }
})

const emit = defineEmits(['click', 'double-click', 'suspend-resume', 'terminate', 'context-menu'])

const rowClasses = computed(() => ({
  'selected': props.selectedPid === props.process.pid,
  'high-cpu': props.process.cpu > 80,
  'high-memory': props.process.memory > 500 * 1024 * 1024
}))

const handleClick = () => {
  emit('click', props.process)
}

const handleDoubleClick = () => {
  emit('double-click', props.process)
}

const handleRowHover = () => {
  emit('row-hover', props.process.pid)
}

const handleSuspendResume = async () => {
  emit('suspend-resume', props.process)
}

const handleTerminate = async () => {
  emit('terminate', props.process)
}

const showContextMenu = (event, process) => {
  event.preventDefault()
  emit('context-menu', event, process)
}
</script>

<style scoped>
.table-row {
  display: flex;
  align-items: center;
  padding: 8px 16px;
  border-bottom: 1px solid var(--border-color);
  transition: all 0.15s ease;
  transform: translateX(0);
  position: relative;
  cursor: pointer;
}

.table-row:hover {
  background: color-mix(in srgb, var(--bg-elevated) 50%, var(--hover-bg) 50%);
  transform: translateX(2px);
  box-shadow: 0 1px 3px rgba(0, 0, 0, 0.1);
}

.table-row.selected {
  background: color-mix(in srgb, var(--accent-color) 10%, var(--bg-elevated));
  box-shadow: 0 0 0 1px var(--accent-color) inset;
}

.table-row.high-cpu::before,
.table-row.selected::before,
.table-row.high-memory::before {
  content: '';
  position: absolute;
  left: 0;
  top: 0;
  bottom: 0;
  width: 3px;
  background: var(--accent-color);
}

.table-row.high-cpu::before {
  background: var(--warning);
}

.table-row.high-memory::before {
  background: var(--danger);
}

.table-row:hover::before {
  background: var(--accent-color);
}

.row-cell {
  display: flex;
  align-items: center;
  gap: 8px;
  overflow: hidden;
}

.name-cell {
  flex: 1;
}

.pid-cell {
  min-width: 80px;
  justify-content: center;
}

.memory-cell,
.cpu-cell {
  width: 150px;
}

.memory-cell {
  flex-direction: column;
  gap: 4px;
}

.cpu-cell {
  flex-direction: column;
  gap: 4px;
}

.status-cell {
  width: 100px;
  justify-content: center;
}

.actions-cell {
  width: 150px;
  justify-content: center;
}

.process-info {
  display: flex;
  flex-direction: column;
  gap: 4px;
  width: 100%;
}

.process-header {
  display: flex;
  align-items: center;
  gap: 8px;
}

.process-name {
  display: flex;
  align-items: center;
  gap: 6px;
  flex: 1;
}

.name {
  font-size: 14px;
  color: var(--text-primary);
  font-weight: 500;
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
}

.system-badge {
  font-size: 11px;
  padding: 2px 6px;
  background: color-mix(in srgb, var(--danger) 20%, transparent);
  color: var(--danger);
  border-radius: 4px;
  font-weight: 500;
}

.process-path {
  font-size: 12px;
  color: var(--text-tertiary);
  white-space: nowrap;
  overflow: hidden;
  text-overflow: ellipsis;
}

.pid {
  display: inline-flex;
  align-items: center;
  justify-content: center;
  padding: 2px 6px;
  background: var(--bg-tertiary);
  border-radius: 4px;
  font-size: 12px;
  font-family: monospace;
  color: var(--text-secondary);
}

.memory-bar,
.cpu-bar {
  height: 4px;
  background: var(--bg-tertiary);
  border-radius: 2px;
  overflow: hidden;
}

.memory-fill {
  height: 100%;
  background: color-mix(in srgb, var(--accent-color) 60%, var(--bg-elevated) 40%);
  transition: width 0.3s ease;
}

.cpu-fill {
  height: 100%;
  background: linear-gradient(90deg, var(--success) 0%, var(--warning) 70%, var(--danger) 100%);
  transition: width 0.3s ease;
}

.memory-text,
.cpu-text {
  font-size: 12px;
  color: var(--text-secondary);
}

.action-buttons {
  display: flex;
  gap: 4px;
  margin-right: 4px;
}

.context-menu-btn {
  min-width: auto;
  padding: 4px 8px;
  font-size: 18px;
  line-height: 1;
}

.action-btn {
  min-width: auto;
  padding: 4px 8px;
}

.btn-suspended {
  opacity: 0.6;
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

/* 进度条动画 */
.memory-fill,
.cpu-fill {
  transition: width 0.5s ease-out, background-color 0.3s ease;
}
</style>
