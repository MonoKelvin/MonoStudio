<template>
    <div class="settings-section">
        <h3>用户数据</h3>
        <p>管理软件设置和用户数据的存储位置</p>
        <div class="settings-group">
            <SettingsItem title="数据目录">
                <template #description>
                    <a class="path-link" @click="openDirectory(currentPath)">{{ currentPath || '加载中...' }}</a>
                </template>
                <BaseButton variant="primary" @click="selectDirectory">
                    选择目录
                </BaseButton>
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

        <div v-if="showMigrationConfirm" class="modal-overlay" @click="cancelMigration">
            <div class="modal-content" @click.stop>
                <div class="modal-header">
                    <h3>确认迁移数据</h3>
                    <button class="modal-close" @click="cancelMigration">×</button>
                </div>
                <div class="modal-body">
                    <p class="migration-warning">即将把数据从：</p>
                    <div class="migration-path old">{{ migrationInfo.oldPath }}</div>
                    <p class="migration-arrow">迁移到：</p>
                    <div class="migration-path new">{{ migrationInfo.newPath }}</div>
                    <p class="migration-note">迁移过程中，源目录中的数据不会被删除。迁移完成后，软件将使用新的数据目录。</p>
                </div>
                <div class="modal-footer">
                    <BaseButton variant="secondary" @click="cancelMigration">取消</BaseButton>
                    <BaseButton variant="primary" @click="confirmMigration">确认迁移</BaseButton>
                </div>
            </div>
        </div>

        <div v-if="showMigrationProgress" class="modal-overlay">
            <div class="modal-content" @click.stop>
                <div class="modal-header">
                    <h3>正在迁移数据</h3>
                </div>
                <div class="modal-body">
                    <div class="migration-progress">
                        <div class="progress-icon">📦</div>
                        <p>正在迁移您的数据，请稍候...</p>
                    </div>
                </div>
            </div>
        </div>

        <div v-if="migrationResult" class="migration-result">
            <!-- Migration result is now shown via toast service -->
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
import BaseSelect from '../../../../../components/base/BaseSelect.vue';
import BaseInput from '../../../../../components/base/BaseInput.vue';
import SettingsItem from '../../../../../components/settings/SettingsItem.vue';

const MIN_PASSWORD_LENGTH = 6;

export default {
    name: 'UserSettings',
    components: {
        BaseButton,
        BaseSelect,
        BaseInput,
        SettingsItem
    },
    emits: ['data-path-changed', 'security-settings-changed'],
    data() {
        return {
            currentPath: '',
            defaultPath: '',
            showMigrationConfirm: false,
            showMigrationProgress: false,
            migrationInfo: {
                oldPath: '',
                newPath: ''
            },
            pendingPath: '',
            migrationResult: null,
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
        securitySettings: {
            handler(newSettings) {
                this.saveSecuritySettings();
                this.syncOldSettingsStructure();
            },
            deep: true
        }
    },
    async mounted() {
        await this.loadPaths();
        await this.loadSecuritySettings();
    },
    methods: {
        async loadPaths() {
            try {
                this.currentPath = await window.electronAPI.userData.getPath();
                this.defaultPath = await window.electronAPI.userData.getDefaultPath();
            } catch (e) {
                console.error('Failed to load paths:', e);
            }
        },
        async selectDirectory() {
            try {
                const result = await window.electronAPI.userData.selectDirectory();
                if (result.success && result.path) {
                    if (result.path === this.currentPath) {
                        return;
                    }
                    this.pendingPath = result.path;
                    this.migrationInfo = {
                        oldPath: this.currentPath,
                        newPath: result.path
                    };
                    this.showMigrationConfirm = true;
                }
            } catch (e) {
                console.error('Failed to select directory:', e);
            }
        },
        cancelMigration() {
            this.showMigrationConfirm = false;
            this.pendingPath = '';
            this.migrationInfo = { oldPath: '', newPath: '' };
        },
        async confirmMigration() {
            this.showMigrationConfirm = false;
            this.showMigrationProgress = true;
            this.migrationResult = null;

            try {
                const result = await window.electronAPI.userData.setPath(this.pendingPath);
                this.showMigrationProgress = false;

                if (result.migration) {
                    // Show toast using global toast service
                    this.$toast[
                        result.migration.success ? 'success' : 'warning'
                    ](
                        result.migration.success ?
                            `已迁移：${result.migration.migrated.join(', ') || '无'}` :
                            '有部分数据未能迁移',
                        {
                            title: result.migration.success ? '数据迁移成功！' : '数据迁移完成',
                            subtitle: result.migration.success ? '' :
                                `失败：${result.migration.failed.join(', ') || '无'}`
                        }
                    );
                    this.currentPath = this.pendingPath;
                    this.$emit('data-path-changed', this.pendingPath);
                } else {
                    // Show error toast
                    this.$toast.error('数据迁移过程中出现未知错误', {
                        title: '迁移失败'
                    });
                }
            } catch (e) {
                this.showMigrationProgress = false;
                console.error('Migration failed:', e);
                // Show error toast
                this.$toast.error('数据迁移过程中出现错误', {
                    title: '迁移失败',
                    subtitle: e.message
                });
            }

            this.pendingPath = '';
            this.migrationInfo = { oldPath: '', newPath: '' };
        },
        async openDirectory(path) {
            if (path) {
                try {
                    const result = await window.electronAPI.userData.openDirectory(path);
                    if (!result.success) {
                        console.error('Failed to open directory:', result.error);
                    }
                } catch (e) {
                    console.error('Failed to open directory:', e);
                }
            }
        },
        async loadSecuritySettings() {
            try {
                const settings = await window.electronAPI.userData.getSecuritySettings();
                if (settings) {
                    this.securitySettings = { ...this.securitySettings, ...settings };
                    this.migrateOldSettings();
                }
            } catch (e) {
                console.error('Failed to load security settings:', e);
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
        async saveSecuritySettings() {
            try {
                await window.electronAPI.userData.saveSecuritySettings(this.securitySettings);
                this.$emit('security-settings-changed');
            } catch (e) {
                console.error('Failed to save security settings:', e);
            }
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

            try {
                const passwordData = await window.electronAPI.userData.getPassword();
                const storedHash = passwordData?.hash;
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
                await window.electronAPI.userData.savePassword({ hash: newHash });
                this.success = '密码修改成功';

                setTimeout(() => {
                    this.closeChangePassword();
                }, 1500);
            } catch (e) {
                console.error('Change password failed:', e);
                this.error = '密码修改失败，请重试';
            }
        }
    }
};
</script>

<style scoped>
.settings-group {
    margin-bottom: 32px;
}

.path-link {
    color: var(--text-muted);
    text-decoration: underline;
    text-underline-offset: 3px;
    cursor: pointer;
    font-family: monospace;
    font-size: 14px;
    word-break: break-all;
}

.path-link:hover {
    color: var(--accent-color);
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
    width: 480px;
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
    background: var(--bg-secondary);
}

.modal-body {
    padding: 20px;
}

.modal-footer {
    display: flex;
    justify-content: flex-end;
    gap: 12px;
    padding: 16px 20px;
    border-top: 1px solid var(--border-color);
}

.migration-warning {
    margin: 0 0 8px;
    color: var(--text-secondary);
}

.migration-arrow {
    margin: 8px 0;
    color: var(--text-secondary);
}

.migration-path {
    padding: 12px;
    border-radius: 6px;
    font-size: 13px;
    word-break: break-all;
    font-family: monospace;
    margin: 8px 0;
}

.migration-path.old {
    background: rgba(239, 68, 68, 0.1);
    border: 1px solid rgba(239, 68, 68, 0.3);
    color: var(--text-primary);
}

.migration-path.new {
    background: rgba(34, 197, 94, 0.1);
    border: 1px solid rgba(34, 197, 94, 0.3);
    color: var(--text-primary);
}

.migration-note {
    margin: 16px 0 0;
    font-size: 13px;
    color: var(--text-secondary);
    line-height: 1.5;
}

.migration-progress {
    text-align: center;
    padding: 20px;
}

.progress-icon {
    font-size: 48px;
    margin-bottom: 16px;
}

.migration-progress p {
    margin: 0;
    color: var(--text-secondary);
}

.settings-select {
    width: 140px;
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
