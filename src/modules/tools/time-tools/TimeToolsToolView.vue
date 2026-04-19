<template>
  <BasePanel class="panel time-tools-panel">
    <div class="time-tools-container">
      <div class="tools-header">
        <h3>{{ toolName }}</h3>
      </div>
      
      <div class="current-time">
        <h4>当前时间</h4>
        <div class="time-display">
          <div class="time-value">{{ currentDateTime }}</div>
          <div class="time-zone">{{ currentTimezone }}</div>
        </div>
      </div>
      
      <div class="tool-section">
        <h4>时间戳转换</h4>
        <div class="converter-section">
          <div class="input-group">
            <label>Unix时间戳</label>
            <BaseInput v-model="timestamp" placeholder="输入时间戳..." />
            <BaseButton size="sm" @click="convertToDateTime">转换为日期时间</BaseButton>
          </div>
          <div class="input-group">
            <label>日期时间</label>
            <BaseInput v-model="dateTime" placeholder="YYYY-MM-DD HH:MM:SS" />
            <BaseButton size="sm" @click="convertToTimestamp">转换为时间戳</BaseButton>
          </div>
        </div>
      </div>
      
      <div class="tool-section">
        <h4>时间差计算</h4>
        <div class="time-diff-section">
          <div class="input-group">
            <label>开始时间</label>
            <BaseInput v-model="startTime" placeholder="YYYY-MM-DD HH:MM:SS" />
          </div>
          <div class="input-group">
            <label>结束时间</label>
            <BaseInput v-model="endTime" placeholder="YYYY-MM-DD HH:MM:SS" />
          </div>
          <BaseButton @click="calculateTimeDiff">计算时间差</BaseButton>
          <div class="time-diff-result" v-if="timeDiffResult">
            {{ timeDiffResult }}
          </div>
        </div>
      </div>
    </div>
  </BasePanel>
</template>

<script>
import BaseInput from '../../../components/base/BaseInput.vue';
import BaseButton from '../../../components/base/BaseButton.vue';

export default {
  name: 'TimeToolsToolView',
  components: {
    BaseInput,
    BaseButton
  },
  props: {
    toolName: {
      type: String,
      default: '时间工具'
    },
    toolDescription: {
      type: String,
      default: '时间转换和计算'
    }
  },
  data() {
    return {
      currentDateTime: '',
      currentTimezone: '',
      timestamp: '',
      dateTime: '',
      startTime: '',
      endTime: '',
      timeDiffResult: ''
    };
  },
  mounted() {
    this.updateCurrentTime();
    setInterval(this.updateCurrentTime, 1000);
  },
  methods: {
    updateCurrentTime() {
      const now = new Date();
      this.currentDateTime = now.toLocaleString();
      this.currentTimezone = Intl.DateTimeFormat().resolvedOptions().timeZone;
    },
    convertToDateTime() {
      const ts = parseInt(this.timestamp);
      if (!isNaN(ts)) {
        const date = new Date(ts * 1000);
        this.dateTime = date.toLocaleString();
      } else {
        alert('请输入有效的时间戳');
      }
    },
    convertToTimestamp() {
      const date = new Date(this.dateTime);
      if (!isNaN(date.getTime())) {
        this.timestamp = Math.floor(date.getTime() / 1000).toString();
      } else {
        alert('请输入有效的日期时间');
      }
    },
    calculateTimeDiff() {
      const start = new Date(this.startTime);
      const end = new Date(this.endTime);
      
      if (isNaN(start.getTime()) || isNaN(end.getTime())) {
        alert('请输入有效的开始和结束时间');
        return;
      }
      
      const diffMs = Math.abs(end.getTime() - start.getTime());
      const diffDays = Math.floor(diffMs / (1000 * 60 * 60 * 24));
      const diffHours = Math.floor((diffMs % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60));
      const diffMinutes = Math.floor((diffMs % (1000 * 60 * 60)) / (1000 * 60));
      const diffSeconds = Math.floor((diffMs % (1000 * 60)) / 1000);
      
      this.timeDiffResult = `${diffDays} 天 ${diffHours} 小时 ${diffMinutes} 分钟 ${diffSeconds} 秒`;
    }
  }
};
</script>

<style scoped>
.time-tools-panel {
  height: 100%;
}

.time-tools-container {
  padding: 20px;
  height: 100%;
  display: flex;
  flex-direction: column;
  gap: 20px;
}

.tools-header h3 {
  margin: 0;
  font-size: 18px;
  font-weight: 600;
}

.current-time {
  background-color: var(--bg-secondary);
  padding: 16px;
  border-radius: var(--radius-md);
}

.current-time h4 {
  margin: 0 0 12px 0;
  font-size: 14px;
  font-weight: 500;
  color: var(--text-secondary);
}

.time-display {
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.time-value {
  font-size: 18px;
  font-weight: 600;
  color: var(--text-primary);
}

.time-zone {
  font-size: 14px;
  color: var(--text-secondary);
}

.tool-section {
  flex: 1;
  min-height: 150px;
}

.tool-section h4 {
  margin: 0 0 12px 0;
  font-size: 14px;
  font-weight: 500;
  color: var(--text-secondary);
}

.converter-section,
.time-diff-section {
  display: flex;
  flex-direction: column;
  gap: 12px;
}

.input-group {
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.input-group label {
  font-size: 14px;
  font-weight: 500;
  color: var(--text-primary);
}

.time-diff-result {
  margin-top: 12px;
  padding: 12px;
  background-color: var(--bg-secondary);
  border-radius: var(--radius-md);
  font-size: 14px;
  color: var(--text-primary);
  font-weight: 500;
}
</style>