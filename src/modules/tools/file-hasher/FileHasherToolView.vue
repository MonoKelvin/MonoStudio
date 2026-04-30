<template>
    <div class="file-hasher">
        <div class="file-select-area" 
            :class="{ 'drag-over': isDragOver }"
            @click="selectFile"
            @dragover.prevent="handleDragOver"
            @dragleave.prevent="handleDragLeave"
            @drop.prevent="handleDrop">
            <div class="file-icon">
                <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5">
                    <path d="M14 2H6a2 2 0 0 0-2 2v16a2 2 0 0 0 2 2h12a2 2 0 0 0 2-2V8z" />
                    <polyline points="14 2 14 8 20 8" />
                    <line x1="16" y1="13" x2="8" y2="13" />
                    <line x1="16" y1="17" x2="8" y2="17" />
                    <polyline points="10 9 9 9 8 9" />
                </svg>
            </div>
            <span class="select-title">点击选择文件或拖放到此处</span>
        </div>

        <div v-if="selectedFile" class="file-info">
            <table class="info-table">
                <tr>
                    <td class="info-label">文件名</td>
                    <td class="info-value file-name" :title="getFilePath(selectedFile)">{{ truncateMiddle(getFilePath(selectedFile)) }}</td>
                    <td class="info-actions" rowspan="3">
                        <BaseButton size="sm" variant="secondary" @click.stop="clearFile">清除文件</BaseButton>
                    </td>
                </tr>
                <tr>
                    <td class="info-label">文件大小</td>
                    <td class="info-value">{{ formatFileSize(selectedFile.size) }}</td>
                </tr>
                <tr>
                    <td class="info-label">文件类型</td>
                    <td class="info-value">{{ selectedFile.type || '未知类型' }}</td>
                </tr>
            </table>
        </div>

        <div class="hash-options">
            <label>哈希算法</label>
            <BaseSelect v-model="selectedAlgorithm" :options="hashAlgorithms" />
        </div>

        <div class="hash-result-section">
            <div class="result-header">
                <label>{{ selectedAlgorithm }} 哈希值</label>
                <BaseButton size="sm" variant="secondary" @click="copyHash" :disabled="!hashResult">
                    复制
                </BaseButton>
            </div>
            <div class="result-display">
                <code v-if="hashResult" class="hash-text">{{ hashResult }}</code>
                <span v-else class="hash-placeholder">选择文件后自动计算</span>
            </div>
        </div>

        <div v-if="statusMessage" class="status-message" :class="statusType">
            {{ statusMessage }}
        </div>

        <input type="file" ref="fileInput" style="display: none" @change="handleFileSelect" />
    </div>
</template>

<script>
import { ref, watch } from 'vue';
import BaseButton from '../../../components/base/BaseButton.vue';
import BaseSelect from '../../../components/base/BaseSelect.vue';

export default {
    name: 'FileHasherToolView',
    components: {
        BaseButton,
        BaseSelect
    },
    setup() {
        const selectedFile = ref(null);
        const selectedAlgorithm = ref('SHA-256');
        const hashResult = ref('');
        const statusMessage = ref('');
        const statusType = ref('info');
        const isDragOver = ref(false);
        const fileInput = ref(null);

        const hashAlgorithms = [
            { label: 'SHA-1', value: 'SHA-1' },
            { label: 'SHA-256', value: 'SHA-256' },
            { label: 'SHA-512', value: 'SHA-512' }
        ];

        const selectFile = () => {
            fileInput.value?.click();
        };

        const handleFileSelect = (event) => {
            const file = event.target?.files?.[0];
            if (file) {
                processFile(file);
            }
        };

        const handleDragOver = () => {
            isDragOver.value = true;
        };

        const handleDragLeave = () => {
            isDragOver.value = false;
        };

        const handleDrop = async (event) => {
            isDragOver.value = false;
            const file = event.dataTransfer?.files?.[0];
            if (file) {
                await processFile(file);
            }
        };

        const getFilePath = (file) => {
            if (!file) return '';
            return file.path || file.name || '';
        };

        const processFile = async (file) => {
            selectedFile.value = file;
            hashResult.value = '';
            await calculateHash(file);
        };

        const calculateHash = async (file) => {
            setStatus('正在计算哈希值...', 'info');
            
            try {
                const hash = await computeHash(file, selectedAlgorithm.value);
                hashResult.value = hash;
                setStatus('哈希值计算完成', 'success');
            } catch (error) {
                setStatus('计算哈希值失败: ' + error.message, 'error');
                hashResult.value = '';
            }
        };

        const computeHash = (file, algorithm) => {
            return new Promise((resolve, reject) => {
                const reader = new FileReader();
                reader.onload = async (e) => {
                    try {
                        const arrayBuffer = e.target.result;
                        const hashBuffer = await crypto.subtle.digest(algorithm, arrayBuffer);
                        const hashArray = Array.from(new Uint8Array(hashBuffer));
                        const hashHex = hashArray.map(b => b.toString(16).padStart(2, '0')).join('');
                        resolve(hashHex);
                    } catch (error) {
                        reject(error);
                    }
                };
                reader.onerror = () => reject(new Error('文件读取失败'));
                reader.readAsArrayBuffer(file);
            });
        };

        const formatFileSize = (bytes) => {
            if (bytes === 0) return '0 B';
            const k = 1024;
            const sizes = ['B', 'KB', 'MB', 'GB', 'TB'];
            const i = Math.floor(Math.log(bytes) / Math.log(k));
            return parseFloat((bytes / Math.pow(k, i)).toFixed(2)) + ' ' + sizes[i];
        };

        const truncateMiddle = (str, maxLen = 60) => {
            if (!str || str.length <= maxLen) return str;
            const half = Math.floor((maxLen - 3) / 2);
            return str.slice(0, half) + '...' + str.slice(-half);
        };

        const clearFile = () => {
            selectedFile.value = null;
            hashResult.value = '';
            statusMessage.value = '';
            if (fileInput.value) {
                fileInput.value.value = '';
            }
        };

        const setStatus = (message, type = 'info') => {
            statusMessage.value = message;
            statusType.value = type;
        };

        const copyHash = async () => {
            if (!hashResult.value) return;
            try {
                await navigator.clipboard.writeText(hashResult.value);
                setStatus('已复制到剪贴板', 'success');
            } catch (err) {
                setStatus('复制失败', 'error');
            }
        };

        watch(selectedAlgorithm, async (newVal, oldVal) => {
            if (newVal !== oldVal && selectedFile.value) {
                await calculateHash(selectedFile.value);
            }
        });

        return {
            selectedFile,
            selectedAlgorithm,
            hashResult,
            statusMessage,
            statusType,
            isDragOver,
            fileInput,
            hashAlgorithms,
            selectFile,
            handleFileSelect,
            handleDragOver,
            handleDragLeave,
            handleDrop,
            getFilePath,
            formatFileSize,
            truncateMiddle,
            clearFile,
            copyHash
        };
    }
};
</script>

<style scoped>
.file-hasher {
    display: flex;
    flex-direction: column;
    gap: var(--spacing-md);
    padding: var(--spacing-md);
}

.file-select-area {
    border: 2px dashed var(--border-color);
    border-radius: var(--radius-md);
    padding: var(--spacing-lg);
    display: flex;
    align-items: center;
    justify-content: center;
    gap: var(--spacing-md);
    transition: all 0.2s ease;
    cursor: pointer;
    background: var(--bg-secondary);
}

.file-select-area:hover {
    border-color: var(--accent-primary);
    background: color-mix(in srgb, var(--accent-primary) 4%, var(--bg-secondary));
}

.file-select-area.drag-over {
    border-color: var(--accent-primary);
    background: color-mix(in srgb, var(--accent-primary) 8%, var(--bg-secondary));
}

.file-icon {
    width: 32px;
    height: 32px;
    color: var(--text-muted);
    flex-shrink: 0;
}

.file-icon svg {
    width: 100%;
    height: 100%;
}

.select-title {
    font-size: var(--font-size-md);
    font-weight: 500;
    color: var(--text-primary);
    text-align: center;
}

.file-info {
    border: 1px solid var(--border-color);
    border-radius: var(--radius-md);
    background: var(--bg-secondary);
    overflow: hidden;
}

.info-table {
    width: 100%;
    border-collapse: collapse;
}

.info-table tr {
    border-bottom: 1px solid var(--border-color);
}

.info-table tr:last-child {
    border-bottom: none;
}

.info-table td {
    padding: var(--spacing-sm) var(--spacing-md);
    font-size: var(--font-size-md);
    vertical-align: middle;
}

.info-label {
    color: var(--text-muted);
    white-space: nowrap;
    width: 60px;
    text-align: right;
}

.info-value {
    color: var(--text-primary);
    width: 100%;
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
}

.file-name {
    font-weight: 500;
}

.info-actions {
    text-align: center;
    background: var(--bg-tertiary);
    border-left: 1px solid var(--border-color);
    white-space: nowrap;
    padding: var(--spacing-sm) var(--spacing-md);
}

.hash-options {
    display: flex;
    flex-direction: column;
    gap: var(--spacing-xs);
}

.hash-options label {
    font-weight: 500;
    color: var(--text-primary);
    font-size: var(--font-size-md);
}

.hash-result-section {
    border: 1px solid var(--border-color);
    border-radius: var(--radius-md);
    background: var(--bg-secondary);
    overflow: hidden;
}

.result-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    padding: var(--spacing-sm) var(--spacing-md);
    background: var(--bg-tertiary);
}

.result-header label {
    font-weight: 500;
    color: var(--text-primary);
    font-size: var(--font-size-md);
}

.result-display {
    padding: var(--spacing-md);
    background: var(--bg-primary);
}

.hash-text {
    font-family: 'Courier New', monospace;
    font-size: var(--font-size-md);
    font-weight: 500;
    color: var(--text-primary);
    word-break: break-all;
    line-height: 1.6;
    display: block;
}

.hash-placeholder {
    font-size: var(--font-size-md);
    color: var(--text-muted);
}

.status-message {
    padding: var(--spacing-sm) var(--spacing-md);
    border-radius: var(--radius-sm);
    font-size: var(--font-size-md);
    text-align: center;
}

.status-message.info {
    background: color-mix(in srgb, var(--accent-primary) 8%, transparent);
    color: var(--accent-primary);
}

.status-message.success {
    background: color-mix(in srgb, var(--success) 8%, transparent);
    color: var(--success);
}

.status-message.error {
    background: color-mix(in srgb, var(--danger) 8%, transparent);
    color: var(--danger);
}
</style>