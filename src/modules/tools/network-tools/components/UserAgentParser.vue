<template>
    <div class="browser-info-section">
        <div class="section-header">
            <div class="section-icon">
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                    <circle cx="12" cy="12" r="10"/>
                    <circle cx="12" cy="12" r="4"/>
                    <line x1="21.17" y1="8" x2="12" y2="8"/>
                    <line x1="3.95" y1="6.06" x2="8.54" y2="14"/>
                    <line x1="10.88" y1="21.94" x2="15.46" y2="14"/>
                </svg>
            </div>
            <h4>浏览器信息</h4>
            <span class="header-status">
                <template v-if="isLoading">正在获取...</template>
                <template v-else>
                    {{ info.browser }}{{ info.browserVersion ? ` ${info.browserVersion}` : '' }}
                </template>
            </span>
        </div>

        <div class="info-grid">
            <div class="info-item" v-if="info.browser">
                <div class="item-icon">
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                        <circle cx="12" cy="12" r="10"/>
                        <circle cx="12" cy="12" r="4"/>
                    </svg>
                </div>
                <div class="item-content">
                    <span class="item-label">浏览器</span>
                    <span class="item-value">{{ info.browser }} {{ info.browserVersion }}</span>
                </div>
            </div>

            <div class="info-item" v-if="info.engine">
                <div class="item-icon">
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                        <polygon points="12 2 2 7 12 12 22 7 12 2"/>
                        <polyline points="2 17 12 22 22 17"/>
                        <polyline points="2 12 12 17 22 12"/>
                    </svg>
                </div>
                <div class="item-content">
                    <span class="item-label">渲染引擎</span>
                    <span class="item-value">{{ info.engine }}</span>
                </div>
            </div>

            <div class="info-item" v-if="info.language">
                <div class="item-icon">
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                        <circle cx="12" cy="12" r="10"/>
                        <line x1="2" y1="12" x2="22" y2="12"/>
                        <path d="M12 2a15.3 15.3 0 0 1 4 10 15.3 15.3 0 0 1-4 10 15.3 15.3 0 0 1-4-10 15.3 15.3 0 0 1 4-10z"/>
                    </svg>
                </div>
                <div class="item-content">
                    <span class="item-label">语言</span>
                    <span class="item-value">{{ info.language }}</span>
                </div>
            </div>

            <div class="info-item" v-if="info.timezone">
                <div class="item-icon">
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                        <circle cx="12" cy="12" r="10"/>
                        <polyline points="12 6 12 12 16 14"/>
                    </svg>
                </div>
                <div class="item-content">
                    <span class="item-label">时区</span>
                    <span class="item-value">{{ info.timezone }}</span>
                </div>
            </div>

            <div class="info-item" v-if="info.devicePixelRatio">
                <div class="item-icon">
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                        <rect x="3" y="3" width="18" height="18" rx="2" ry="2"/>
                        <line x1="3" y1="9" x2="21" y2="9"/>
                        <line x1="9" y1="21" x2="9" y2="9"/>
                    </svg>
                </div>
                <div class="item-content">
                    <span class="item-label">设备像素比</span>
                    <span class="item-value">{{ info.devicePixelRatio }}x</span>
                </div>
            </div>

            <div class="info-item" v-if="info.cookiesEnabled !== undefined">
                <div class="item-icon">
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                        <path d="M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z"/>
                    </svg>
                </div>
                <div class="item-content">
                    <span class="item-label">Cookie</span>
                    <span class="item-value">{{ info.cookiesEnabled ? '已启用' : '已禁用' }}</span>
                </div>
            </div>

            <div class="info-item" v-if="info.doNotTrack !== undefined">
                <div class="item-icon">
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                        <circle cx="12" cy="12" r="10"/>
                        <line x1="4.93" y1="4.93" x2="19.07" y2="19.07"/>
                    </svg>
                </div>
                <div class="item-content">
                    <span class="item-label">跟踪保护</span>
                    <span class="item-value">{{ info.doNotTrack ? '已启用' : '未启用' }}</span>
                </div>
            </div>

            <div class="info-item" v-if="info.webGL">
                <div class="item-icon">
                    <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                        <polygon points="12 2 2 7 12 12 22 7 12 2"/>
                        <polyline points="2 17 12 22 22 17"/>
                        <polyline points="2 12 12 17 22 12"/>
                    </svg>
                </div>
                <div class="item-content">
                    <span class="item-label">WebGL</span>
                    <span class="item-value">{{ info.webGL }}</span>
                </div>
            </div>
        </div>
    </div>
</template>

<script>
import { ref, onMounted } from 'vue';

export function useUserAgent() {
    const isLoading = ref(true);
    const info = ref({
        browser: '',
        browserVersion: '',
        engine: '',
        language: '',
        timezone: '',
        devicePixelRatio: '',
        cookiesEnabled: undefined,
        doNotTrack: undefined,
        webGL: ''
    });

    const getWebGLInfo = () => {
        try {
            const canvas = document.createElement('canvas');
            const gl = canvas.getContext('webgl') || canvas.getContext('experimental-webgl');
            if (gl && gl instanceof WebGLRenderingContext) {
                const debugInfo = gl.getExtension('WEBGL_debug_renderer_info');
                if (debugInfo) {
                    const renderer = gl.getParameter(debugInfo.UNMASKED_RENDERER_WEBGL);
                    return renderer;
                }
                return '支持';
            }
            return '不支持';
        } catch (e) {
            return '未知';
        }
    };

    const parse = (ua = navigator.userAgent) => {
        isLoading.value = true;

        let browser = '未知';
        let browserVersion = '';
        let engine = '未知';

        if (ua.includes('Firefox/')) {
            browser = 'Firefox';
            const match = ua.match(/Firefox\/(\d+)/);
            browserVersion = match ? match[1] : '';
            engine = 'Gecko';
        } else if (ua.includes('Edg/')) {
            browser = 'Microsoft Edge';
            const match = ua.match(/Edg\/(\d+)/);
            browserVersion = match ? match[1] : '';
            engine = 'Blink';
        } else if (ua.includes('Chrome/') && !ua.includes('Chromium')) {
            browser = 'Chrome';
            const match = ua.match(/Chrome\/(\d+)/);
            browserVersion = match ? match[1] : '';
            engine = 'Blink';
        } else if (ua.includes('Safari/') && !ua.includes('Chrome')) {
            browser = 'Safari';
            const match = ua.match(/Version\/(\d+)/);
            browserVersion = match ? match[1] : '';
            engine = 'WebKit';
        } else if (ua.includes('Opera') || ua.includes('OPR/')) {
            browser = 'Opera';
            const match = ua.match(/OPR\/(\d+)/);
            browserVersion = match ? match[1] : '';
            engine = 'Blink';
        } else if (ua.includes('Trident/') || ua.includes('MSIE')) {
            browser = 'Internet Explorer';
            const match = ua.match(/(?:MSIE |Trident\/)\d+.*rv:(\d+)/);
            if (match) {
                browserVersion = match[1];
            }
            engine = 'Trident';
        } else if (ua.includes('Chromium')) {
            browser = 'Chromium';
            const match = ua.match(/Chromium\/(\d+)/);
            browserVersion = match ? match[1] : '';
            engine = 'Blink';
        }

        const language = navigator.language || navigator.userLanguage || '';
        const timezone = Intl.DateTimeFormat().resolvedOptions().timeZone || '';
        const devicePixelRatio = window.devicePixelRatio ? window.devicePixelRatio.toFixed(1) : '1';
        const cookiesEnabled = navigator.cookieEnabled;
        const doNotTrack = navigator.doNotTrack === '1' || navigator.doNotTrack === 'yes';
        const webGL = getWebGLInfo();

        info.value = {
            browser,
            browserVersion,
            engine,
            language,
            timezone,
            devicePixelRatio,
            cookiesEnabled,
            doNotTrack,
            webGL
        };

        isLoading.value = false;
    };

    onMounted(() => {
        parse();
    });

    return {
        isLoading,
        info,
        parse
    };
}

export default {
    name: 'UserAgentParser',
    setup() {
        const { isLoading, info } = useUserAgent();

        return {
            isLoading,
            info
        };
    }
};
</script>

<style scoped>
.browser-info-section {
    background: var(--bg-secondary);
    border-radius: var(--radius-md);
    padding: var(--spacing-md);
    border: 1px solid var(--border-color);
    transition: box-shadow 0.2s ease;
}

.browser-info-section:hover {
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.06);
}

.section-header {
    display: flex;
    align-items: center;
    gap: var(--spacing-sm);
    margin-bottom: var(--spacing-md);
    flex-wrap: wrap;
}

.section-icon {
    width: 36px;
    height: 36px;
    display: flex;
    align-items: center;
    justify-content: center;
    background: rgba(59, 130, 246, 0.1);
    border-radius: var(--radius-sm);
    color: #3b82f6;
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

.header-status {
    font-size: var(--font-size-sm);
    color: var(--text-secondary);
    background: var(--bg-tertiary);
    padding: 2px 10px;
    border-radius: var(--radius-sm);
    margin-left: auto;
}

.info-grid {
    display: flex;
    flex-wrap: wrap;
    gap: var(--spacing-sm);
}

.info-item {
    display: flex;
    align-items: center;
    gap: var(--spacing-sm);
    background: var(--bg-primary);
    border-radius: var(--radius-md);
    padding: var(--spacing-sm) var(--spacing-md);
    border: 1px solid var(--border-color);
    transition: all 0.2s ease;
    flex: 1 1 calc(50% - var(--spacing-sm) / 2);
    min-width: 200px;
}

.info-item:hover {
    border-color: var(--accent-color);
    transform: translateX(2px);
}

.item-icon {
    width: 32px;
    height: 32px;
    display: flex;
    align-items: center;
    justify-content: center;
    background: var(--bg-secondary);
    border-radius: var(--radius-sm);
    color: var(--accent-color);
    flex-shrink: 0;
    transition: all 0.2s ease;
}

.item-icon svg {
    width: 16px;
    height: 16px;
}

.item-content {
    display: flex;
    flex-direction: column;
    gap: 2px;
    min-width: 0;
}

.item-label {
    font-size: var(--font-size-sm);
    color: var(--text-muted);
}

.item-value {
    font-size: var(--font-size-sm);
    color: var(--text-primary);
    font-weight: 500;
    white-space: nowrap;
    overflow: hidden;
    text-overflow: ellipsis;
}
</style>