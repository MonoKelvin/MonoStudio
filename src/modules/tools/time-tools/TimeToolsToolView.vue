<template>
  <BasePanel class="panel time-tools-panel">
    <div class="time-tools-container">
      <!-- 当前时间 -->
      <div class="current-time">
        <div class="time-display">
          <div class="time-value">当前时间: {{ currentDateTime }}</div>
          <div class="time-zone">当前时区: {{ currentTimezone }}</div>
        </div>
      </div>
      
      <!-- 时间戳转换 -->
      <div class="tool-row">
        <div class="tool-card">
          <h4>时间戳转换</h4>
          <div class="converter-section">
            <div class="input-row">
              <div class="input-group">
                <label>Unix时间戳</label>
                <BaseInput v-model="timestamp" placeholder="输入时间戳..." @input="handleTimestampInput" />
              </div>
              <div class="input-group">
                <label>日期时间</label>
                <BaseInput v-model="dateTime" placeholder="YYYY-MM-DD HH:MM:SS" @input="handleDateTimeInput" />
              </div>
            </div>
          </div>
        </div>
      </div>
      
      <!-- 时间差计算 -->
      <div class="tool-row">
        <div class="tool-card">
          <h4>时间差计算</h4>
          <div class="input-row">
            <div class="input-group">
              <label>开始时间</label>
              <BaseInput v-model="startTime" placeholder="YYYY-MM-DD HH:MM:SS" />
            </div>
            <div class="input-group">
              <label>结束时间</label>
              <BaseInput v-model="endTime" placeholder="YYYY-MM-DD HH:MM:SS" />
            </div>
            <BaseButton @click="calculateTimeDiff" class="calc-btn" variant="primary">计算</BaseButton>
          </div>
          <div class="result" v-if="timeDiffResult">
            结果: {{ timeDiffResult }}
          </div>
        </div>
      </div>
      
      <!-- 日期计算器 -->
      <div class="tool-row">
        <div class="tool-card">
          <h4>日期计算器</h4>
          <div class="input-row">
            <div class="input-group">
              <label>基础日期</label>
              <BaseInput v-model="baseDate" placeholder="YYYY-MM-DD" />
            </div>
            <div class="input-group">
              <label>天数</label>
              <BaseSpinbox v-model="daysToAdd" :min="0" :max="365" />
            </div>
            <BaseButton @click="calculateDate" class="calc-btn" variant="primary">计算</BaseButton>
          </div>
          <div class="result" v-if="calculatedDate">
            结果: {{ calculatedDate }}
          </div>
        </div>
      </div>
      
      <!-- 世界时区 -->
      <div class="tool-row">
        <div class="tool-card">
          <h4>世界时区</h4>
          <div class="timezone-section">
            <div class="timezone-grid">
              <div class="timezone-item" v-for="tz in worldTimezones" :key="tz.name">
                <span class="timezone-name">{{ tz.name }}</span>
                <span class="timezone-time">{{ tz.time }}</span>
              </div>
            </div>
          </div>
        </div>
      </div>
    </div>
  </BasePanel>
</template>

<script setup>
import { ref, onMounted, onUnmounted, computed, getCurrentInstance } from 'vue';
import BaseInput from '../../../components/base/BaseInput.vue';
import BaseButton from '../../../components/base/BaseButton.vue';
import BasePanel from '../../../components/base/BasePanel.vue';
import BaseSpinbox from '../../../components/base/BaseSpinbox.vue';

const { proxy } = getCurrentInstance();
const $dialog = proxy.$dialog;

const props = defineProps({
  toolName: {
    type: String,
    default: '时间工具'
  },
  toolDescription: {
    type: String,
    default: '时间转换和计算'
  }
});

const currentDateTime = ref('');
const currentTimezone = ref('');
const timestamp = ref('');
const dateTime = ref('');
const startTime = ref('');
const endTime = ref('');
const timeDiffResult = ref('');
const baseDate = ref('');
const daysToAdd = ref('');
const calculatedDate = ref('');

let updateInterval = null;
const tick = ref(0);

const worldTimezones = computed(() => {
  tick.value;
  const now = new Date();
  return [
    { name: '纽约', time: now.toLocaleString('zh-CN', { timeZone: 'America/New_York' }) },
    { name: '伦敦', time: now.toLocaleString('zh-CN', { timeZone: 'Europe/London' }) },
    { name: '巴黎', time: now.toLocaleString('zh-CN', { timeZone: 'Europe/Paris' }) },
    { name: '东京', time: now.toLocaleString('zh-CN', { timeZone: 'Asia/Tokyo' }) },
    { name: '悉尼', time: now.toLocaleString('zh-CN', { timeZone: 'Australia/Sydney' }) },
    { name: '北京', time: now.toLocaleString('zh-CN', { timeZone: 'Asia/Shanghai' }) }
  ];
});

const updateCurrentTime = () => {
  const now = new Date();
  currentDateTime.value = now.toLocaleString();
  currentTimezone.value = Intl.DateTimeFormat().resolvedOptions().timeZone;
  tick.value++;
};

const convertToDateTime = () => {
  const ts = parseInt(timestamp.value);
  if (!isNaN(ts)) {
    const date = new Date(ts * 1000);
    dateTime.value = date.toLocaleString();
  } else {
    $dialog.alert('请输入有效的时间戳', '输入错误', 'warning');
  }
};

const convertToTimestamp = () => {
  const date = new Date(dateTime.value);
  if (!isNaN(date.getTime())) {
    timestamp.value = Math.floor(date.getTime() / 1000).toString();
  } else {
    $dialog.alert('请输入有效的日期时间', '输入错误', 'warning');
  }
};

const calculateTimeDiff = () => {
  const start = new Date(startTime.value);
  const end = new Date(endTime.value);
  
  if (isNaN(start.getTime()) || isNaN(end.getTime())) {
    $dialog.alert('请输入有效的开始和结束时间', '输入错误', 'warning');
    return;
  }
  
  const diffMs = Math.abs(end.getTime() - start.getTime());
  const diffDays = Math.floor(diffMs / (1000 * 60 * 60 * 24));
  const diffHours = Math.floor((diffMs % (1000 * 60 * 60 * 24)) / (1000 * 60 * 60));
  const diffMinutes = Math.floor((diffMs % (1000 * 60 * 60)) / (1000 * 60));
  const diffSeconds = Math.floor((diffMs % (1000 * 60)) / 1000);
  
  timeDiffResult.value = `${diffDays} 天 ${diffHours} 小时 ${diffMinutes} 分钟 ${diffSeconds} 秒`;
};

const calculateDate = () => {
  const date = new Date(baseDate.value);
  const days = parseInt(daysToAdd.value);
  
  if (isNaN(date.getTime()) || isNaN(days)) {
    $dialog.alert('请输入有效的日期和天数', '输入错误', 'warning');
    return;
  }
  
  date.setDate(date.getDate() + days);
  calculatedDate.value = date.toLocaleDateString('zh-CN');
};

const handleTimestampInput = () => {
  const ts = parseInt(timestamp.value);
  if (!isNaN(ts)) {
    const date = new Date(ts * 1000);
    dateTime.value = date.toLocaleString();
  }
};

const handleDateTimeInput = () => {
  const date = new Date(dateTime.value);
  if (!isNaN(date.getTime())) {
    timestamp.value = Math.floor(date.getTime() / 1000).toString();
  }
};

const copyToClipboard = (text) => {
  navigator.clipboard.writeText(text.value || text)
    .then(() => {
      $dialog.alert('已复制到剪贴板', '复制成功', 'success');
    })
    .catch(err => {
      console.error('复制失败:', err);
      $dialog.alert('复制失败，请手动复制', '复制失败', 'error');
    });
};

onMounted(() => {
  updateCurrentTime();
  updateInterval = setInterval(updateCurrentTime, 1000);
});

onUnmounted(() => {
  if (updateInterval) {
    clearInterval(updateInterval);
  }
});
</script>

<style scoped>
.time-tools-panel {
  height: 100%;
}

.time-tools-container {
  padding: var(--spacing-sm);
  height: 100%;
  display: flex;
  flex-direction: column;
  gap: var(--spacing-sm);
  overflow-y: auto;
  box-sizing: border-box;
}

.current-time {
  background: var(--bg-elevated);
  padding: var(--spacing-md);
  border-radius: var(--radius-md);
  box-shadow: var(--shadow-card);
  margin-bottom: var(--spacing-xs);
}

.time-display {
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.time-value {
  font-size: var(--font-size-xl);
  font-weight: 600;
  color: var(--text-primary);
  font-family: var(--font-mono);
}

.time-zone {
  font-size: var(--font-size-sm);
  color: var(--text-muted);
}

.tool-row {
  flex-shrink: 0;
}

.tool-card {
  background: var(--bg-elevated);
  padding: var(--spacing-md);
  border-radius: var(--radius-md);
  box-shadow: var(--shadow-card);
  transition: all 0.2s ease;
}

.tool-card:hover {
  box-shadow: var(--shadow-card-hover);
}

.tool-card h4 {
  margin: 0 0 var(--spacing-sm) 0;
  font-size: var(--font-size-xl);
  font-weight: 500;
  color: var(--text-secondary);
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.input-row {
  display: flex;
  gap: var(--spacing-sm);
  margin-bottom: var(--spacing-sm);
}

.input-group {
  flex: 1;
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.input-group label {
  font-size: var(--font-size-md);
  color: var(--text-muted);
  text-transform: uppercase;
  letter-spacing: 0.5px;
}

.input-group .BaseInput {
  width: 100%;
}

.input-group :deep(.BaseSpinbox) {
  width: 100%;
}

.input-group :deep(.ui-spinbox) {
  width: 100%;
  height: 34px;
}

.calc-btn {
  align-self: flex-end;
  margin-bottom: 0;
  white-space: nowrap;
}

.result {
  margin-top: var(--spacing-xs);
  padding: var(--spacing-sm);
  background: var(--bg-secondary);
  border-radius: var(--radius-sm);
  font-size: var(--font-size-md);
  color: var(--accent-primary);

  
  font-weight: 500;
  font-family: var(--font-mono);
  border: 1px solid var(--border);
}

.timezone-section {
  margin-top: var(--spacing-xs);
}

.timezone-grid {
  display: grid;
  grid-template-columns: repeat(2, 1fr);
  gap: var(--spacing-xs);
}

.timezone-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: var(--spacing-md) var(--spacing-md);
  background: var(--background-tertiary);
  border-radius: var(--radius-sm);
  border: 1px solid var(--border);
  transition: all 0.2s ease;
}

.timezone-item:hover {
  background: var(--background-hover);
  border-color: var(--accent-primary);
}

.timezone-name {
  font-size: var(--font-size-md);
  color: var(--text-secondary);
}

.timezone-time {
  font-size: var(--font-size-md);
  color: var(--accent-color);
  font-family: var(--font-mono);
  font-weight: 600;
}

.converter-section,
.time-diff-section,
.date-calculator-section {
  display: flex;
  flex-direction: column;
  gap: var(--spacing-sm);
}

@media (max-width: 768px) {
  .time-tools-container {
    padding: var(--spacing-xs);
    gap: var(--spacing-xs);
  }
  
  .current-time,
  .tool-card {
    padding: var(--spacing-sm);
  }
  
  .time-value {
    font-size: var(--font-size-lg);
  }
  
  .input-row {
    flex-direction: column;
  }
  
  .timezone-grid {
    grid-template-columns: repeat(2, 1fr);
  }
}
</style>