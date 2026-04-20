<template>
  <BasePanel class="panel process-manager-panel">
    <div class="process-manager-container">
      <div class="process-header">
        <h3>{{ toolName }}</h3>
        <BaseButton @click="refreshProcesses">刷新进程</BaseButton>
      </div>
      
      <div class="process-search">
        <BaseInput v-model="searchQuery" placeholder="搜索进程..." />
      </div>
      
      <div class="process-list" v-if="processes.length > 0">
        <div class="process-header-row">
          <div class="process-name-header">进程名称</div>
          <div class="process-pid-header">PID</div>
          <div class="process-memory-header">内存</div>
          <div class="process-cpu-header">CPU</div>
          <div class="process-actions-header">操作</div>
        </div>
        <div v-for="process in filteredProcesses" :key="process.pid" class="process-item">
          <div class="process-name">{{ process.name }}</div>
          <div class="process-pid">{{ process.pid }}</div>
          <div class="process-memory">{{ formatMemory(process.memory) }}</div>
          <div class="process-cpu">{{ process.cpu }}%</div>
          <div class="process-actions">
            <BaseButton size="sm" @click="killProcess(process.pid)">结束进程</BaseButton>
          </div>
        </div>
      </div>
      
      <div class="no-processes" v-else>
        <div class="no-processes-icon">⚙️</div>
        <p>没有找到进程</p>
        <p>点击"刷新进程"按钮获取进程列表</p>
      </div>
    </div>
  </BasePanel>
</template>

<script>
import BaseButton from '../../../components/base/BaseButton.vue';
import BaseInput from '../../../components/base/BaseInput.vue';

export default {
  name: 'ProcessManagerToolView',
  components: {
    BaseButton,
    BaseInput
  },
  props: {
    toolName: {
      type: String,
      default: '进程管理'
    },
    toolDescription: {
      type: String,
      default: '查看和管理系统进程'
    }
  },
  data() {
    return {
      processes: [],
      searchQuery: ''
    };
  },
  computed: {
    filteredProcesses() {
      if (!this.searchQuery) {
        return this.processes;
      }
      return this.processes.filter(process => 
        process.name.toLowerCase().includes(this.searchQuery.toLowerCase())
      );
    }
  },
  mounted() {
    this.refreshProcesses();
  },
  methods: {
    refreshProcesses() {
      // 模拟进程数据
      this.processes = [
        { pid: 1234, name: 'explorer.exe', memory: 123456, cpu: 2.5 },
        { pid: 5678, name: 'chrome.exe', memory: 456789, cpu: 12.3 },
        { pid: 9012, name: 'node.exe', memory: 789012, cpu: 8.7 },
        { pid: 3456, name: 'svchost.exe', memory: 345678, cpu: 1.2 },
        { pid: 7890, name: 'taskmgr.exe', memory: 234567, cpu: 0.5 }
      ];
    },
    killProcess(pid) {
      if (confirm(`确定要结束 PID ${pid} 的进程吗？`)) {
        this.processes = this.processes.filter(process => process.pid !== pid);
      }
    },
    formatMemory(bytes) {
      if (bytes < 1024) return bytes + ' B';
      if (bytes < 1024 * 1024) return (bytes / 1024).toFixed(2) + ' KB';
      return (bytes / (1024 * 1024)).toFixed(2) + ' MB';
    }
  }
};
</script>

<style scoped>
.process-manager-panel {
  height: 100%;
}

.process-manager-container {
  padding: 20px;
  height: 100%;
  display: flex;
  flex-direction: column;
}

.process-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 20px;
}

.process-header h3 {
  margin: 0;
  font-size: 18px;
  font-weight: 600;
}

.process-search {
  margin-bottom: 20px;
}

.process-list {
  flex: 1;
  background-color: var(--bg-secondary);
  border-radius: var(--radius-md);
  overflow: hidden;
  display: flex;
  flex-direction: column;
}

.process-header-row {
  display: grid;
  grid-template-columns: 1fr 100px 120px 80px 120px;
  padding: 12px 16px;
  background-color: var(--bg-tertiary);
  border-bottom: 1px solid var(--border-color);
  font-weight: 500;
  font-size: 14px;
  color: var(--text-secondary);
}

.process-item {
  display: grid;
  grid-template-columns: 1fr 100px 120px 80px 120px;
  padding: 12px 16px;
  border-bottom: 1px solid var(--border-color);
  align-items: center;
  transition: background-color 0.2s ease;
}

.process-item:hover {
  background-color: var(--bg-tertiary);
}

.process-name {
  font-size: 14px;
  color: var(--text-primary);
  font-weight: 500;
}

.process-pid,
.process-memory,
.process-cpu {
  font-size: 14px;
  color: var(--text-secondary);
}

.process-actions {
  display: flex;
  gap: 8px;
}

.no-processes {
  flex: 1;
  display: flex;
  flex-direction: column;
  align-items: center;
  justify-content: center;
  text-align: center;
  color: var(--text-secondary);
  gap: 16px;
}

.no-processes-icon {
  font-size: 48px;
}
</style>