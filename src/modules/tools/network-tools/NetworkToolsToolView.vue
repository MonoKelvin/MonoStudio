<template>
    <BasePanel class="panel network-tools-panel">
        <div class="network-tools-container">
            <div class="info-header">
                <div class="header-title">
                    <div class="header-icon">
                        <BaseSvgIcon :icon="networkIcon" :size="20" color="var(--accent-color)" />
                    </div>
                    <h3>{{ toolName }}</h3>
                </div>
                <div class="header-actions">
                    <BaseButton @click="handleRefresh" class="refresh-btn" :disabled="isLoading">
                        <span class="refresh-icon" :class="{ spinning: isLoading }">
                            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                                <path d="M23 4v6h-6"/>
                                <path d="M1 20v-6h6"/>
                                <path d="M3.51 9a9 9 0 0 1 14.85-3.36L23 10"/>
                                <path d="M20.49 15a9 9 0 0 1-14.85 3.36L1 14"/>
                            </svg>
                        </span>
                        {{ isLoading ? '刷新中...' : '刷新' }}
                    </BaseButton>
                </div>
            </div>

            <div class="tool-sections">
                <section class="tool-section overview-section">
                    <NetworkOverview
                        :public-ip="publicIp"
                        :ip-location="ipLocation"
                        :local-ip="localIp"
                        :network-interface="networkInterface"
                        :online="online"
                        :connection-type="connectionType"
                        :effective-type="effectiveType"
                        :rtt="rtt"
                    />
                </section>

                <section class="tool-section">
                    <SpeedTest :online="online" />
                </section>

                <div class="sections-grid three-cols">
                    <section class="tool-section">
                        <PingTest />
                    </section>

                    <section class="tool-section">
                        <DnsLookup />
                    </section>

                    <section class="tool-section">
                        <IpQuery />
                    </section>
                </div>

                <section class="tool-section">
                    <UserAgentParser />
                </section>
            </div>
        </div>
    </BasePanel>
</template>

<script>
import BasePanel from '../../../components/base/BasePanel.vue';
import BaseButton from '../../../components/base/BaseButton.vue';
import BaseSvgIcon from '../../../components/base/BaseSvgIcon.vue';
import { NetworkOverview, SpeedTest, PingTest, DnsLookup, IpQuery, UserAgentParser } from './components/index.js';
import { useNetworkInfo } from './composables/useNetworkInfo.js';
import networkIcon from '../../../assets/icons/network-tools.svg?raw';
import { toastService } from '../../../common/services/toastService.js';

export default {
    name: 'NetworkToolsToolView',
    components: {
        BasePanel,
        BaseButton,
        BaseSvgIcon,
        NetworkOverview,
        SpeedTest,
        PingTest,
        DnsLookup,
        IpQuery,
        UserAgentParser
    },
    props: {
        toolName: {
            type: String,
            default: '网络工具'
        },
        toolDescription: {
            type: String,
            default: '网络连接测试和诊断'
        }
    },
    setup() {
        const {
            publicIp,
            ipLocation,
            localIp,
            networkInterface,
            online,
            connectionType,
            effectiveType,
            rtt,
            isLoading,
            refresh
        } = useNetworkInfo();

        const handleRefresh = async () => {
            await refresh();
            toastService.success('刷新成功');
        };

        return {
            networkIcon,
            publicIp,
            ipLocation,
            localIp,
            networkInterface,
            online,
            connectionType,
            effectiveType,
            rtt,
            isLoading,
            handleRefresh
        };
    }
};
</script>

<style scoped>
.network-tools-panel {
    height: 100%;
    overflow: hidden;
}

.network-tools-container {
    padding: var(--spacing-md);
    height: 100%;
    display: flex;
    flex-direction: column;
    gap: var(--spacing-lg);
    overflow-y: auto;
}

.info-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    flex-shrink: 0;
}

.header-title {
    display: flex;
    align-items: center;
    gap: var(--spacing-sm);
}

.header-icon {
    width: 32px;
    height: 32px;
    display: flex;
    align-items: center;
    justify-content: center;
    background: rgba(59, 130, 246, 0.1);
    border-radius: var(--radius-sm);
}

.header-title h3 {
    margin: 0;
    font-size: var(--font-size-lg);
    font-weight: 600;
    color: var(--text-primary);
}

.header-actions {
    display: flex;
    gap: var(--spacing-sm);
}

.refresh-btn {
    display: flex;
    align-items: center;
    gap: var(--spacing-xs);
}

.refresh-icon {
    width: 16px;
    height: 16px;
    display: flex;
    align-items: center;
    justify-content: center;
}

.refresh-icon svg {
    width: 100%;
    height: 100%;
}

.refresh-icon.spinning svg {
    animation: spin 1s linear infinite;
}

@keyframes spin {
    from { transform: rotate(0deg); }
    to { transform: rotate(360deg); }
}

.tool-sections {
    display: flex;
    flex-direction: column;
    gap: var(--spacing-lg);
}

.tool-section {
    animation: fadeIn 0.3s ease;
}

@keyframes fadeIn {
    from {
        opacity: 0;
        transform: translateY(8px);
    }
    to {
        opacity: 1;
        transform: translateY(0);
    }
}

.overview-section {
    animation-delay: 0s;
}

.sections-grid {
    display: grid;
    grid-template-columns: repeat(2, 1fr);
    gap: var(--spacing-lg);
}

.sections-grid.three-cols {
    grid-template-columns: repeat(3, 1fr);
}

@media (max-width: 900px) {
    .sections-grid,
    .sections-grid.three-cols {
        grid-template-columns: repeat(2, 1fr);
    }
}

@media (max-width: 600px) {
    .info-header {
        flex-direction: column;
        align-items: flex-start;
        gap: var(--spacing-sm);
    }

    .header-actions {
        width: 100%;
    }

    .refresh-btn {
        width: 100%;
        justify-content: center;
    }

    .sections-grid,
    .sections-grid.three-cols {
        grid-template-columns: 1fr;
    }
}
</style>
