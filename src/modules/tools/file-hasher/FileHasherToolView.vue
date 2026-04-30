<template>
    <div class="file-hasher">
        <div class="file-select-area" 
            :class="{ 'drag-over': isDragOver }"
            @click="selectFile"
            @dragover.prevent="handleDragOver"
            @dragleave.prevent="handleDragLeave"
            @drop.prevent="handleDrop">
            <BaseSvgIcon :icon="fileHasherIcon" :size="32" color="var(--text-muted)" />
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
            <div class="hash-select-wrapper">
                <BaseSelect v-model="selectedAlgorithm" :options="hashAlgorithms" />
            </div>
        </div>

        <div class="hash-result-section">
            <label class="result-label">{{ selectedAlgorithm }} 哈希值</label>
            <div class="result-display">
                <code v-if="hashResult" class="hash-text">{{ hashResult }}</code>
                <span v-else class="hash-placeholder">选择文件后自动计算</span>
                <BaseButton
                  v-if="hashResult"
                  class="guid-copy-icon-btn"
                  :class="{ 'is-success': copySuccess, 'is-recovering': copyRecovering }"
                  size="sm"
                  @click="copyToClipboard(hashResult)"
                >
                  <img :src="copySuccess ? successIcon : copyIcon" alt="" aria-hidden="true" />
                </BaseButton>
            </div>
        </div>

        <input type="file" ref="fileInput" style="display: none" @change="handleFileSelect" />
    </div>
</template>

<script>
import { ref, watch, onBeforeUnmount } from 'vue';
import BaseButton from '../../../components/base/BaseButton.vue';
import BaseSelect from '../../../components/base/BaseSelect.vue';
import BaseSvgIcon from '../../../components/base/BaseSvgIcon.vue';
import fileHasherIcon from '../../../assets/icons/file-hasher.svg?raw';
import copyIcon from '../../../assets/icons/copy.svg';
import successIcon from '../../../assets/icons/check-success.svg';
import { toastService } from '../../../common/services/toastService.js';

export default {
    name: 'FileHasherToolView',
    components: {
        BaseButton,
        BaseSelect,
        BaseSvgIcon
    },
    setup() {
        const selectedFile = ref(null);
        const selectedAlgorithm = ref('SHA-256');
        const hashResult = ref('');
        const isDragOver = ref(false);
        const fileInput = ref(null);
        const copySuccess = ref(false);
        const copyRecovering = ref(false);
        const copySuccessTimer = ref(null);

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
            toastService.info('正在计算哈希值...');

            try {
                const hash = await computeHash(file, selectedAlgorithm.value);
                hashResult.value = hash;
                toastService.success('哈希值计算完成');
            } catch (error) {
                toastService.error('计算哈希值失败: ' + error.message);
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
            if (fileInput.value) {
                fileInput.value.value = '';
            }
        };

        const copyToClipboard = async (text) => {
            if (!text) return;
            try {
                await navigator.clipboard.writeText(text);
                copySuccess.value = false;
                copyRecovering.value = false;
                requestAnimationFrame(() => {
                    copySuccess.value = true;
                });
                if (copySuccessTimer.value) clearTimeout(copySuccessTimer.value);
                copySuccessTimer.value = setTimeout(() => {
                    copySuccess.value = false;
                    copyRecovering.value = true;
                    copySuccessTimer.value = setTimeout(() => {
                        copyRecovering.value = false;
                        copySuccessTimer.value = null;
                    }, 240);
                }, 1500);
            } catch {
                // ignore clipboard errors
            }
        };

        watch(selectedAlgorithm, async (newVal, oldVal) => {
            if (newVal !== oldVal && selectedFile.value) {
                await calculateHash(selectedFile.value);
            }
        });

        onBeforeUnmount(() => {
            if (copySuccessTimer.value) {
                clearTimeout(copySuccessTimer.value);
                copySuccessTimer.value = null;
            }
        });

        return {
            selectedFile,
            selectedAlgorithm,
            hashResult,
            isDragOver,
            fileInput,
            hashAlgorithms,
            fileHasherIcon,
            copyIcon,
            successIcon,
            copySuccess,
            copyRecovering,
            selectFile,
            handleFileSelect,
            handleDragOver,
            handleDragLeave,
            handleDrop,
            getFilePath,
            formatFileSize,
            truncateMiddle,
            clearFile,
            copyToClipboard
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
    padding: var(--spacing-2xl);
    display: flex;
    align-items: center;
    justify-content: center;
    gap: var(--spacing-md);
    transition: all 0.2s ease;
    cursor: pointer;
    background: var(--bg-secondary);
    user-select: none;
    -webkit-user-select: none;
}

.file-select-area:hover {
    border-color: var(--accent-hover);
    background: color-mix(in srgb, var(--accent-color) 5%, transparent);
}

.file-select-area.drag-over {
    border-color: var(--accent-hover);
    background: color-mix(in srgb, var(--accent-color) 10%, transparent);
}

.file-icon svg {
    width: 100%;
    height: 100%;
}

.select-title {
    font-size: var(--font-size-lg);
    font-weight: 500;
    color: var(--text-muted);
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
    align-items: center;
    justify-content: flex-start;
    gap: var(--spacing-md);
}

.hash-options label {
    font-weight: 500;
    color: var(--text-secondary);
    font-size: var(--font-size-md);
}

.hash-select-wrapper {
    flex-shrink: 0;
    min-width: 140px;
}

.hash-result-section {
    border: 1px solid var(--border-color);
    border-radius: var(--radius-md);
    background: var(--bg-secondary);
    overflow: hidden;
}

.result-label {
    display: block;
    font-weight: 500;
    color: var(--text-secondary);
    font-size: var(--font-size-md);
    padding: var(--spacing-sm) var(--spacing-md);
    background: var(--bg-tertiary);
}

.result-display {
    display: flex;
    align-items: center;
    justify-content: space-between;
    gap: var(--spacing-md);
    padding: var(--spacing-md);
    background: var(--bg-primary);
}

.guid-copy-icon-btn {
    flex-shrink: 0;
}

.guid-copy-icon-btn img {
    width: 16px;
    height: 16px;
    flex-shrink: 0;
}

.hash-text {
    font-family: 'Courier New', monospace;
    font-size: var(--font-size-md);
    font-weight: 500;
    color: var(--text-primary);
    word-break: break-all;
    line-height: 1.6;
    flex: 1;
    min-width: 0;
}

.hash-placeholder {
    font-size: var(--font-size-md);
    color: var(--text-muted);
    flex: 1;
    min-width: 0;
}
</style>