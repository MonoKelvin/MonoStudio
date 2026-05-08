import { ref } from 'vue';

export function useDnsLookup() {
    const domain = ref('');
    const results = ref([]);
    const isLooking = ref(false);

    const lookup = async () => {
        if (!domain.value) return;

        results.value = [];
        isLooking.value = true;

        try {
            const controller = new AbortController();
            const timeoutId = setTimeout(() => controller.abort(), 10000);
            const startTime = performance.now();

            await fetch(`https://${domain.value}`, {
                method: 'HEAD',
                mode: 'no-cors',
                signal: controller.signal
            });

            clearTimeout(timeoutId);
            const endTime = performance.now();

            results.value.push({
                type: 'A',
                value: domain.value,
                ttl: Math.round(endTime - startTime)
            });
        } catch (error) {
            results.value.push({
                type: 'ERROR',
                value: error.message || '查找失败',
                ttl: null
            });
        }

        isLooking.value = false;
    };

    const reset = () => {
        results.value = [];
        isLooking.value = false;
    };

    return {
        domain,
        results,
        isLooking,
        lookup,
        reset
    };
}
