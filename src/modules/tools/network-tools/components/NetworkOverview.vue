<template>
    <div class="network-overview">
        <div class="info-grid">
            <div class="info-card" :class="{ loading: publicIp === '获取中...' }">
                <div class="card-icon public-ip">
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                        <circle cx="12" cy="12" r="10"/>
                        <line x1="2" y1="12" x2="22" y2="12"/>
                        <path d="M12 2a15.3 15.3 0 0 1 4 10 15.3 15.3 0 0 1-4 10 15.3 15.3 0 0 1-4-10 15.3 15.3 0 0 1 4-10z"/>
                    </svg>
                </div>
                <div class="card-content">
                    <div class="card-label">公网 IP</div>
                    <div class="card-value">{{ publicIp }}</div>
                    <div class="card-subtitle" v-if="ipLocation">{{ ipLocation }}</div>
                </div>
            </div>

            <div class="info-card" :class="{ loading: localIp === '获取中...' }">
                <div class="card-icon local-ip">
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                        <rect x="2" y="3" width="20" height="14" rx="2" ry="2"/>
                        <line x1="8" y1="21" x2="16" y2="21"/>
                        <line x1="12" y1="17" x2="12" y2="21"/>
                    </svg>
                </div>
                <div class="card-content">
                    <div class="card-label">本地 IP</div>
                    <div class="card-value">{{ localIp }}</div>
                    <div class="card-subtitle" v-if="networkInterface">{{ networkInterface }}</div>
                </div>
            </div>

            <div class="info-card" :class="{ highlight: online }">
                <div class="card-icon connection">
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                        <path d="M5 12.55a11 11 0 0 1 14.08 0"/>
                        <path d="M1.42 9a16 16 0 0 1 21.16 0"/>
                        <path d="M8.53 16.11a6 6 0 0 1 6.95 0"/>
                        <line x1="12" y1="20" x2="12.01" y2="20"/>
                    </svg>
                </div>
                <div class="card-content">
                    <div class="card-label">网络状态</div>
                    <div class="card-value" :class="online ? 'online' : 'offline'">
                        {{ online ? '在线' : '离线' }}
                    </div>
                    <div class="card-subtitle" v-if="effectiveType">{{ effectiveType }}</div>
                </div>
            </div>

            <div class="info-card">
                <div class="card-icon rtt">
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                        <polyline points="22 12 18 12 15 21 9 3 6 12 2 12"/>
                    </svg>
                </div>
                <div class="card-content">
                    <div class="card-label">连接类型</div>
                    <div class="card-value">{{ connectionType }}</div>
                    <div class="card-subtitle" v-if="rtt">延迟: {{ rtt }}</div>
                </div>
            </div>
        </div>
    </div>
</template>

<script>
export default {
    name: 'NetworkOverview',
    props: {
        publicIp: {
            type: String,
            default: '获取中...'
        },
        ipLocation: {
            type: String,
            default: ''
        },
        localIp: {
            type: String,
            default: '获取中...'
        },
        networkInterface: {
            type: String,
            default: ''
        },
        online: {
            type: Boolean,
            default: false
        },
        connectionType: {
            type: String,
            default: '未知'
        },
        effectiveType: {
            type: String,
            default: ''
        },
        rtt: {
            type: String,
            default: ''
        }
    }
};
</script>

<style scoped>
.network-overview {
    width: 100%;
}

.info-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
    gap: var(--spacing-md);
}

.info-card {
    background: var(--bg-secondary);
    border: 1px solid var(--border-color);
    border-radius: var(--radius-md);
    padding: var(--spacing-md);
    display: flex;
    align-items: flex-start;
    gap: var(--spacing-md);
    transition: all 0.25s ease;
}

.info-card:hover {
    border-color: var(--accent-color);
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.08);
    transform: translateY(-2px);
}

.info-card.highlight {
    border-color: rgba(59, 130, 246, 0.3);
    background: rgba(59, 130, 246, 0.03);
}

.info-card.loading {
    opacity: 0.7;
}

.card-icon {
    width: 40px;
    height: 40px;
    display: flex;
    align-items: center;
    justify-content: center;
    border-radius: var(--radius-sm);
    flex-shrink: 0;
}

.card-icon svg {
    width: 20px;
    height: 20px;
}

.card-icon.public-ip {
    background: rgba(59, 130, 246, 0.1);
    color: var(--accent-color);
}

.card-icon.local-ip {
    background: rgba(16, 185, 129, 0.1);
    color: var(--success);
}

.card-icon.connection {
    background: rgba(139, 92, 246, 0.1);
    color: #8b5cf6;
}

.card-icon.rtt {
    background: rgba(245, 158, 11, 0.1);
    color: #f59e0b;
}

.card-content {
    flex: 1;
    min-width: 0;
}

.card-label {
    font-size: var(--font-size-md);
    font-weight: 600;
    color: var(--text-muted);
    margin-bottom: var(--spacing-xs);
}

.card-value {
    font-size: var(--font-size-md);
    font-weight: 600;
    color: var(--text-primary);
    font-family: 'SF Mono', 'Consolas', monospace;
    word-break: break-all;
}

.card-value.online {
    color: var(--success);
}

.card-value.offline {
    color: var(--danger);
}

.card-subtitle {
    font-size: var(--font-size-md);
    color: var(--text-muted);
    margin-top: 2px;
}

@media (max-width: 768px) {
    .info-grid {
        grid-template-columns: repeat(2, 1fr);
    }
}

@media (max-width: 480px) {
    .info-grid {
        grid-template-columns: 1fr;
    }

    .info-card:hover {
        transform: none;
    }
}
</style>
