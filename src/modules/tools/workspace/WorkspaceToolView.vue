<template>
    <BasePanel class="panel workspace-panel">
        <div class="workspace-container">
            <div class="tab-bar">
                <button
                    class="tab-btn"
                    :class="{ active: activeTab === 'current' }"
                    @click="activeTab = 'current'"
                >
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                        <rect x="2" y="3" width="20" height="14" rx="2" ry="2"/>
                        <line x1="8" y1="21" x2="16" y2="21"/>
                        <line x1="12" y1="17" x2="12" y2="21"/>
                    </svg>
                    当前窗口
                    <span class="tab-badge" v-if="windowCount > 0">{{ windowCount }}</span>
                </button>
                <button
                    class="tab-btn"
                    :class="{ active: activeTab === 'saved' }"
                    @click="activeTab = 'saved'"
                >
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                        <path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z"/>
                        <polyline points="17 21 17 13 7 13 7 21"/>
                        <polyline points="7 3 7 8 15 8"/>
                    </svg>
                    已保存
                    <span class="tab-badge" v-if="savedWorkspaces.length > 0">{{ savedWorkspaces.length }}</span>
                </button>
            </div>

            <div class="panel-content">
                <div v-if="activeTab === 'current'" class="current-tab">
                    <div class="section-header">
                        <div class="section-title-area">
                            <span class="section-title">当前打开的窗口</span>
                            <BaseButton @click="handleRefresh" :disabled="isLoading" size="sm" variant="secondary">
                                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" class="refresh-icon" :class="{ spinning: isLoading }">
                                    <path d="M23 4v6h-6"/>
                                    <path d="M1 20v-6h6"/>
                                    <path d="M3.51 9a9 9 0 0 1 14.85-3.36L23 10"/>
                                    <path d="M20.49 15a9 9 0 0 1-14.85 3.36L1 14"/>
                                </svg>
                                刷新
                            </BaseButton>
                        </div>
                        <div class="section-actions">
                            <div class="save-form" v-if="windowCount > 0">
                                <BaseInput
                                    v-model="workspaceName"
                                    placeholder="输入工作区名称..."
                                    class="name-input"
                                    @keyup.enter="handleSave"
                                />
                                <BaseButton @click="handleSave" :disabled="!workspaceName || isLoading" size="sm">
                                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" class="btn-icon">
                                        <path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z"/>
                                        <polyline points="17 21 17 13 7 13 7 21"/>
                                        <polyline points="7 3 7 8 15 8"/>
                                    </svg>
                                    保存
                                </BaseButton>
                            </div>
                        </div>
                    </div>

                    <div v-if="isLoading && windowCount === 0" class="loading-state">
                        <BaseSpinbox />
                        <p>正在获取窗口信息...</p>
                    </div>

                    <div v-else-if="windowCount === 0" class="empty-state">
                        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5" class="empty-icon">
                            <rect x="2" y="3" width="20" height="14" rx="2" ry="2"/>
                            <line x1="8" y1="21" x2="16" y2="21"/>
                            <line x1="12" y1="17" x2="12" y2="21"/>
                        </svg>
                        <p>当前没有检测到窗口</p>
                        <span class="empty-hint">确保其他应用程序已打开并可见</span>
                    </div>

                    <div v-else class="window-list">
                        <div
                            v-for="(win, index) in openWindows"
                            :key="win.hwnd || index"
                            class="window-item"
                        >
                            <div class="window-icon">
                                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                                    <rect x="2" y="3" width="20" height="14" rx="2" ry="2"/>
                                    <line x1="8" y1="21" x2="16" y2="21"/>
                                    <line x1="12" y1="17" x2="12" y2="21"/>
                                </svg>
                            </div>
                            <div class="window-info">
                                <span class="window-title">{{ win.title || '未命名窗口' }}</span>
                                <span class="window-path">{{ win.path || '路径未知' }}</span>
                            </div>
                            <div class="window-meta">
                                <span class="window-position">{{ win.width }}×{{ win.height }}</span>
                            </div>
                        </div>
                    </div>
                </div>

                <div v-if="activeTab === 'saved'" class="saved-tab">
                    <div v-if="isLoading && savedWorkspaces.length === 0" class="loading-state">
                        <BaseSpinbox />
                        <p>正在加载已保存的工作区...</p>
                    </div>

                    <div v-else-if="savedWorkspaces.length === 0" class="empty-state">
                        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5" class="empty-icon">
                            <path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z"/>
                            <polyline points="17 21 17 13 7 13 7 21"/>
                            <polyline points="7 3 7 8 15 8"/>
                        </svg>
                        <p>暂无保存的工作区</p>
                        <span class="empty-hint">切换到"当前窗口"标签保存您的工作区</span>
                    </div>

                    <div v-else class="workspace-list">
                        <div
                            v-for="ws in savedWorkspaces"
                            :key="ws.id"
                            class="workspace-item"
                        >
                            <div class="workspace-icon">
                                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                                    <path d="M19 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11l5 5v11a2 2 0 0 1-2 2z"/>
                                    <polyline points="17 21 17 13 7 13 7 21"/>
                                    <polyline points="7 3 7 8 15 8"/>
                                </svg>
                            </div>
                            <div class="workspace-info">
                                <span class="workspace-name">{{ ws.name }}</span>
                                <span class="workspace-meta">
                                    {{ ws.windowCount }} 个窗口 · {{ formatTimestamp(ws.timestamp) }}
                                </span>
                            </div>
                            <div class="workspace-actions">
                                <BaseButton
                                    @click="handleRestore(ws)"
                                    :disabled="isRestoring"
                                    variant="primary"
                                    size="sm"
                                >
                                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" class="btn-icon">
                                        <polygon points="5 3 19 12 5 21 5 3"/>
                                    </svg>
                                    还原
                                </BaseButton>
                                <BaseButton
                                    @click="handleDelete(ws)"
                                    :disabled="isLoading"
                                    variant="danger"
                                    size="sm"
                                >
                                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" class="btn-icon">
                                        <polyline points="3 6 5 6 21 6"/>
                                        <path d="M19 6v14a2 2 0 0 1-2 2H7a2 2 0 0 1-2-2V6m3 0V4a2 2 0 0 1 2-2h4a2 2 0 0 1 2 2v2"/>
                                    </svg>
                                </BaseButton>
                            </div>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </BasePanel>
</template>

<script>
import { ref } from 'vue';
import BasePanel from '../../../components/base/BasePanel.vue';
import BaseButton from '../../../components/base/BaseButton.vue';
import BaseInput from '../../../components/base/BaseInput.vue';
import BaseSpinbox from '../../../components/base/BaseSpinbox.vue';
import { useWorkspace } from './composables/useWorkspace.js';
import { toastService } from '../../../common/services/toastService.js';

export default {
    name: 'WorkspaceToolView',
    components: {
        BasePanel,
        BaseButton,
        BaseInput,
        BaseSpinbox
    },
    setup() {
        const {
            openWindows,
            savedWorkspaces,
            isLoading,
            isRestoring,
            activeTab,
            windowCount,
            fetchOpenWindows,
            fetchSavedWorkspaces,
            saveCurrentWorkspace,
            deleteWorkspace,
            restoreWorkspace,
            formatTimestamp
        } = useWorkspace();

        const workspaceName = ref('');

        const handleRefresh = async () => {
            if (activeTab.value === 'current') {
                await fetchOpenWindows();
            } else {
                await fetchSavedWorkspaces();
            }
        };

        const handleSave = async () => {
            if (!workspaceName.value) {
                toastService.warning('请输入工作区名称');
                return;
            }

            const result = await saveCurrentWorkspace(workspaceName.value);
            if (result.success) {
                toastService.success('工作区保存成功');
                workspaceName.value = '';
            } else {
                toastService.error('保存失败：' + (result.error || '未知错误'));
            }
        };

        const handleRestore = async (ws) => {
            const result = await restoreWorkspace(ws);
            if (result.success) {
                const successCount = result.results.filter(r => r.success).length;
                toastService.success(`已还原 ${successCount}/${result.results.length} 个窗口`);
                await fetchOpenWindows();
            } else {
                toastService.error('还原失败：' + (result.error || '未知错误'));
            }
        };

        const handleDelete = async (ws) => {
            const result = await deleteWorkspace(ws);
            if (result.success) {
                toastService.success('工作区已删除');
            } else {
                toastService.error('删除失败：' + (result.error || '未知错误'));
            }
        };

        return {
            openWindows,
            savedWorkspaces,
            isLoading,
            isRestoring,
            activeTab,
            windowCount,
            workspaceName,
            handleRefresh,
            handleSave,
            handleRestore,
            handleDelete,
            formatTimestamp
        };
    },
    async mounted() {
        await this.handleRefresh();
    },
    watch: {
        activeTab(newTab) {
            if (newTab === 'current') {
                this.handleRefresh();
            } else if (newTab === 'saved') {
                this.handleRefresh();
            }
        }
    }
};
</script>

<style scoped>
.workspace-panel {
    height: 100%;
    overflow: hidden;
}

.workspace-container {
    padding: var(--spacing-md);
    padding-top: 0;
    height: 100%;
    display: flex;
    flex-direction: column;
    gap: var(--spacing-sm);
}

.refresh-icon {
    width: 14px;
    height: 14px;
    transition: transform 0.3s ease;
}

.refresh-icon.spinning {
    animation: spin 1s linear infinite;
}

@keyframes spin {
    from { transform: rotate(0deg); }
    to { transform: rotate(360deg); }
}

.tab-bar {
    display: flex;
    background: var(--bg-tertiary);
    flex-shrink: 0;
}

.tab-btn {
    flex: 1;
    display: flex;
    align-items: center;
    justify-content: center;
    gap: var(--spacing-sm);
    padding: var(--spacing-md) var(--spacing-lg);
    border: none;
    background: transparent;
    border-radius: var(--radius-md);
    color: var(--text-secondary);
    font-size: var(--font-size-lg);
    font-weight: 500;
    cursor: pointer;
    transition: all 0.2s ease;
}

.tab-btn svg {
    width: 20px;
    height: 20px;
}

.tab-btn:hover {
    color: var(--accent-hover);
}

.tab-btn.active {
    color: var(--accent-color);
}

.tab-badge {
    background: var(--accent-color);
    color: white;
    font-size: var(--font-size-xs);
    font-weight: 600;
    border-radius: 50%;
    min-width: 18px;
    min-height: 18px;
    text-align: center;
    margin-left: 4px;
    margin-top: 2px;
}

.tab-btn:not(.active) .tab-badge {
    background: var(--text-muted);
}

.panel-content {
    flex: 1;
    overflow-y: auto;
    min-height: 0;
    padding-top: var(--spacing-xs);
}

.current-tab,
.saved-tab {
    display: flex;
    flex-direction: column;
    gap: var(--spacing-lg);
}

.section-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    gap: var(--spacing-lg);
    flex-wrap: wrap;
}

.section-title-area {
    display: flex;
    align-items: center;
    gap: var(--spacing-md);
}

.section-title {
    font-size: var(--font-size-lg);
    font-weight: 600;
    color: var(--text-primary);
}

.section-actions {
    display: flex;
    gap: var(--spacing-md);
    align-items: center;
}

.save-form {
    display: flex;
    gap: var(--spacing-md);
    align-items: center;
}

.name-input {
    width: 200px;
}

.btn-icon {
    width: 14px;
    height: 14px;
    margin-right: 4px;
}

.loading-state,
.empty-state {
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
    padding: var(--spacing-xl) var(--spacing-lg);
    color: var(--text-muted);
}

.empty-icon {
    width: 56px;
    height: 56px;
    margin-bottom: var(--spacing-md);
    opacity: 0.5;
}

.empty-state p {
    margin: 0;
    font-size: var(--font-size-lg);
    color: var(--text-secondary);
}

.empty-hint {
    font-size: var(--font-size-md);
    color: var(--text-muted);
    margin-top: var(--spacing-sm);
}

.window-list,
.workspace-list {
    display: flex;
    flex-direction: column;
    gap: var(--spacing-sm);
}

.window-item {
    display: flex;
    align-items: center;
    gap: var(--spacing-md);
    padding: var(--spacing-md) var(--spacing-lg);
    background: var(--bg-secondary);
    border-radius: var(--radius-lg);
    border: 1px solid var(--border-color);
    transition: all 0.2s ease;
}

.window-item:hover {
    border-color: var(--accent-color);
    box-shadow: 0 2px 8px rgba(0, 0, 0, 0.08);
}

.window-icon,
.workspace-icon {
    width: 40px;
    height: 40px;
    display: flex;
    align-items: center;
    justify-content: center;
    background: var(--bg-tertiary);
    border-radius: var(--radius-md);
    color: var(--text-muted);
    flex-shrink: 0;
}

.window-icon svg,
.workspace-icon svg {
    width: 20px;
    height: 20px;
}

.window-info,
.workspace-info {
    flex: 1;
    min-width: 0;
    display: flex;
    flex-direction: column;
    gap: var(--spacing-xs);
}

.window-title,
.workspace-name {
    font-size: var(--font-size-md);
    font-weight: 600;
    color: var(--text-primary);
    white-space: nowrap;
    overflow: hidden;
    text-overflow: ellipsis;
}

.window-path,
.workspace-meta {
    font-size: var(--font-size-md);
    color: var(--text-muted);
    white-space: nowrap;
    overflow: hidden;
    text-overflow: ellipsis;
}

.window-meta {
    flex-shrink: 0;
}

.window-position {
    font-size: var(--font-size-md);
    color: var(--text-muted);
    font-family: 'SF Mono', 'Consolas', monospace;
    background: var(--bg-tertiary);
    padding: var(--spacing-xs) var(--spacing-sm);
    border-radius: var(--radius-md);
}

.workspace-actions {
    display: flex;
    gap: var(--spacing-sm);
    flex-shrink: 0;
}

@media (max-width: 600px) {
    .section-header {
        flex-direction: column;
        align-items: stretch;
    }

    .save-form {
        width: 100%;
    }

    .name-input {
        flex: 1;
    }

    .window-item {
        flex-wrap: wrap;
    }

    .window-meta {
        width: 100%;
        margin-top: var(--spacing-sm);
    }

    .workspace-actions {
        width: 100%;
        justify-content: flex-end;
    }
}
</style>
