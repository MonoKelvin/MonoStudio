import { ref, onMounted } from 'vue';

export function useUserAgent() {
    const isLoading = ref(true);
    const info = ref({
        browser: '',
        browserVersion: '',
        os: '',
        osVersion: '',
        device: '',
        engine: '',
        platform: '',
        vendor: ''
    });

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

        let os = '未知';
        let osVersion = '';

        if (ua.includes('Windows NT 10')) {
            os = 'Windows';
            if (ua.includes('Windows NT 10')) {
                osVersion = '11';
            }
        } else if (ua.includes('Windows NT 6.3')) {
            os = 'Windows';
            osVersion = '8.1';
        } else if (ua.includes('Windows NT 6.2')) {
            os = 'Windows';
            osVersion = '8';
        } else if (ua.includes('Windows NT 6.1')) {
            os = 'Windows';
            osVersion = '7';
        } else if (ua.includes('Windows Phone')) {
            os = 'Windows Phone';
            const match = ua.match(/Windows Phone ([\d.]+)/);
            osVersion = match ? match[1] : '';
        } else if (ua.includes('Mac OS X')) {
            os = 'macOS';
            const match = ua.match(/Mac OS X (\d+[._]\d+)/);
            if (match) {
                const versionParts = match[1].replace('_', '.').split('.');
                const majorVersion = parseInt(versionParts[0]);
                if (majorVersion >= 14) {
                    osVersion = 'Sonoma';
                } else if (majorVersion >= 13) {
                    osVersion = 'Ventura';
                } else if (majorVersion >= 12) {
                    osVersion = 'Monterey';
                } else if (majorVersion >= 11) {
                    osVersion = 'Big Sur';
                } else {
                    osVersion = match[1].replace('_', '.');
                }
            }
        } else if (ua.includes('Linux')) {
            os = 'Linux';
            if (ua.includes('Ubuntu')) {
                os = 'Ubuntu';
            } else if (ua.includes('Fedora')) {
                os = 'Fedora';
            } else if (ua.includes('Debian')) {
                os = 'Debian';
            } else if (ua.includes('Arch')) {
                os = 'Arch Linux';
            }
        } else if (ua.includes('Android')) {
            os = 'Android';
            const match = ua.match(/Android (\d+(\.\d+)?)/);
            osVersion = match ? match[1] : '';
        } else if (ua.includes('iPhone') || ua.includes('iPad')) {
            os = 'iOS';
            const match = ua.match(/OS (\d+_\d+)/);
            if (match) {
                osVersion = match[1].replace('_', '.');
            }
        } else if (ua.includes('CrOS')) {
            os = 'Chrome OS';
        }

        let device = '桌面';
        if (ua.includes('Mobile') && !ua.includes('Windows Phone')) {
            device = '手机';
        }
        if (ua.includes('iPad')) {
            device = '平板';
        }
        if (ua.includes('Tablet')) {
            device = '平板';
        }

        const platform = navigator.platform || '';

        const vendor = navigator.vendor || '';

        info.value = {
            browser,
            browserVersion,
            os,
            osVersion,
            device,
            engine,
            platform,
            vendor
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
