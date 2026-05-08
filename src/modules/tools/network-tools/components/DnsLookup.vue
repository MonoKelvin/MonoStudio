<template>
    <div class="dns-section">
        <div class="section-header">
            <div class="section-icon">
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                    <path d="M21 16V8a2 2 0 0 0-1-1.73l-7-4a2 2 0 0 0-2 0l-7 4A2 2 0 0 0 3 8v8a2 2 0 0 0 1 1.73l7 4a2 2 0 0 0 2 0l7-4A2 2 0 0 0 21 16z"/>
                    <polyline points="3.27 6.96 12 12.01 20.73 6.96"/>
                    <line x1="12" y1="22.08" x2="12" y2="12"/>
                </svg>
            </div>
            <h4>DNS 查找</h4>
        </div>

        <div class="dns-content">
            <div class="input-group">
                <BaseInput
                    v-model="domain"
                    placeholder="输入域名..."
                    @keyup.enter="handleLookup"
                    class="dns-input"
                />
                <BaseButton
                    @click="handleLookup"
                    :disabled="!domain || isLooking"
                    class="dns-btn"
                >
                    <span v-if="!isLooking">查找</span>
                    <span v-else class="loading-spinner"></span>
                </BaseButton>
            </div>

            <Transition name="fade">
                <div class="dns-results" v-if="results.length > 0">
                    <div
                        v-for="(result, index) in results"
                        :key="index"
                        class="dns-item"
                        :class="result.type.toLowerCase()"
                    >
                        <div class="record-info">
                            <span class="record-type">{{ result.type }}</span>
                            <span class="record-value">{{ result.value }}</span>
                        </div>
                        <span class="record-ttl" v-if="result.ttl">响应: {{ result.ttl }}ms</span>
                    </div>
                </div>
            </Transition>
        </div>
    </div>
</template>

<script>
import BaseButton from '../../../../components/base/BaseButton.vue';
import BaseInput from '../../../../components/base/BaseInput.vue';
import { useDnsLookup } from '../composables/useDnsLookup.js';

export default {
    name: 'DnsLookup',
    components: {
        BaseButton,
        BaseInput
    },
    setup() {
        const { domain, results, isLooking, lookup, reset } = useDnsLookup();

        const handleLookup = () => {
            reset();
            lookup();
        };

        return {
            domain,
            results,
            isLooking,
            handleLookup
        };
    }
};
</script>

<style scoped>
.dns-section {
    background: var(--bg-secondary);
    border-radius: var(--radius-md);
    padding: var(--spacing-md);
    border: 1px solid var(--border-color);
    transition: box-shadow 0.2s ease;
}

.dns-section:hover {
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
    background: rgba(16, 185, 129, 0.1);
    border-radius: var(--radius-sm);
    color: var(--success);
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

.dns-content {
    display: flex;
    flex-direction: column;
    gap: var(--spacing-md);
}

.input-group {
    display: flex;
    gap: var(--spacing-sm);
}

.dns-input {
    flex: 1;
}

.dns-btn {
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

.dns-results {
    background: var(--bg-primary);
    border-radius: var(--radius-md);
    padding: var(--spacing-md);
    border: 1px solid var(--border-color);
}

.dns-item {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: var(--spacing-sm) 0;
    border-bottom: 1px solid var(--border-color);
    transition: all 0.2s ease;
}

.dns-item:last-child {
    border-bottom: none;
}

.dns-item:hover {
    transform: translateX(4px);
}

.dns-item.error {
    color: var(--danger);
}

.record-info {
    display: flex;
    align-items: center;
    gap: var(--spacing-sm);
}

.record-type {
    font-size: var(--font-size-md);
    font-weight: 600;
    color: var(--accent-color);
    background: rgba(59, 130, 246, 0.1);
    padding: 4px 10px;
    border-radius: var(--radius-sm);
    min-width: 50px;
    text-align: center;
}

.record-value {
    font-size: var(--font-size-md);
    color: var(--text-primary);
    font-family: 'SF Mono', monospace;
}

.record-ttl {
    font-size: var(--font-size-md);
    color: var(--text-muted);
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

    .dns-btn {
        width: 100%;
    }

    .dns-item {
        flex-direction: column;
        align-items: flex-start;
        gap: var(--spacing-xs);
    }

    .dns-item:hover {
        transform: none;
    }
}
</style>
