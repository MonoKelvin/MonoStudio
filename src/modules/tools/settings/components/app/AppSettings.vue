<template>
    <div class="settings-section">
        <h3>应用行为</h3>
        <p>控制应用的基本行为和功能</p>
        <div class="settings-group">
            <SettingsItem title="开发者工具" description="控制开发者工具的显示">
                <BaseSwitch :checked="localDevToolsOpen" @change="onDevToolsChange" />
            </SettingsItem>
        </div>

        <h3>密码管理</h3>
        <p>管理个人生活模块的访问密码和保护设置</p>
        <div class="settings-group">
            <SettingsItem title="修改密码" description="更改个人生活模块的访问密码">
                <BaseButton variant="primary" @click="showChangePassword = true">
                    修改密码
                </BaseButton>
            </SettingsItem>

            <SettingsItem title="保护模式" description="个人生活模块的访问控制方式">
                <BaseSelect v-model="securitySettings.protectionMode" :options="protectionModeOptions"
                    class="settings-select" />
            </SettingsItem>

            <SettingsItem v-if="securitySettings.protectionMode === 'time'" title="时间间隔" description="选择需要重新输入密码的时间间隔">
                <BaseSelect v-model="securitySettings.passwordTimeout" :options="timeoutOptions"
                    class="settings-select" />
            </SettingsItem>
        </div>

        <div v-if="showChangePassword" class="modal-overlay" @click="closeChangePassword">
            <div class="modal-content" @click.stop>
                <div class="modal-header">
                    <h3>修改密码</h3>
                    <button class="modal-close" @click="closeChangePassword">×</button>
                </div>
                <div class="modal-body">
                    <form @submit.prevent="changePassword" class="password-form">
                        <BaseInput v-model="oldPassword" type="password" placeholder="当前密码" class="password-input" />
                        <BaseInput v-model="newPassword" type="password" placeholder="新密码（至少6位）"
                            class="password-input" />
                        <BaseInput v-model="confirmPassword" type="password" placeholder="确认新密码"
                            class="password-input" />
                        <p v-if="error" class="password-error">{{ error }}</p>
                        <p v-if="success" class="password-success">{{ success }}</p>
                        <div class="modal-actions">
                            <BaseButton variant="secondary" @click="closeChangePassword">取消</BaseButton>
                            <BaseButton type="submit" variant="primary">确认修改</BaseButton>
                        </div>
                    </form>
                </div>
            </div>
        </div>
    </div>
</template>

<script>
import BaseButton from '../../../../../components/base/BaseButton.vue';
import BaseSwitch from '../../../../../components/base/BaseSwitch.vue';
import BaseSelect from '../../../../../components/base/BaseSelect.vue';
import BaseInput from '../../../../../components/base/BaseInput.vue';
import SettingsItem from '../../../../../components/settings/SettingsItem.vue';

const PASSWORD_STORAGE_KEY = 'personal-life-password-hash';
const SECURITY_SETTINGS_KEY = 'personal-life-security-settings';
const MIN_PASSWORD_LENGTH = 6;

export default {
    name: 'AppSettings',
    components: {
        BaseButton,
        BaseSwitch,
        BaseSelect,
        BaseInput,
        SettingsItem
    },
    props: {
        openDevTools: {
            type: Boolean,
            default: false
        }
    },
    emits: ['change-dev-tools', 'security-settings-changed'],
    data() {
        return {
            localDevToolsOpen: false,
            showChangePassword: false,
            oldPassword: '',
            newPassword: '',
            confirmPassword: '',
            error: '',
            success: '',
            securitySettings: {
                protectionMode: 'session',
                passwordTimeout: 30,
                requirePasswordOnInactive: false,
                requirePasswordOnBlur: false,
                requirePasswordEveryTime: false,
                requirePasswordByTime: false,
                keepSession: true
            },
            protectionModeOptions: [
                { label: '会话保持', value: 'session', description: '首次输入密码后，本次会话内无需再次验证' },
                { label: '每次验证', value: 'every', description: '每次进入个人生活模块都需要输入密码' },
                { label: '时间间隔', value: 'time', description: '间隔设定时间后需要重新输入密码' },
                { label: '未激活验证', value: 'inactive', description: '软件未激活时需要重新输入密码' }
            ],
            timeoutOptions: [
                { label: '5分钟', value: 5 },
                { label: '10分钟', value: 10 },
                { label: '15分钟', value: 15 },
                { label: '30分钟', value: 30 },
                { label: '45分钟', value: 45 },
                { label: '1小时', value: 60 },
                { label: '2小时', value: 120 }
            ]
        };
    },
    watch: {
        openDevTools: {
            immediate: true,
            handler(newVal) {
                this.localDevToolsOpen = newVal;
            }
        },
        securitySettings: {
            handler(newSettings) {
                this.saveSecuritySettings();
                this.syncOldSettingsStructure();
            },
            deep: true
        }
    },
    mounted() {
        this.loadSecuritySettings();
    },
    methods: {
        loadSecuritySettings() {
            const stored = localStorage.getItem(SECURITY_SETTINGS_KEY);
            if (stored) {
                const settings = JSON.parse(stored);
                this.securitySettings = { ...this.securitySettings, ...settings };
                this.migrateOldSettings();
            }
        },
        migrateOldSettings() {
            const { requirePasswordEveryTime, requirePasswordByTime, requirePasswordOnInactive, keepSession } = this.securitySettings;
            if (requirePasswordEveryTime) {
                this.securitySettings.protectionMode = 'every';
            } else if (requirePasswordByTime) {
                this.securitySettings.protectionMode = 'time';
            } else if (requirePasswordOnInactive) {
                this.securitySettings.protectionMode = 'inactive';
            } else if (keepSession) {
                this.securitySettings.protectionMode = 'session';
            }
        },
        syncOldSettingsStructure() {
            const { protectionMode } = this.securitySettings;
            this.securitySettings.requirePasswordEveryTime = protectionMode === 'every';
            this.securitySettings.requirePasswordByTime = protectionMode === 'time';
            this.securitySettings.requirePasswordOnInactive = protectionMode === 'inactive';
            this.securitySettings.keepSession = protectionMode === 'session';
        },
        saveSecuritySettings() {
            localStorage.setItem(SECURITY_SETTINGS_KEY, JSON.stringify(this.securitySettings));
            this.$emit('security-settings-changed');
        },
        onDevToolsChange(newVal) {
            this.localDevToolsOpen = newVal;
            this.$emit('change-dev-tools', newVal);
        },
        closeChangePassword() {
            this.showChangePassword = false;
            this.oldPassword = '';
            this.newPassword = '';
            this.confirmPassword = '';
            this.error = '';
            this.success = '';
        },
        async hashPassword(password) {
            const encoder = new TextEncoder();
            const data = encoder.encode(password);
            const hashBuffer = await crypto.subtle.digest('SHA-256', data);
            const hashArray = Array.from(new Uint8Array(hashBuffer));
            const hashHex = hashArray.map(b => b.toString(16).padStart(2, '0')).join('');
            return hashHex;
        },
        async changePassword() {
            this.error = '';
            this.success = '';

            const storedHash = localStorage.getItem(PASSWORD_STORAGE_KEY);
            const oldHash = await this.hashPassword(this.oldPassword);

            if (oldHash !== storedHash) {
                this.error = '当前密码错误';
                return;
            }

            if (this.newPassword.length < MIN_PASSWORD_LENGTH) {
                this.error = `新密码至少需要${MIN_PASSWORD_LENGTH}位`;
                return;
            }

            if (this.newPassword !== this.confirmPassword) {
                this.error = '两次输入的密码不一致';
                return;
            }

            const newHash = await this.hashPassword(this.newPassword);
            localStorage.setItem(PASSWORD_STORAGE_KEY, newHash);
            this.success = '密码修改成功';

            setTimeout(() => {
                this.closeChangePassword();
            }, 1500);
        }
    }
};
</script>

<style scoped>
.settings-group {
    margin-bottom: 32px;
}

.settings-control-group {
    display: flex;
    align-items: center;
    gap: 12px;
    width: 100%;
    justify-content: space-between;
}

.settings-control-sub {
    flex: 1;
    max-width: 150px;
    margin-left: auto;
}

.settings-select {
    width: 140px;
}

.modal-overlay {
    position: fixed;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background: rgba(0, 0, 0, 0.5);
    backdrop-filter: blur(8px);
    -webkit-backdrop-filter: blur(8px);
    display: flex;
    align-items: center;
    justify-content: center;
    z-index: 1000;
}

.modal-content {
    background: var(--bg-primary);
    border-radius: 12px;
    width: 400px;
    max-width: 90%;
    box-shadow: var(--shadow-soft);
}

.modal-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 20px;
    border-bottom: 1px solid var(--border-color);
}

.modal-header h3 {
    margin: 0;
    font-size: 16px;
    font-weight: 600;
}

.modal-close {
    background: none;
    border: none;
    font-size: 24px;
    cursor: pointer;
    color: var(--text-secondary);
    padding: 0;
    width: 30px;
    height: 30px;
    display: flex;
    align-items: center;
    justify-content: center;
    border-radius: 4px;
    transition: background 0.2s;
}

.modal-close:hover {
    background: var(--hover-bg);
}

.modal-body {
    padding: 20px;
}

.password-form {
    display: flex;
    flex-direction: column;
    gap: 16px;
}

.password-input {
    width: 100%;
}

.password-error {
    color: var(--error-color);
    font-size: 12px;
    margin: 0;
}

.password-success {
    color: var(--success-color);
    font-size: 12px;
    margin: 0;
}

.modal-actions {
    display: flex;
    gap: 12px;
    justify-content: flex-end;
    margin-top: 8px;
}
</style>
