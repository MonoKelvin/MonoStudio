import { ref } from 'vue';

export function useSpeedTest() {
    const isTesting = ref(false);
    const downloadProgress = ref(0);
    const uploadProgress = ref(0);
    const maxDownloadSpeed = ref(0);
    const maxUploadSpeed = ref(0);
    const result = ref({
        download: { speed: '--', unit: 'Mbps', status: '点击开始测速' },
        upload: { speed: '--', unit: 'Mbps', status: '点击开始测速' }
    });

    const downloadUrls = [
        'https://www.google.com/images/phd/px.gif',
        'https://www.gstatic.com/images/branding/product/1x/googleg_16dp.png',
        'https://ssl.gstatic.com/s2/ozanos-ui-icons/8ea07ad27b8f848be689f4e5f7ab21db.png'
    ];

    let animationFrameId = null;
    let downloadTargetProgress = 0;
    let uploadTargetProgress = 0;
    let downloadCurrentDisplay = 0;
    let uploadCurrentDisplay = 0;

    const smoothAnimate = () => {
        const lerp = (start, end, factor) => start + (end - start) * factor;

        downloadCurrentDisplay = lerp(downloadCurrentDisplay, downloadTargetProgress, 0.15);
        uploadCurrentDisplay = lerp(uploadCurrentDisplay, uploadTargetProgress, 0.15);

        downloadProgress.value = Math.round(downloadCurrentDisplay);
        uploadProgress.value = Math.round(uploadCurrentDisplay);

        if (isTesting.value) {
            animationFrameId = requestAnimationFrame(smoothAnimate);
        }
    };

    const testWithTimeout = (url, timeout = 8000) => {
        return new Promise((resolve, reject) => {
            const startTime = performance.now();
            const controller = new AbortController();
            const timeoutId = setTimeout(() => {
                controller.abort();
                reject(new Error('Timeout'));
            }, timeout);

            fetch(url + '?r=' + Math.random(), {
                mode: 'no-cors',
                signal: controller.signal,
                cache: 'no-store'
            }).then(() => {
                clearTimeout(timeoutId);
                const endTime = performance.now();
                resolve({ duration: endTime - startTime, success: true });
            }).catch((err) => {
                clearTimeout(timeoutId);
                reject(err);
            });
        });
    };

    const uploadTest = (timeout = 8000) => {
        const data = new Uint8Array(256 * 1024);
        return new Promise((resolve, reject) => {
            const startTime = performance.now();
            const controller = new AbortController();
            const timeoutId = setTimeout(() => {
                controller.abort();
                reject(new Error('Timeout'));
            }, timeout);

            fetch('https://httpbin.org/post', {
                method: 'POST',
                body: data,
                mode: 'no-cors',
                signal: controller.signal
            }).then(() => {
                clearTimeout(timeoutId);
                const endTime = performance.now();
                resolve({ duration: endTime - startTime, success: true });
            }).catch((err) => {
                clearTimeout(timeoutId);
                reject(err);
            });
        });
    };

    const runDownloadTests = async () => {
        const successfulDownloads = [];
        const total = downloadUrls.length;

        for (let i = 0; i < total; i++) {
            downloadTargetProgress = Math.round(((i + 0.3) / total) * 85);

            try {
                const { duration } = await testWithTimeout(downloadUrls[i], 8000);
                if (duration < 7000) {
                    successfulDownloads.push({ duration, bytes: 2048 });
                    const avgDuration = successfulDownloads.reduce((sum, t) => sum + t.duration, 0) / successfulDownloads.length;
                    const totalBytes = successfulDownloads.reduce((sum, t) => sum + t.bytes, 0);
                    const speedBps = (totalBytes * 8) / (avgDuration / 1000);
                    const speedMbps = (speedBps / 1000000);
                    result.value.download.speed = speedMbps.toFixed(2);
                    if (speedMbps > maxDownloadSpeed.value) {
                        maxDownloadSpeed.value = Math.ceil(speedMbps);
                    }
                }
            } catch (error) {
                console.warn(`Download test ${i + 1} failed:`, error.message);
            }

            downloadTargetProgress = Math.round(((i + 1) / total) * 85);
            await new Promise(r => setTimeout(r, 50));
        }

        downloadTargetProgress = 100;

        if (successfulDownloads.length > 0) {
            const avgDuration = successfulDownloads.reduce((sum, t) => sum + t.duration, 0) / successfulDownloads.length;
            const totalBytes = successfulDownloads.reduce((sum, t) => sum + t.bytes, 0);
            const speedBps = (totalBytes * 8) / (avgDuration / 1000);
            const speedMbps = (speedBps / 1000000);
            result.value.download = { speed: speedMbps.toFixed(2), unit: 'Mbps', status: '下载速度' };
            if (speedMbps > maxDownloadSpeed.value) {
                maxDownloadSpeed.value = Math.ceil(speedMbps);
            }
        } else {
            result.value.download = { speed: '--', unit: 'Mbps', status: '下载速度' };
            maxDownloadSpeed.value = 0;
        }
    };

    const runUploadTests = async () => {
        const successfulUploads = [];
        const total = 3;

        for (let i = 0; i < total; i++) {
            uploadTargetProgress = Math.round(((i + 0.3) / total) * 85);

            try {
                const { duration } = await uploadTest(8000);
                if (duration < 7000) {
                    successfulUploads.push({ duration, bytes: 256 * 1024 });
                    const avgDuration = successfulUploads.reduce((sum, t) => sum + t.duration, 0) / successfulUploads.length;
                    const totalBytes = successfulUploads.reduce((sum, t) => sum + t.bytes, 0);
                    const speedBps = (totalBytes * 8) / (avgDuration / 1000);
                    const speedMbps = (speedBps / 1000000);
                    result.value.upload.speed = speedMbps.toFixed(2);
                    if (speedMbps > maxUploadSpeed.value) {
                        maxUploadSpeed.value = Math.ceil(speedMbps);
                    }
                }
            } catch (error) {
                console.warn(`Upload test ${i + 1} failed:`, error.message);
            }

            uploadTargetProgress = Math.round(((i + 1) / total) * 85);
            await new Promise(r => setTimeout(r, 50));
        }

        uploadTargetProgress = 100;

        if (successfulUploads.length > 0) {
            const avgDuration = successfulUploads.reduce((sum, t) => sum + t.duration, 0) / successfulUploads.length;
            const totalBytes = successfulUploads.reduce((sum, t) => sum + t.bytes, 0);
            const speedBps = (totalBytes * 8) / (avgDuration / 1000);
            const speedMbps = (speedBps / 1000000);
            result.value.upload = { speed: speedMbps.toFixed(2), unit: 'Mbps', status: '上传速度' };
            if (speedMbps > maxUploadSpeed.value) {
                maxUploadSpeed.value = Math.ceil(speedMbps);
            }
        } else {
            result.value.upload = { speed: '--', unit: 'Mbps', status: '上传速度' };
            maxUploadSpeed.value = 0;
        }
    };

    const start = async () => {
        isTesting.value = true;
        downloadProgress.value = 0;
        uploadProgress.value = 0;
        downloadCurrentDisplay = 0;
        uploadCurrentDisplay = 0;
        downloadTargetProgress = 0;
        uploadTargetProgress = 0;
        maxDownloadSpeed.value = 0;
        maxUploadSpeed.value = 0;

        result.value = {
            download: { speed: '0', unit: 'Mbps', status: '测试中...' },
            upload: { speed: '0', unit: 'Mbps', status: '测试中...' }
        };

        animationFrameId = requestAnimationFrame(smoothAnimate);

        await Promise.all([
            runDownloadTests(),
            runUploadTests()
        ]);

        if (animationFrameId) {
            cancelAnimationFrame(animationFrameId);
        }

        isTesting.value = false;
    };

    const reset = () => {
        result.value = {
            download: { speed: '--', unit: 'Mbps', status: '点击开始测速' },
            upload: { speed: '--', unit: 'Mbps', status: '点击开始测速' }
        };
        downloadProgress.value = 0;
        uploadProgress.value = 0;
        downloadCurrentDisplay = 0;
        uploadCurrentDisplay = 0;
        downloadTargetProgress = 0;
        uploadTargetProgress = 0;
        maxDownloadSpeed.value = 0;
        maxUploadSpeed.value = 0;
    };

    return {
        isTesting,
        downloadProgress,
        uploadProgress,
        maxDownloadSpeed,
        maxUploadSpeed,
        result,
        start,
        reset
    };
}
