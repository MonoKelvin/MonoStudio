<template>
  <BasePanel class="panel password-generator-panel">
    <div class="password-generator-container">
      <div class="generator-header">
        <h3>{{ toolName }}</h3>
        <BaseButton @click="generatePassword">生成密码</BaseButton>
      </div>
      
      <div class="password-result">
        <div class="result-header">
          <label>生成的密码</label>
          <BaseButton size="sm" @click="copyPassword" :disabled="!generatedPassword">
            复制
          </BaseButton>
        </div>
        <BaseInput 
          v-model="generatedPassword" 
          readonly 
          :class="{ 'password-strong': passwordStrength === 'strong', 'password-medium': passwordStrength === 'medium', 'password-weak': passwordStrength === 'weak' }"
        />
        <div class="password-strength">
          <span>密码强度: </span>
          <span :class="'strength-' + passwordStrength">
            {{ passwordStrengthText }}
          </span>
        </div>
      </div>
      
      <div class="generator-options">
        <div class="option-group">
          <label>密码长度</label>
          <BaseSpinbox v-model="passwordLength" :min="6" :max="50" />
        </div>
        
        <div class="option-group">
          <label>字符类型</label>
          <div class="checkbox-group">
            <BaseCheckbox v-model="options.uppercase">大写字母 (A-Z)</BaseCheckbox>
            <BaseCheckbox v-model="options.lowercase">小写字母 (a-z)</BaseCheckbox>
            <BaseCheckbox v-model="options.numbers">数字 (0-9)</BaseCheckbox>
            <BaseCheckbox v-model="options.symbols">特殊字符 (!@#$%^&*)</BaseCheckbox>
          </div>
        </div>
      </div>
    </div>
  </BasePanel>
</template>

<script>
import BaseInput from '../../../components/base/BaseInput.vue';
import BaseButton from '../../../components/base/BaseButton.vue';
import BaseCheckbox from '../../../components/base/BaseCheckbox.vue';
import BaseSpinbox from '../../../components/base/BaseSpinbox.vue';

export default {
  name: 'PasswordGeneratorToolView',
  components: {
    BaseInput,
    BaseButton,
    BaseCheckbox,
    BaseSpinbox
  },
  props: {
    toolName: {
      type: String,
      default: '密码生成器'
    },
    toolDescription: {
      type: String,
      default: '生成安全的随机密码'
    }
  },
  data() {
    return {
      generatedPassword: '',
      passwordLength: 12,
      options: {
        uppercase: true,
        lowercase: true,
        numbers: true,
        symbols: true
      }
    };
  },
  computed: {
    passwordStrength() {
      if (!this.generatedPassword) return 'weak';
      const length = this.generatedPassword.length;
      let strength = 0;
      
      if (length >= 8) strength += 1;
      if (length >= 12) strength += 1;
      if (this.options.uppercase && /[A-Z]/.test(this.generatedPassword)) strength += 1;
      if (this.options.lowercase && /[a-z]/.test(this.generatedPassword)) strength += 1;
      if (this.options.numbers && /[0-9]/.test(this.generatedPassword)) strength += 1;
      if (this.options.symbols && /[!@#$%^&*(),.?":{}|<>]/.test(this.generatedPassword)) strength += 1;
      
      if (strength >= 5) return 'strong';
      if (strength >= 3) return 'medium';
      return 'weak';
    },
    passwordStrengthText() {
      switch (this.passwordStrength) {
        case 'strong': return '强';
        case 'medium': return '中';
        case 'weak': return '弱';
        default: return '弱';
      }
    }
  },
  methods: {
    generatePassword() {
      const charset = {
        uppercase: 'ABCDEFGHIJKLMNOPQRSTUVWXYZ',
        lowercase: 'abcdefghijklmnopqrstuvwxyz',
        numbers: '0123456789',
        symbols: '!@#$%^&*(),.?":{}|<>'
      };
      
      let availableChars = '';
      for (const [key, value] of Object.entries(this.options)) {
        if (value) {
          availableChars += charset[key];
        }
      }
      
      if (!availableChars) {
        alert('请至少选择一种字符类型');
        return;
      }
      
      let password = '';
      for (let i = 0; i < this.passwordLength; i++) {
        const randomIndex = Math.floor(Math.random() * availableChars.length);
        password += availableChars[randomIndex];
      }
      
      this.generatedPassword = password;
    },
    copyPassword() {
      if (this.generatedPassword) {
        navigator.clipboard.writeText(this.generatedPassword).then(() => {
          alert('已复制到剪贴板');
        }).catch(err => {
          console.error('复制失败:', err);
        });
      }
    }
  },
  mounted() {
    this.generatePassword();
  }
};
</script>

<style scoped>
.password-generator-panel {
  height: 100%;
}

.password-generator-container {
  padding: 20px;
  height: 100%;
  display: flex;
  flex-direction: column;
}

.generator-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 20px;
}

.generator-header h3 {
  margin: 0;
  font-size: 18px;
  font-weight: 600;
}

.password-result {
  margin-bottom: 20px;
}

.result-header {
  display: flex;
  justify-content: space-between;
  align-items: center;
  margin-bottom: 8px;
}

.result-header label {
  font-weight: 500;
  color: var(--text-primary);
}

.password-strength {
  margin-top: 8px;
  font-size: 14px;
  color: var(--text-secondary);
}

.strength-strong {
  color: #10b981;
  font-weight: 500;
}

.strength-medium {
  color: #f59e0b;
  font-weight: 500;
}

.strength-weak {
  color: #ef4444;
  font-weight: 500;
}

.generator-options {
  flex: 1;
}

.option-group {
  margin-bottom: 20px;
}

.option-group label {
  display: block;
  margin-bottom: 8px;
  font-weight: 500;
  color: var(--text-primary);
}

.checkbox-group {
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.password-strong {
  border-color: #10b981 !important;
}

.password-medium {
  border-color: #f59e0b !important;
}

.password-weak {
  border-color: #ef4444 !important;
}
</style>