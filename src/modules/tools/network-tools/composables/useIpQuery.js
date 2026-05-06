import { ref } from 'vue';

export function useIpQuery() {
    const queryTarget = ref('');
    const result = ref(null);
    const isQuerying = ref(false);

    const query = async () => {
        if (!queryTarget.value) return;

        result.value = null;
        isQuerying.value = true;

        try {
            let ip = queryTarget.value;

            if (!/^[\d.:a-f]+$/.test(ip)) {
                try {
                    const dnsResponse = await fetch(`https://dns.google/resolve?name=${ip}&type=A`, { timeout: 5000 });
                    const dnsData = await dnsResponse.json();
                    if (dnsData.Answer && dnsData.Answer.length > 0) {
                        ip = dnsData.Answer[0].data;
                    }
                } catch {
                    // DNS lookup failed, try with original input
                }
            }

            const response = await fetch(`https://ipapi.co/${ip}/json/`, { timeout: 5000 });
            const data = await response.json();

            result.value = {
                ip: ip,
                country: data.country_name || data.country || '未知',
                region: data.region || '未知',
                city: data.city || '未知',
                isp: data.isp || data.org || '未知',
                org: data.org || '未知',
                asn: data.asn || ''
            };
        } catch (error) {
            console.error('IP query failed:', error);
            result.value = {
                ip: queryTarget.value,
                country: '查询失败',
                region: '-',
                city: '-',
                isp: '-',
                org: '-',
                asn: ''
            };
        }

        isQuerying.value = false;
    };

    const reset = () => {
        result.value = null;
        isQuerying.value = false;
    };

    return {
        queryTarget,
        result,
        isQuerying,
        query,
        reset
    };
}
