<template>
  <BasePanel class="panel password-gate-panel">
    <div class="gate-content">
      <div class="gate-icon">
        <img v-if="!isUnlocked" :src="lockIcon" alt="" />
        <img v-else :src="unlockIcon" alt="" />
      </div>
      <h3 v-if="!hasPassword && !isUnlocked">设置访问密码</h3>
      <h3 v-else-if="!isUnlocked">请输入密码</h3>
      <h3 v-else>已解锁</h3>

      <p class="gate-description" v-if="!hasPassword && !isUnlocked">
        首次使用个人生活模块，请设置访问密码
      </p>
      <p class="gate-description" v-else-if="!isUnlocked">
        输入密码以访问个人生活模块
      </p>

      <form v-if="!isUnlocked" @submit.prevent="handleSubmit" class="gate-form">
        <BaseInput
          v-if="!hasPassword"
          v-model="newPassword"
          type="password"
          placeholder="设置密码（至少4位）"
          class="gate-input"
        />
        <BaseInput
          v-else
          v-model="password"
          type="password"
          placeholder="请输入密码"
          class="gate-input"
        />
        <p v-if="error" class="gate-error">{{ error }}</p>
        <BaseButton type="submit" variant="primary" class="gate-submit">
          {{ hasPassword ? '解锁' : '设置密码' }}
        </BaseButton>
      </form>

      <BaseButton v-else variant="primary" @click="handleLock" class="gate-lock-btn">
        重新锁定
      </BaseButton>
    </div>
  </BasePanel>
</template>

<script>
import lockIcon from '../assets/icons/lock.svg';
import unlockIcon from '../assets/icons/unlock.svg';

const DEBUG_MODE = true;
const DEBUG_PASSWORD = 'monostudio';

export default {
  name: 'PasswordGate',
  emits: ['unlocked', 'locked'],
  data() {
    return {
      lockIcon,
      unlockIcon,
      password: DEBUG_MODE ? DEBUG_PASSWORD : '',
      newPassword: '',
      error: '',
      hasPassword: false,
      isUnlocked: false,
      DEBUG_MODE
    };
  },
  mounted() {
    this.checkPassword();
  },
  methods: {
    checkPassword() {
      const stored = localStorage.getItem('personal-life-password');
      this.hasPassword = !!stored;
    },
    handleSubmit() {
      this.error = '';

      if (this.hasPassword) {
        const stored = localStorage.getItem('personal-life-password');
        const isDebugPassword = DEBUG_MODE && this.password === DEBUG_PASSWORD;
        if (this.password === stored || isDebugPassword) {
          this.isUnlocked = true;
          this.$emit('unlocked');
        } else {
          this.error = '密码错误，请重试';
        }
      } else {
        if (this.newPassword.length < 4) {
          this.error = '密码至少需要4位';
          return;
        }
        localStorage.setItem('personal-life-password', this.newPassword);
        this.hasPassword = true;
        this.isUnlocked = true;
        this.$emit('unlocked');
      }
    },
    handleLock() {
      this.isUnlocked = false;
      this.password = DEBUG_MODE ? DEBUG_PASSWORD : '';
      this.newPassword = '';
      this.$emit('locked');
    }
  }
};
</script>

<style scoped>
.password-gate-panel {
  height: 100%;
  display: flex;
  align-items: center;
  justify-content: center;
}

.gate-content {
  display: flex;
  flex-direction: column;
  align-items: center;
  padding: 40px 20px;
  text-align: center;
  max-width: 320px;
}

.gate-icon {
  width: 64px;
  height: 64px;
  margin-bottom: 20px;
  opacity: 0.8;
}

.gate-icon img {
  width: 100%;
  height: 100%;
}

.gate-content h3 {
  margin: 0 0 12px;
  font-size: 18px;
  font-weight: 600;
  color: var(--text-primary);
}

.gate-description {
  margin: 0 0 24px;
  color: var(--text-secondary);
  font-size: 14px;
  line-height: 1.5;
}

.gate-form {
  width: 100%;
  display: flex;
  flex-direction: column;
  gap: 12px;
}

.gate-input {
  width: 100%;
}

.gate-error {
  color: var(--danger);
  font-size: 13px;
  margin: 0;
}

.gate-submit,
.gate-lock-btn {
  width: 100%;
  margin-top: 8px;
}
</style>