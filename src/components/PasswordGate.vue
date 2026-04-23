<template>
    <BasePanel class="panel password-gate-panel">
        <div class="gate-content">
            <div class="item-icon-svg gate-icon">
                <img :src="lockIcon" alt="" />
            </div>
            <h3 v-if="!hasPassword">设置访问密码</h3>
            <h3 v-else>请输入密码</h3>

            <p class="gate-description" v-if="!hasPassword">
                首次使用个人生活模块，请设置访问密码
            </p>
            <p class="gate-description" v-else>
                输入密码以访问个人生活模块
            </p>

            <form @submit.prevent="handleSubmit" class="gate-form">
                <BaseInput v-if="!hasPassword" v-model="newPassword" type="password" placeholder="设置密码（至少6位）"
                    class="gate-input" />
                <BaseInput v-if="!hasPassword" v-model="confirmPassword" type="password" placeholder="确认密码"
                    class="gate-input" />
                <BaseInput v-else v-model="password" type="password" placeholder="请输入密码" class="gate-input" />
                <p v-if="error" class="gate-error">{{ error }}</p>
                <BaseButton type="submit" variant="primary" class="gate-submit">
                    {{ hasPassword ? '解锁' : '设置密码' }}
                </BaseButton>
            </form>
        </div>
    </BasePanel>
</template>

<script>
import lockIcon from '../assets/icons/lock.svg';

const MIN_PASSWORD_LENGTH = 6;

export default {
    name: 'PasswordGate',
    emits: ['unlocked'],
    data() {
        return {
            lockIcon,
            password: '',
            newPassword: '',
            confirmPassword: '',
            error: '',
            hasPassword: false
        };
    },
    mounted() {
        this.checkPassword();
    },
    methods: {
        async hashPassword(password) {
            const encoder = new TextEncoder();
            const data = encoder.encode(password);
            const hashBuffer = await crypto.subtle.digest('SHA-256', data);
            const hashArray = Array.from(new Uint8Array(hashBuffer));
            const hashHex = hashArray.map(b => b.toString(16).padStart(2, '0')).join('');
            return hashHex;
        },
        async checkPassword() {
            try {
                const passwordData = await window.electronAPI.userData.getPassword();
                this.hasPassword = !!(passwordData && passwordData.hash);
            } catch (e) {
                console.error('Failed to check password:', e);
                this.hasPassword = false;
            }
        },
        async handleSubmit() {
            this.error = '';

            try {
                const passwordData = await window.electronAPI.userData.getPassword();

                if (this.hasPassword && passwordData) {
                    const inputHash = await this.hashPassword(this.password);

                    if (inputHash === passwordData.hash) {
                        this.$emit('unlocked');
                        this.password = '';
                    } else {
                        this.error = '密码错误，请重试';
                    }
                } else {
                    if (this.newPassword.length < MIN_PASSWORD_LENGTH) {
                        this.error = `密码至少需要${MIN_PASSWORD_LENGTH}位`;
                        return;
                    }

                    if (this.newPassword !== this.confirmPassword) {
                        this.error = '两次输入的密码不一致';
                        return;
                    }

                    const passwordHash = await this.hashPassword(this.newPassword);
                    await window.electronAPI.userData.savePassword({ hash: passwordHash });
                    this.hasPassword = true;
                    this.$emit('unlocked');
                    this.newPassword = '';
                    this.confirmPassword = '';
                }
            } catch (e) {
                console.error('Password operation failed:', e);
                this.error = '操作失败，请重试';
            }
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
