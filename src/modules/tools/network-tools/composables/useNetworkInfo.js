import { ref, computed, onMounted, onBeforeUnmount } from 'vue';

export function useNetworkInfo() {
    const publicIp = ref('获取中...');
    const ipLocation = ref('');
    const localIp = ref('获取中...');
    const networkInterface = ref('');
    const online = ref(navigator.onLine);
    const connectionType = ref('未知');
    const effectiveType = ref('');
    const rtt = ref('');
    const isLoading = ref(false);

    let networkListenerSetup = false;

    const handleOnline = () => {
        online.value = true;
        getConnectionInfo();
    };

    const handleOffline = () => {
        online.value = false;
    };

    const setupNetworkListeners = () => {
        if (networkListenerSetup) return;
        window.addEventListener('online', handleOnline);
        window.addEventListener('offline', handleOffline);
        networkListenerSetup = true;
    };

    const removeNetworkListeners = () => {
        window.removeEventListener('online', handleOnline);
        window.removeEventListener('offline', handleOffline);
    };

    const getPublicIp = async () => {
        publicIp.value = '获取中...';
        ipLocation.value = '';

        try {
            const controller = new AbortController();
            const timeoutId = setTimeout(() => controller.abort(), 5000);
            const response = await fetch('https://ipapi.co/json/', { signal: controller.signal });
            clearTimeout(timeoutId);
            const data = await response.json();
            publicIp.value = data.ip || '未知';
            ipLocation.value = data.country_name ? `${data.country_name}${data.city ? ', ' + data.city : ''}` : '';
        } catch {
            try {
                const controller = new AbortController();
                const timeoutId = setTimeout(() => controller.abort(), 5000);
                const response = await fetch('https://api.ipify.org?format=json', { signal: controller.signal });
                clearTimeout(timeoutId);
                const data = await response.json();
                publicIp.value = data.ip || '未知';
            } catch {
                publicIp.value = '获取失败';
            }
        }
    };

    const getLocalIp = () => {
        localIp.value = '获取中...';
        networkInterface.value = '';

        try {
            const pc = new RTCPeerConnection({ iceServers: [] });
            pc.createDataChannel('');
            pc.createOffer().then(offer => pc.setLocalDescription(offer));

            const timeout = setTimeout(() => {
                if (localIp.value === '获取中...') {
                    localIp.value = '超时';
                    networkInterface.value = '';
                    pc.close();
                }
            }, 5000);

            pc.onicecandidate = (event) => {
                if (!event.candidate) return;

                const ipRegex = /([0-9]{1,3}(\.[0-9]{1,3}){3}|[a-f0-9:]+)/;
                const match = event.candidate.candidate.match(ipRegex);

                if (match && localIp.value === '获取中...') {
                    const ip = match[0];
                    if (!ip.startsWith('192.168') && !ip.startsWith('10.') && !ip.startsWith('172.')) {
                        localIp.value = ip;
                        networkInterface.value = '公网IP';
                    } else if (localIp.value === '获取中...' || localIp.value === '超时') {
                        localIp.value = ip;
                        networkInterface.value = '局域网';
                    }
                    clearTimeout(timeout);
                    pc.close();
                }
            };

            pc.onicegatheringstatechange = () => {
                if (pc.iceGatheringState === 'complete') {
                    clearTimeout(timeout);
                    if (localIp.value === '获取中...') {
                        localIp.value = '未找到';
                        networkInterface.value = '';
                    }
                }
            };
        } catch {
            localIp.value = '获取失败';
            networkInterface.value = '';
        }
    };

    const getConnectionInfo = () => {
        const conn = navigator.connection || navigator.mozConnection || navigator.webkitConnection;

        if (conn) {
            connectionType.value = conn.type || '未知';
            effectiveType.value = conn.effectiveType || '';
            rtt.value = conn.rtt ? conn.rtt + 'ms' : '';

            const updateConnectionInfo = () => {
                effectiveType.value = conn.effectiveType || '';
                rtt.value = conn.rtt ? conn.rtt + 'ms' : '';
            };

            conn.addEventListener('change', updateConnectionInfo);
        } else {
            connectionType.value = '不支持';
            effectiveType.value = '';
            rtt.value = '';
        }
    };

    const refresh = async () => {
        isLoading.value = true;
        getLocalIp();
        await getPublicIp();
        getConnectionInfo();
        setTimeout(() => {
            isLoading.value = false;
        }, 800);
    };

    const init = () => {
        setupNetworkListeners();
        getLocalIp();
        getPublicIp();
        getConnectionInfo();
    };

    onMounted(() => {
        init();
    });

    onBeforeUnmount(() => {
        removeNetworkListeners();
    });

    return {
        publicIp,
        ipLocation,
        localIp,
        networkInterface,
        online,
        connectionType,
        effectiveType,
        rtt,
        isLoading,
        getPublicIp,
        getLocalIp,
        getConnectionInfo,
        refresh,
        init
    };
}
