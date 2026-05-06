<template>
    <div class="ipquery-section">
        <div class="section-header">
            <div class="section-icon">
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                    <circle cx="12" cy="12" r="10"/>
                    <line x1="2" y1="12" x2="22" y2="12"/>
                    <path d="M12 2a15.3 15.3 0 0 1 4 10 15.3 15.3 0 0 1-4 10 15.3 15.3 0 0 1-4-10 15.3 15.3 0 0 1 4-10z"/>
                </svg>
            </div>
            <h4>IP 信息查询</h4>
        </div>

        <div class="ipquery-content">
            <div class="input-group">
                <BaseInput
                    v-model="queryTarget"
                    placeholder="输入 IP 地址或域名..."
                    @keyup.enter="handleQuery"
                    class="ipquery-input"
                />
                <BaseButton
                    @click="handleQuery"
                    :disabled="!queryTarget || isQuerying"
                    class="ipquery-btn"
                >
                    <span v-if="!isQuerying">查询</span>
                    <span v-else class="loading-spinner"></span>
                </BaseButton>
            </div>

            <Transition name="fade">
                <div class="ip-info-panel" v-if="result">
                    <div class="ip-header">
                        <span class="ip-label">IP / 域名</span>
                        <span class="ip-value">{{ result.ip }}</span>
                    </div>
                    <div class="info-grid">
                        <div class="info-item">
                            <span class="item-label">国家</span>
                            <span class="item-value">{{ result.country }}</span>
                        </div>
                        <div class="info-item">
                            <span class="item-label">地区</span>
                            <span class="item-value">{{ result.region }}</span>
                        </div>
                        <div class="info-item">
                            <span class="item-label">城市</span>
                            <span class="item-value">{{ result.city }}</span>
                        </div>
                        <div class="info-item">
                            <span class="item-label">运营商</span>
                            <span class="item-value">{{ result.isp }}</span>
                        </div>
                        <div class="info-item full-width">
                            <span class="item-label">组织</span>
                            <span class="item-value">{{ result.org }}</span>
                        </div>
                    </div>
                </div>
            </Transition>
        </div>
    </div>
</template>

<script>
import BaseButton from '../../../../components/base/BaseButton.vue';
import BaseInput from '../../../../components/base/BaseInput.vue';
import { useIpQuery } from '../composables/useIpQuery.js';

export default {
    name: 'IpQuery',
    components: {
        BaseButton,
        BaseInput
    },
    setup() {
        const { queryTarget, result, isQuerying, query, reset } = useIpQuery();

        const handleQuery = () => {
            reset();
            query();
        };

        return {
            queryTarget,
            result,
            isQuerying,
            handleQuery
        };
    }
};
</script>

<style scoped>
.ipquery-section {
    background: var(--bg-secondary);
    border-radius: var(--radius-md);
    padding: var(--spacing-md);
    border: 1px solid var(--border-color);
    transition: box-shadow 0.2s ease;
}

.ipquery-section:hover {
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.06);
}

.section-header {
    display: flex;
    align-items: center;
    gap: var(--spacing-sm);
    margin-bottom: var(--spacing-md);
}

.section-icon {
    width: 36px;
    height: 36px;
    display: flex;
    align-items: center;
    justify-content: center;
    background: rgba(139, 92, 246, 0.1);
    border-radius: var(--radius-sm);
    color: #8b5cf6;
}

.section-icon svg {
    width: 18px;
    height: 18px;
}

.section-header h4 {
    margin: 0;
    font-size: var(--font-size-md);
    font-weight: 600;
    color: var(--text-primary);
}

.ipquery-content {
    display: flex;
    flex-direction: column;
    gap: var(--spacing-md);
}

.input-group {
    display: flex;
    gap: var(--spacing-sm);
}

.ipquery-input {
    flex: 1;
}

.ipquery-btn {
    min-width: 80px;
    display: flex;
    align-items: center;
    justify-content: center;
}

.loading-spinner {
    width: 14px;
    height: 14px;
    border: 2px solid rgba(255, 255, 255, 0.3);
    border-top-color: white;
    border-radius: 50%;
    animation: spin 0.8s linear infinite;
}

@keyframes spin {
    to { transform: rotate(360deg); }
}

.ip-info-panel {
    background: var(--bg-primary);
    border-radius: var(--radius-md);
    padding: var(--spacing-md);
    border: 1px solid var(--border-color);
}

.ip-header {
    display: flex;
    flex-direction: column;
    gap: var(--spacing-xs);
    padding-bottom: var(--spacing-md);
    border-bottom: 1px solid var(--border-color);
    margin-bottom: var(--spacing-md);
}

.ip-label {
    font-size: var(--font-size-md);
    color: var(--text-muted);
}

.ip-value {
    font-size: var(--font-size-lg);
    font-weight: 600;
    color: var(--text-primary);
    font-family: 'SF Mono', monospace;
}

.info-grid {
    display: grid;
    grid-template-columns: repeat(2, 1fr);
    gap: var(--spacing-md);
}

.info-item {
    display: flex;
    flex-direction: column;
    gap: 4px;
}

.info-item.full-width {
    grid-column: 1 / -1;
}

.item-label {
    font-size: var(--font-size-md);
    color: var(--text-muted);
}

.item-value {
    font-size: var(--font-size-md);
    color: var(--text-primary);
    font-weight: 500;
}

.fade-enter-active,
.fade-leave-active {
    transition: opacity 0.2s ease;
}

.fade-enter-from,
.fade-leave-to {
    opacity: 0;
}

@media (max-width: 480px) {
    .input-group {
        flex-direction: column;
    }

    .ipquery-btn {
        width: 100%;
    }

    .info-grid {
        grid-template-columns: 1fr;
    }
}
</style>
