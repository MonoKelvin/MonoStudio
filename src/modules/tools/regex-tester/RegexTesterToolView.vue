<template>
  <BasePanel class="panel regex-tester-panel">
    <div class="regex-tester-container">
      <div class="tester-header">
        <h3>{{ toolName }}</h3>
      </div>
      
      <div class="regex-input-section">
        <label>正则表达式</label>
        <div class="regex-input-group">
          <BaseInput v-model="regexPattern" placeholder="输入正则表达式..." @input="testRegex" />
          <div class="regex-flags">
            <label class="flag-item">
              <input type="checkbox" v-model="flags.global" @change="testRegex" />
              <span>g (全局)</span>
            </label>
            <label class="flag-item">
              <input type="checkbox" v-model="flags.ignoreCase" @change="testRegex" />
              <span>i (忽略大小写)</span>
            </label>
            <label class="flag-item">
              <input type="checkbox" v-model="flags.multiline" @change="testRegex" />
              <span>m (多行)</span>
            </label>
          </div>
        </div>
      </div>
      
      <div class="test-input-section">
        <label>测试文本</label>
        <BaseTextarea v-model="testText" placeholder="输入要测试的文本..." @input="testRegex" :rows="6" />
      </div>
      
      <div class="test-results" v-if="testResults.length > 0">
        <div class="results-header">
          <h4>匹配结果</h4>
          <span class="match-count">{{ testResults.length }} 个匹配</span>
        </div>
        <div class="results-list">
          <div v-for="(result, index) in testResults" :key="index" class="result-item">
            <div class="result-index">匹配 {{ index + 1 }}</div>
            <div class="result-text">{{ result }}</div>
          </div>
        </div>
      </div>
      
      <div class="no-results" v-else-if="regexPattern && testText">
        <p>没有找到匹配项</p>
      </div>
    </div>
  </BasePanel>
</template>

<script>
import BaseInput from '../../../components/base/BaseInput.vue';
import BaseTextarea from '../../../components/base/BaseTextarea.vue';

export default {
  name: 'RegexTesterToolView',
  components: {
    BaseInput,
    BaseTextarea
  },
  props: {
    toolName: {
      type: String,
      default: '正则表达式测试'
    },
    toolDescription: {
      type: String,
      default: '测试和验证正则表达式'
    }
  },
  data() {
    return {
      regexPattern: '',
      testText: '',
      flags: {
        global: true,
        ignoreCase: false,
        multiline: false
      },
      testResults: []
    };
  },
  methods: {
    testRegex() {
      if (!this.regexPattern || !this.testText) {
        this.testResults = [];
        return;
      }
      
      try {
        // 构建正则表达式标志
        let flagString = '';
        if (this.flags.global) flagString += 'g';
        if (this.flags.ignoreCase) flagString += 'i';
        if (this.flags.multiline) flagString += 'm';
        
        // 创建正则表达式对象
        const regex = new RegExp(this.regexPattern, flagString);
        
        // 执行匹配
        const results = [];
        let match;
        
        if (this.flags.global) {
          while ((match = regex.exec(this.testText)) !== null) {
            results.push(match[0]);
            // 防止无限循环
            if (!regex.global) break;
          }
        } else {
          match = regex.exec(this.testText);
          if (match) {
            results.push(match[0]);
          }
        }
        
        this.testResults = results;
      } catch (error) {
        console.error('正则表达式错误:', error);
        this.testResults = [];
      }
    }
  }
};
</script>

<style scoped>
.regex-tester-panel {
  height: 100%;
}

.regex-tester-container {
  padding: 20px;
  height: 100%;
  display: flex;
  flex-direction: column;
  gap: 20px;
}

.tester-header h3 {
  margin: 0;
  font-size: 18px;
  font-weight: 600;
}

.regex-input-section,
.test-input-section {
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.regex-input-section label,
.test-input-section label {
  font-weight: 500;
  color: var(--text-primary);
}

.regex-input-group {
  display: flex;
  flex-direction: column;
  gap: 12px;
}

.regex-flags {
  display: flex;
  gap: 20px;
  flex-wrap: wrap;
}

.flag-item {
  display: flex;
  align-items: center;
  gap: 6px;
  cursor: pointer;
  font-size: 14px;
}

.flag-item input[type="checkbox"] {
  cursor: pointer;
}

.test-results {
  flex: 1;
  padding: 16px;
  background-color: var(--bg-secondary);
  border-radius: var(--radius-md);
  overflow-y: auto;
}

.results-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 12px;
}

.results-header h4 {
  margin: 0;
  font-size: 14px;
  font-weight: 500;
  color: var(--text-secondary);
}

.match-count {
  font-size: 14px;
  color: var(--text-secondary);
  background-color: var(--bg-primary);
  padding: 4px 8px;
  border-radius: var(--radius-sm);
}

.results-list {
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.result-item {
  padding: 12px;
  background-color: var(--bg-primary);
  border-radius: var(--radius-md);
  display: flex;
  gap: 12px;
}

.result-index {
  width: 60px;
  font-size: 14px;
  font-weight: 500;
  color: var(--text-secondary);
  flex-shrink: 0;
}

.result-text {
  flex: 1;
  font-size: 14px;
  color: var(--text-primary);
  word-break: break-all;
}

.no-results {
  flex: 1;
  padding: 40px;
  background-color: var(--bg-secondary);
  border-radius: var(--radius-md);
  display: flex;
  align-items: center;
  justify-content: center;
  color: var(--text-secondary);
}
</style>