<template>
    <div class="speed-test-section">
        <div class="section-header">
            <div class="section-icon">
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                    <path d="M13 2L3 14h9l-1 8 10-12h-9l1-8z"/>
                </svg>
            </div>
            <h4>网速测试</h4>
        </div>

        <div class="speed-content">
            <div class="gauge-area">
                <div class="gauges-wrapper">
                    <div class="gauge-item">
                        <div class="gauge-ring">
                            <svg viewBox="0 0 120 120">
                                <defs>
                                    <linearGradient id="downloadGradient" x1="0%" y1="0%" x2="100%" y2="0%">
                                        <stop offset="0%" stop-color="#3b82f6"/>
                                        <stop offset="100%" stop-color="#60a5fa"/>
                                    </linearGradient>
                                </defs>
                                <circle
                                    class="ring-bg"
                                    cx="60"
                                    cy="60"
                                    r="50"
                                    fill="none"
                                    stroke-width="6"
                                />
                                <circle
                                    class="ring-progress download"
                                    cx="60"
                                    cy="60"
                                    r="50"
                                    fill="none"
                                    stroke-width="6"
                                    :stroke-dasharray="circumference"
                                    :stroke-dashoffset="downloadOffset"
                                />
                            </svg>
                            <div class="ring-content">
                                <span class="speed-value">{{ result.download.speed }}</span>
                                <span class="speed-unit">{{ result.download.unit }}</span>
                            </div>
                        </div>
                        <span class="gauge-title">下载速度</span>
                    </div>

                    <div class="gauge-divider"></div>

                    <div class="gauge-item">
                        <div class="gauge-ring">
                            <svg viewBox="0 0 120 120">
                                <defs>
                                    <linearGradient id="uploadGradient" x1="0%" y1="0%" x2="100%" y2="0%">
                                        <stop offset="0%" stop-color="#10b981"/>
                                        <stop offset="100%" stop-color="#34d399"/>
                                    </linearGradient>
                                </defs>
                                <circle
                                    class="ring-bg"
                                    cx="60"
                                    cy="60"
                                    r="50"
                                    fill="none"
                                    stroke-width="6"
                                />
                                <circle
                                    class="ring-progress upload"
                                    cx="60"
                                    cy="60"
                                    r="50"
                                    fill="none"
                                    stroke-width="6"
                                    :stroke-dasharray="circumference"
                                    :stroke-dashoffset="uploadOffset"
                                />
                            </svg>
                            <div class="ring-content">
                                <span class="speed-value">{{ result.upload.speed }}</span>
                                <span class="speed-unit">{{ result.upload.unit }}</span>
                            </div>
                        </div>
                        <span class="gauge-title">上传速度</span>
                    </div>
                </div>

                <div class="action-row">
                    <BaseButton
                        @click="handleStart"
                        :disabled="isTesting || !online"
                        variant="primary"
                        class="start-btn"
                    >
                        <span class="btn-spinner" v-if="isTesting"></span>
                        <svg v-else viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" class="btn-icon">
                            <polygon points="5 3 19 12 5 21 5 3"/>
                        </svg>
                        {{ isTesting ? '测试中...' : '开始测速' }}
                    </BaseButton>
                </div>
            </div>

            <div class="info-area">
                <div class="info-card">
                    <div class="info-header">
                        <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" class="info-icon">
                            <rect x="2" y="3" width="20" height="14" rx="2" ry="2"/>
                            <line x1="8" y1="21" x2="16" y2="21"/>
                            <line x1="12" y1="17" x2="12" y2="21"/>
                        </svg>
                        <span>网络状态</span>
                    </div>

                    <div class="info-grid">
                        <div class="info-cell">
                            <span class="cell-label">连接类型</span>
                            <span class="cell-value">{{ connectionInfo.type }}</span>
                        </div>
                        <div class="info-cell">
                            <span class="cell-label">有效带宽</span>
                            <span class="cell-value">{{ connectionInfo.downlink > 0 ? connectionInfo.downlink.toFixed(1) : '--' }} Mbps</span>
                        </div>
                        <div class="info-cell">
                            <span class="cell-label">网络延迟</span>
                            <span class="cell-value">{{ connectionInfo.rtt > 0 ? connectionInfo.rtt + ' ms' : '--' }}</span>
                        </div>
                        <div class="info-cell">
                            <span class="cell-label">网速等级</span>
                            <span class="cell-value" :style="{ color: speedLevel.color }">
                                {{ speedLevel.level }}
                            </span>
                        </div>
                        <div class="info-cell highlight">
                            <span class="cell-label">实际网速</span>
                            <span class="cell-value">{{ connectionInfo.downlink > 0 ? connectionInfo.downlink.toFixed(2) : '--' }} Mbps</span>
                        </div>
                    </div>
                </div>
            </div>
        </div>
    </div>
</template>

<script>
import BaseButton from '../../../../components/base/BaseButton.vue';
import { useSpeedTest } from '../composables/useSpeedTest.js';
import { useNetworkMonitor } from '../composables/useNetworkMonitor.js';
import { computed } from 'vue';

export default {
    name: 'SpeedTest',
    components: {
        BaseButton
    },
    props: {
        online: {
            type: Boolean,
            default: true
        }
    },
    setup(props) {
        const {
            isTesting,
            result,
            start,
            maxDownloadSpeed,
            maxUploadSpeed
        } = useSpeedTest();

        const { connectionInfo, getSpeedLevel } = useNetworkMonitor();

        const speedLevel = computed(() => getSpeedLevel(connectionInfo.value.downlink));

        const circumference = 2 * Math.PI * 50;

        const downloadOffset = computed(() => {
            const maxSpeed = maxDownloadSpeed?.value || 100;
            const currentSpeed = parseFloat(result.value.download.speed) || 0;
            const progress = maxSpeed > 0 ? Math.min((currentSpeed / maxSpeed) * 100, 100) : 0;
            return circumference - (progress / 100) * circumference;
        });

        const uploadOffset = computed(() => {
            const maxSpeed = maxUploadSpeed?.value || 100;
            const currentSpeed = parseFloat(result.value.upload.speed) || 0;
            const progress = maxSpeed > 0 ? Math.min((currentSpeed / maxSpeed) * 100, 100) : 0;
            return circumference - (progress / 100) * circumference;
        });

        const handleStart = () => {
            if (!props.online) return;
            start();
        };

        return {
            isTesting,
            result,
            connectionInfo,
            speedLevel,
            circumference,
            downloadOffset,
            uploadOffset,
            maxDownloadSpeed,
            maxUploadSpeed,
            handleStart
        };
    }
};
</script>

<style scoped>
.speed-test-section {
    background: var(--bg-secondary);
    border-radius: var(--radius-md);
    padding: var(--spacing-md);
    border: 1px solid var(--border-color);
}

.section-header {
    display: flex;
    align-items: center;
    gap: var(--spacing-sm);
    margin-bottom: var(--spacing-md);
}

.section-icon {
    width: 32px;
    height: 32px;
    display: flex;
    align-items: center;
    justify-content: center;
    background: rgba(245, 158, 11, 0.1);
    border-radius: var(--radius-sm);
    color: #f59e0b;
}

.section-icon svg {
    width: 16px;
    height: 16px;
}

.section-header h4 {
    margin: 0;
    font-size: var(--font-size-md);
    font-weight: 600;
    color: var(--text-primary);
}

.speed-content {
    display: flex;
    gap: var(--spacing-md);
}

.gauge-area {
    flex: 1;
    display: flex;
    flex-direction: column;
    gap: var(--spacing-sm);
}

.gauges-wrapper {
    display: flex;
    align-items: center;
    justify-content: center;
    gap: var(--spacing-lg);
    padding: var(--spacing-md);
    background: var(--bg-tertiary);
    border-radius: var(--radius-md);
}

.gauge-item {
    display: flex;
    flex-direction: column;
    align-items: center;
    gap: var(--spacing-xs);
}

.gauge-ring {
    position: relative;
    width: 120px;
    height: 120px;
}

.gauge-ring svg {
    width: 100%;
    height: 100%;
    transform: rotate(-90deg);
}

.ring-bg {
    stroke: var(--bg-tertiary);
}

.ring-progress {
    stroke-linecap: round;
    transition: stroke-dashoffset 0.3s ease-out;
}

.ring-progress.download {
    stroke: url(#downloadGradient);
}

.ring-progress.upload {
    stroke: url(#uploadGradient);
}

.ring-content {
    position: absolute;
    inset: 0;
    display: flex;
    flex-direction: column;
    align-items: center;
    justify-content: center;
}

.speed-value {
    font-size: var(--font-size-xl);
    font-weight: 700;
    color: var(--text-primary);
    font-family: 'SF Mono', 'Consolas', monospace;
    line-height: 1;
}

.speed-unit {
    font-size: var(--font-size-sm);
    color: var(--text-muted);
    margin-top: 2px;
}

.gauge-title {
    font-size: var(--font-size-sm);
    font-weight: 600;
    color: var(--text-secondary);
}

.gauge-divider {
    width: 1px;
    height: 100px;
    background: var(--border-color);
}

.action-row {
    display: flex;
    justify-content: center;
}

.start-btn {
    min-width: 120px;
}

.btn-icon {
    width: 12px;
    height: 12px;
    margin-right: 4px;
}

.btn-spinner {
    width: 12px;
    height: 12px;
    border: 2px solid rgba(255, 255, 255, 0.3);
    border-top-color: white;
    border-radius: 50%;
    animation: spin 0.8s linear infinite;
    margin-right: 4px;
    display: inline-block;
}

@keyframes spin {
    to {
        transform: rotate(360deg);
    }
}

.info-area {
    width: 180px;
    flex-shrink: 0;
}

.info-card {
    height: 100%;
    padding: var(--spacing-sm);
    background: var(--bg-tertiary);
    border-radius: var(--radius-md);
    display: flex;
    flex-direction: column;
}

.info-header {
    display: flex;
    align-items: center;
    gap: var(--spacing-xs);
    margin-bottom: var(--spacing-sm);
    padding-bottom: var(--spacing-xs);
    border-bottom: 1px solid var(--border-color);
}

.info-icon {
    width: 14px;
    height: 14px;
    color: var(--text-muted);
}

.info-header span {
    font-size: var(--font-size-md);
    font-weight: 600;
    color: var(--text-secondary);
}

.info-grid {
    display: flex;
    flex-direction: column;
    gap: var(--spacing-xs);
}

.info-cell {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: 2px 0;
}

.cell-label {
    color: var(--text-tertiary);
    font-size: var(--font-size-md);
    font-weight: 500;
}

.cell-value {
    font-size: var(--font-size-md);
    color: var(--text-primary);
    font-family: 'SF Mono', 'Consolas', monospace;
}

.highlight .cell-value {
    color: var(--accent-color);
}

@media (max-width: 600px) {
    .speed-content {
        flex-direction: column;
    }

    .info-area {
        width: 100%;
    }

    .gauges-wrapper {
        gap: var(--spacing-md);
    }

    .gauge-ring {
        width: 100px;
        height: 100px;
    }

    .speed-value {
        font-size: var(--font-size-md);
    }
}
</style>
