<template>
    <div class="ping-section">
        <div class="section-header">
            <div class="section-icon">
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                    <circle cx="12" cy="12" r="10"/>
                    <polyline points="12 6 12 12 16 14"/>
                </svg>
            </div>
            <h4>Ping 测试</h4>
        </div>

        <div class="ping-content">
            <div class="input-group">
                <BaseInput
                    v-model="target"
                    placeholder="输入域名或 IP 地址..."
                    @keyup.enter="handleStart"
                    class="ping-input"
                />
                <BaseButton
                    @click="handleStart"
                    :disabled="!target || isPinging"
                    class="ping-btn"
                >
                    <span v-if="!isPinging">Ping</span>
                    <span v-else class="loading-spinner"></span>
                </BaseButton>
            </div>

            <Transition name="fade">
                <div class="ping-results" v-if="results.length > 0">
                    <div class="results-header">
                        <span class="target">目标: {{ target }}</span>
                        <div class="stats" v-if="isComplete">
                            <span class="stat success">{{ successRate }}% 成功</span>
                            <span class="divider">|</span>
                            <span class="stat">平均 {{ avgTime }}ms</span>
                        </div>
                    </div>

                    <div class="results-list">
                        <div
                            v-for="(result, index) in results"
                            :key="index"
                            class="result-item"
                            :class="result.type"
                        >
                            <span class="seq">#{{ index + 1 }}</span>
                            <span class="time" v-if="result.type === 'success'">{{ result.time }}ms</span>
                            <span class="time timeout" v-else>{{ result.status }}</span>
                            <span class="status">
                                <svg v-if="result.type === 'success'" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" class="status-icon success">
                                    <polyline points="20 6 9 17 4 12"/>
                                </svg>
                                <svg v-else viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" class="status-icon fail">
                                    <line x1="18" y1="6" x2="6" y2="18"/>
                                    <line x1="6" y1="6" x2="18" y2="18"/>
                                </svg>
                            </span>
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
import { usePing } from '../composables/usePing.js';

export default {
    name: 'PingTest',
    components: {
        BaseButton,
        BaseInput
    },
    setup() {
        const { target, results, isPinging, isComplete, successRate, avgTime, start, reset } = usePing();

        const handleStart = () => {
            reset();
            start();
        };

        return {
            target,
            results,
            isPinging,
            isComplete,
            successRate,
            avgTime,
            handleStart
        };
    }
};
</script>

<style scoped>
.ping-section {
    background: var(--bg-secondary);
    border-radius: var(--radius-md);
    padding: var(--spacing-md);
    border: 1px solid var(--border-color);
    transition: box-shadow 0.2s ease;
}

.ping-section:hover {
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
    background: rgba(59, 130, 246, 0.1);
    border-radius: var(--radius-sm);
    color: var(--accent-color);
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

.ping-content {
    display: flex;
    flex-direction: column;
    gap: var(--spacing-md);
}

.input-group {
    display: flex;
    gap: var(--spacing-sm);
}

.ping-input {
    flex: 1;
}

.ping-btn {
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

.ping-results {
    background: var(--bg-primary);
    border-radius: var(--radius-md);
    padding: var(--spacing-md);
    border: 1px solid var(--border-color);
}

.results-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: var(--spacing-sm);
    padding-bottom: var(--spacing-sm);
    border-bottom: 1px solid var(--border-color);
}

.target {
    font-size: var(--font-size-md);
    color: var(--text-secondary);
    font-family: 'SF Mono', monospace;
}

.stats {
    display: flex;
    align-items: center;
    gap: var(--spacing-sm);
    font-size: var(--font-size-md);
}

.stat {
    color: var(--text-muted);
}

.stat.success {
    color: var(--success);
}

.divider {
    color: var(--border-color);
}

.results-list {
    display: flex;
    flex-direction: column;
    gap: var(--spacing-xs);
}

.result-item {
    display: flex;
    align-items: center;
    gap: var(--spacing-md);
    padding: var(--spacing-sm) var(--spacing-md);
    background: var(--bg-secondary);
    border-radius: var(--radius-sm);
    border-left: 3px solid transparent;
    transition: all 0.2s ease;
}

.result-item:hover {
    transform: translateX(4px);
}

.result-item.success {
    border-left-color: var(--success);
}

.result-item.timeout {
    border-left-color: var(--danger);
}

.seq {
    font-size: var(--font-size-md);
    color: var(--text-muted);
    font-family: monospace;
    min-width: 28px;
}

.time {
    font-size: var(--font-size-md);
    font-weight: 600;
    color: var(--text-primary);
    font-family: 'SF Mono', monospace;
    min-width: 60px;
}

.time.timeout {
    color: var(--danger);
    font-weight: 400;
}

.status {
    margin-left: auto;
}

.status-icon {
    width: 16px;
    height: 16px;
}

.status-icon.success {
    color: var(--success);
}

.status-icon.fail {
    color: var(--danger);
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

    .ping-btn {
        width: 100%;
    }

    .results-header {
        flex-direction: column;
        align-items: flex-start;
        gap: var(--spacing-xs);
    }

    .result-item:hover {
        transform: none;
    }
}
</style>
