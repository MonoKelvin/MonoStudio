<template>
  <BasePanel class="panel json-formatter-panel">
    <div class="json-formatter-container">
      <div class="formatter-header">
        <h3>{{ toolName }}</h3>
        <div class="action-buttons">
          <BaseButton size="sm" @click="format">格式化</BaseButton>
          <BaseButton size="sm" @click="minify">压缩</BaseButton>
          <BaseButton size="sm" @click="validate">验证</BaseButton>
          <BaseButton size="sm" @click="clear">清空</BaseButton>
        </div>
      </div>
      
      <div class="input-section">
        <label>输入 JSON</label>
        <BaseTextarea 
          v-model="inputJson" 
          placeholder="请输入要格式化的 JSON 数据..."
          :rows="6"
        />
      </div>
      
      <div class="output-section">
        <div class="output-header">
          <label>输出结果</label>
          <BaseButton size="sm" @click="copyOutput" :disabled="!outputJson">
            复制
          </BaseButton>
        </div>
        <BaseTextarea 
          v-model="outputJson" 
          placeholder="格式化或压缩结果将显示在这里..."
          :rows="6"
          readonly
        />
        <div v-if="errorMessage" class="error-message">
          {{ errorMessage }}
        </div>
      </div>
    </div>
  </BasePanel>
</template>

<script>
import BaseTextarea from '../../../components/base/BaseTextarea.vue';
import BaseButton from '../../../components/base/BaseButton.vue';

export default {
  name: 'JsonFormatterToolView',
  components: {
    BaseTextarea,
    BaseButton
  },
  props: {
    toolName: {
      type: String,
      default: 'JSON 格式化'
    },
    toolDescription: {
      type: String,
      default: '格式化和验证 JSON 数据'
    }
  },
  data() {
    return {
      inputJson: '',
      outputJson: '',
      errorMessage: ''
    };
  },
  methods: {
    format() {
      if (!this.inputJson) {
        this.outputJson = '';
        this.errorMessage = '';
        return;
      }
      try {
        const parsed = JSON.parse(this.inputJson);
        this.outputJson = JSON.stringify(parsed, null, 2);
        this.errorMessage = '';
      } catch (error) {
        this.outputJson = '';
        this.errorMessage = 'JSON 格式错误: ' + error.message;
      }
    },
    minify() {
      if (!this.inputJson) {
        this.outputJson = '';
        this.errorMessage = '';
        return;
      }
      try {
        const parsed = JSON.parse(this.inputJson);
        this.outputJson = JSON.stringify(parsed);
        this.errorMessage = '';
      } catch (error) {
        this.outputJson = '';
        this.errorMessage = 'JSON 格式错误: ' + error.message;
      }
    },
    validate() {
      if (!this.inputJson) {
        this.outputJson = '';
        this.errorMessage = '';
        return;
      }
      try {
        JSON.parse(this.inputJson);
        this.outputJson = 'JSON 格式正确';
        this.errorMessage = '';
      } catch (error) {
        this.outputJson = '';
        this.errorMessage = 'JSON 格式错误: ' + error.message;
      }
    },
    clear() {
      this.inputJson = '';
      this.outputJson = '';
      this.errorMessage = '';
    },
    copyOutput() {
      if (this.outputJson) {
        navigator.clipboard.writeText(this.outputJson).then(() => {
          alert('已复制到剪贴板');
        }).catch(err => {
          console.error('复制失败:', err);
        });
      }
    }
  }
};
</script>

<style scoped>
.json-formatter-panel {
  height: 100%;
}

.json-formatter-container {
  padding: 20px;
  height: 100%;
  display: flex;
  flex-direction: column;
}

.formatter-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 20px;
}

.formatter-header h3 {
  margin: 0;
  font-size: 18px;
  font-weight: 600;
}

.action-buttons {
  display: flex;
  gap: 10px;
}

.input-section,
.output-section {
  margin-bottom: 20px;
  flex: 1;
}

.input-section label,
.output-section label {
  display: block;
  margin-bottom: 8px;
  font-weight: 500;
  color: var(--text-primary);
}

.output-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 8px;
}

.output-header label {
  margin: 0;
}

.error-message {
  margin-top: 8px;
  padding: 8px 12px;
  background-color: rgba(239, 68, 68, 0.1);
  border: 1px solid rgba(239, 68, 68, 0.3);
  border-radius: var(--radius-md);
  color: #dc2626;
  font-size: 14px;
}

.BaseTextarea {
  width: 100%;
  box-sizing: border-box;
}
</style>