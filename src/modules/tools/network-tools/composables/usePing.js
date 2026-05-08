import { ref, computed } from 'vue';

export function usePing() {
    const target = ref('google.com');
    const results = ref([]);
    const isPinging = ref(false);
    const isComplete = ref(false);

    const successRate = computed(() => {
        if (results.value.length === 0) return 0;
        const success = results.value.filter(r => r.type === 'success').length;
        return Math.round((success / results.value.length) * 100);
    });

    const avgTime = computed(() => {
        const successResults = results.value.filter(r => r.type === 'success');
        if (successResults.length === 0) return 0;
        const total = successResults.reduce((sum, r) => sum + r.time, 0);
        return Math.round(total / successResults.length);
    });

    const start = async (count = 5, interval = 500) => {
        if (!target.value) return;

        results.value = [];
        isPinging.value = true;
        isComplete.value = false;

        for (let i = 0; i < count; i++) {
            const startTime = performance.now();

            try {
                const controller = new AbortController();
                const timeoutId = setTimeout(() => controller.abort(), 5000);

                await fetch(`https://${target.value}`, {
                    method: 'HEAD',
                    mode: 'no-cors',
                    signal: controller.signal
                });

                clearTimeout(timeoutId);
                const endTime = performance.now();
                const time = Math.round(endTime - startTime);
                results.value.push({ time, status: '成功', type: 'success' });
            } catch {
                results.value.push({ time: 0, status: '超时', type: 'timeout' });
            }

            if (i < count - 1) {
                await new Promise(resolve => setTimeout(resolve, interval));
            }
        }

        isPinging.value = false;
        isComplete.value = true;
    };

    const reset = () => {
        results.value = [];
        isPinging.value = false;
        isComplete.value = false;
    };

    return {
        target,
        results,
        isPinging,
        isComplete,
        successRate,
        avgTime,
        start,
        reset
    };
}
