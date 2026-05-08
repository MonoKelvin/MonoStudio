import { ref, onMounted, onUnmounted } from 'vue';

export function useNetworkMonitor() {
    const connectionInfo = ref({
        effectiveType: '--',
        downlink: 0,
        rtt: 0,
        saveData: false,
        type: '未知'
    });

    const networkTypes = {
        'bluetooth': '蓝牙',
        'cellular': '移动网络',
        'ethernet': '以太网',
        'wifi': 'WiFi',
        'wimax': 'WiMAX',
        'none': '无连接',
        'unknown': '未知'
    };

    const updateConnectionInfo = () => {
        const connection = navigator.connection || navigator.mozConnection || navigator.webkitConnection;

        if (connection) {
            connectionInfo.value = {
                effectiveType: connection.effectiveType || '--',
                downlink: connection.downlink || 0,
                rtt: connection.rtt || 0,
                saveData: connection.saveData || false,
                type: networkTypes[connection.type] || networkTypes['unknown']
            };
        } else {
            connectionInfo.value = {
                effectiveType: '--',
                downlink: 0,
                rtt: 0,
                saveData: false,
                type: '不支持'
            };
        }
    };

    let intervalId = null;

    onMounted(() => {
        updateConnectionInfo();

        const connection = navigator.connection || navigator.mozConnection || navigator.webkitConnection;
        if (connection) {
            connection.addEventListener('change', updateConnectionInfo);
        }

        intervalId = setInterval(updateConnectionInfo, 1000);
    });

    onUnmounted(() => {
        if (intervalId) {
            clearInterval(intervalId);
        }

        const connection = navigator.connection || navigator.mozConnection || navigator.webkitConnection;
        if (connection) {
            connection.removeEventListener('change', updateConnectionInfo);
        }
    });

    const getSpeedLevel = (downlink) => {
        if (downlink >= 10) return { level: '极快', color: '#10b981' };
        if (downlink >= 5) return { level: '快速', color: '#3b82f6' };
        if (downlink >= 1) return { level: '一般', color: '#f59e0b' };
        if (downlink > 0) return { level: '较慢', color: '#ef4444' };
        return { level: '--', color: 'var(--text-muted)' };
    };

    return {
        connectionInfo,
        updateConnectionInfo,
        getSpeedLevel
    };
}
