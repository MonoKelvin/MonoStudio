<template>
  <Teleport to="body">
    <Transition name="detail-fade">
      <div v-if="visible" class="detail-overlay" @click="handleOverlayClick">
        <Transition name="detail-scale">
          <div v-if="visible" class="detail-container" @click.stop>
            <!-- 对话框头部 -->
            <div class="detail-header">
              <div class="header-left">
                <ProcessIcon :name="process?.name" class="detail-process-icon" />
                <div>
                  <h3 class="detail-title">{{ process?.name }}</h3>
                  <p class="detail-subtitle">PID: {{ process?.pid }}</p>
                </div>
              </div>
              <BaseButton
                variant="default"
                size="sm"
                @click="handleClose"
                class="close-btn"
              >
                <span>×</span>
              </BaseButton>
            </div>

            <!-- 对话框内容 -->
            <div class="detail-content">
              <!-- 基本信息 -->
              <div class="info-section">
                <h4 class="section-title">基本信息</h4>
                <div class="info-grid">
                  <InfoItem label="进程名称" :value="process?.name" />
                  <InfoItem label="进程ID" :value="process?.pid" />
                  <InfoItem label="状态" :value="getStatusText(process)" :status="process?.status" />
                  <InfoItem label="优先级" :value="getPriorityText(process)" />
                  <InfoItem label="用户名" :value="process?.username" />
                  <InfoItem label="会话ID" :value="process?.session_id" />
                </div>
              </div>

              <!-- 性能信息 -->
              <div class="info-section">
                <h4 class="section-title">性能信息</h4>
                <div class="performance-grid">
                  <div class="performance-item">
                    <div class="performance-label">CPU使用率</div>
                    <div class="performance-bar">
                      <div
                        class="performance-fill"
                        :style="{ width: `${process?.cpu || 0}%` }"
                      ></div>
                    </div>
                    <div class="performance-value">{{ (process?.cpu || 0).toFixed(1) }}%</div>
                  </div>
                  <div class="performance-item">
                    <div class="performance-label">内存使用</div>
                    <div class="performance-bar">
                      <div
                        class="performance-fill memory-fill"
                        :style="{ width: `${getMemoryPercentage(process)}%` }"
                      ></div>
                    </div>
                    <div class="performance-value">{{ formatMemory(process?.memory || 0) }}</div>
                  </div>
                </div>
              </div>

              <!-- 详细信息 -->
              <div class="info-section">
                <h4 class="section-title">详细信息</h4>
                <div class="info-grid">
                  <InfoItem label="完整路径" :value="process?.path" />
                  <InfoItem label="命令行" :value="process?.command_line" />
                  <InfoItem label="工作目录" :value="process?.working_directory" />
                  <InfoItem label="父进程ID" :value="process?.parent_pid" />
                  <InfoItem label="线程数" :value="process?.threads" />
                  <InfoItem label="句柄数" :value="process?.handles" />
                </div>
              </div>

              <!-- 网络连接 -->
              <div v-if="process?.network && process.network.length > 0" class="info-section">
                <h4 class="section-title">网络连接</h4>
                <div class="network-list">
                  <div
                    v-for="connection in process.network"
                    :key="connection.id"
                    class="network-item"
                  >
                    <div class="network-info">
                      <span class="network-address">{{ connection.address }}</span>
                      <span class="network-type">{{ connection.type }}</span>
                    </div>
                    <div class="network-status">
                      <span
                        class="status-badge"
                        :class="getConnectionStatusClass(connection)"
                      >
                        {{ connection.status }}
                      </span>
                    </div>
                  </div>
                </div>
              </div>
            </div>

            <!-- 对话框底部 -->
            <div class="detail-footer">
              <BaseButton
                variant="danger"
                size="md"
                @click="handleTerminate"
                :disabled="isSystemProcess(process)"
                class="terminate-btn"
              >
                结束进程
              </BaseButton>
              <BaseButton
                variant="default"
                size="md"
                @click="handleClose"
                class="cancel-btn"
              >
                关闭
              </BaseButton>
            </div>
          </div>
        </Transition>
      </div>
    </Transition>
  </Teleport>
</template>

<script>
import { computed } from 'vue'
import BaseButton from '../../../../components/base/BaseButton.vue'
import ProcessIcon from './ProcessIcon.vue'
import InfoItem from './InfoItem.vue'
import dialogService from '../../../../common/services/dialogService'

export default {
  name: 'ProcessDetailDialog',
  components: {
    BaseButton,
    ProcessIcon,
    InfoItem
  },
  props: {
    visible: {
      type: Boolean,
      default: false
    },
    process: {
      type: Object,
      default: null
    }
  },
  emits: ['update:visible', 'terminate'],
  setup(props, { emit }) {
    const isSystemProcess = (process) => {
      if (!process) return true
      const systemProcesses = ['system', 'kernel', 'svchost', 'csrss', 'lsass', 'wininit']
      return systemProcesses.some(sp =>
        process.name.toLowerCase().includes(sp) ||
        (process.path && process.path.toLowerCase().includes('windows'))
      )
    }

    const getStatusText = (process) => {
      const statusMap = {
        running: '运行中',
        sleeping: '睡眠',
        stopped: '已停止',
        suspended: '挂起',
        unknown: '未知'
      }
      return statusMap[process?.status] || '未知'
    }

    const getPriorityText = (process) => {
      const priorityMap = {
        high: '高',
        above_normal: '高于正常',
        normal: '正常',
        below_normal: '低于正常',
        idle: '空闲'
      }
      return priorityMap[process?.priority] || '未知'
    }

    const getMemoryPercentage = (process) => {
      if (!process || !process.memory) return 0
      const maxMemory = 1024 * 1024 * 1024 // 1GB as reference
      return Math.min((process.memory / maxMemory) * 100, 100)
    }

    const formatMemory = (bytes) => {
      if (!bytes) return '0 B'
      if (bytes < 1024) return bytes + ' B'
      if (bytes < 1024 * 1024) return (bytes / 1024).toFixed(1) + ' KB'
      if (bytes < 1024 * 1024 * 1024) return (bytes / (1024 * 1024)).toFixed(1) + ' MB'
      return (bytes / (1024 * 1024 * 1024)).toFixed(1) + ' GB'
    }

    const getConnectionStatusClass = (connection) => {
      if (connection.status === 'LISTENING') return 'status-listening'
      if (connection.status === 'ESTABLISHED') return 'status-established'
      if (connection.status === 'TIME_WAIT') return 'status-time-wait'
      return 'status-closed'
    }

    const handleOverlayClick = () => {
      emit('update:visible', false)
    }

    const handleClose = () => {
      emit('update:visible', false)
    }

    const handleTerminate = async () => {
      if (isSystemProcess(props.process)) {
        dialogService.warning('无法结束系统进程', '警告')
        return
      }

      const confirmed = await dialogService.confirm(
        `确定要结束进程 ${props.process.name} (PID: ${props.process.pid}) 吗？`,
        '确认结束进程',
        'warning'
      )

      if (confirmed) {
        emit('terminate', props.process)
        handleClose()
      }
    }

    return {
      isSystemProcess,
      getStatusText,
      getPriorityText,
      getMemoryPercentage,
      formatMemory,
      getConnectionStatusClass,
      handleOverlayClick,
      handleClose,
      handleTerminate
    }
  }
}
</script>

<style scoped>
.detail-overlay {
  position: fixed;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  background: rgba(0, 0, 0, 0.5);
  backdrop-filter: blur(4px);
  display: flex;
  align-items: center;
  justify-content: center;
  z-index: 10000;
  padding: 20px;
}

.detail-container {
  background: var(--bg-elevated);
  border-radius: 12px;
  box-shadow: 0 20px 60px rgba(0, 0, 0, 0.3);
  max-width: 600px;
  width: 100%;
  max-height: 90vh;
  display: flex;
  flex-direction: column;
  border: 1px solid var(--border-color);
}

.detail-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 20px 24px;
  border-bottom: 1px solid var(--border-color);
}

.header-left {
  display: flex;
  align-items: center;
  gap: 12px;
}

.detail-process-icon {
  width: 32px;
  height: 32px;
}

.detail-title {
  margin: 0;
  font-size: 18px;
  font-weight: 600;
  color: var(--text-primary);
}

.detail-subtitle {
  margin: 4px 0 0;
  font-size: 14px;
  color: var(--text-secondary);
}

.close-btn {
  width: 32px;
  height: 32px;
  padding: 0;
  display: flex;
  align-items: center;
  justify-content: center;
  border-radius: 6px;
}

.close-btn span {
  font-size: 20px;
  line-height: 1;
}

.detail-content {
  flex: 1;
  overflow-y: auto;
  padding: 24px;
}

.info-section {
  margin-bottom: 32px;
}

.info-section:last-child {
  margin-bottom: 0;
}

.section-title {
  margin: 0 0 16px;
  font-size: 16px;
  font-weight: 600;
  color: var(--text-primary);
}

.performance-grid {
  display: flex;
  flex-direction: column;
  gap: 16px;
}

.performance-item {
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.performance-label {
  font-size: 14px;
  color: var(--text-secondary);
}

.performance-bar {
  height: 8px;
  background: var(--bg-tertiary);
  border-radius: 4px;
  overflow: hidden;
}

.performance-fill {
  height: 100%;
  background: linear-gradient(90deg, var(--success) 0%, var(--warning) 70%, var(--danger) 100%);
  transition: width 0.3s ease;
}

.performance-fill.memory-fill {
  background: color-mix(in srgb, var(--accent-color) 60%, var(--bg-elevated) 40%);
}

.performance-value {
  font-size: 14px;
  font-weight: 500;
  color: var(--text-primary);
}

.network-list {
  display: flex;
  flex-direction: column;
  gap: 12px;
}

.network-item {
  display: flex;
  align-items: center;
  justify-content: space-between;
  padding: 12px 16px;
  background: var(--bg-secondary);
  border-radius: 8px;
  border: 1px solid var(--border-color);
}

.network-info {
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.network-address {
  font-size: 14px;
  color: var(--text-primary);
  font-family: monospace;
}

.network-type {
  font-size: 12px;
  color: var(--text-secondary);
}

.status-badge {
  display: inline-flex;
  align-items: center;
  padding: 4px 8px;
  border-radius: 4px;
  font-size: 12px;
  font-weight: 500;
}

.status-listening {
  background: color-mix(in srgb, var(--success) 20%, transparent);
  color: var(--success);
}

.status-established {
  background: color-mix(in srgb, var(--info) 20%, transparent);
  color: var(--info);
}

.status-time-wait {
  background: color-mix(in srgb, var(--warning) 20%, transparent);
  color: var(--warning);
}

.status-closed {
  background: var(--bg-tertiary);
  color: var(--text-tertiary);
}

.detail-footer {
  display: flex;
  justify-content: flex-end;
  gap: 12px;
  padding: 16px 24px;
  border-top: 1px solid var(--border-color);
}

.terminate-btn {
  min-width: 100px;
}

.cancel-btn {
  min-width: 80px;
}

/* 动画效果 */
.detail-fade-enter-active,
.detail-fade-leave-active {
  transition: opacity 0.3s ease;
}

.detail-fade-enter-from,
.detail-fade-leave-to {
  opacity: 0;
}

.detail-scale-enter-active,
.detail-scale-leave-active {
  transition: transform 0.3s ease, opacity 0.3s ease;
}

.detail-scale-enter-from,
.detail-scale-leave-to {
  transform: scale(0.9);
  opacity: 0;
}

/* 滚动条样式 */
.detail-content::-webkit-scrollbar {
  width: 8px;
}

.detail-content::-webkit-scrollbar-track {
  background: var(--bg-primary);
}

.detail-content::-webkit-scrollbar-thumb {
  background: var(--border-color);
  border-radius: 4px;
}

.detail-content::-webkit-scrollbar-thumb:hover {
  background: var(--text-tertiary);
}
</style>