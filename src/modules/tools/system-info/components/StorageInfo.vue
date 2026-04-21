<template>
    <div class="storage-info">
        <div v-for="(drive, index) in drives" :key="index" class="storage-item">
            <div class="storage-header">
                <span class="label">{{ formatMountPoint(drive) }}</span>
                <span class="storage-percent">{{ calculateRemainingPercent(drive) }}% 剩余</span>
            </div>
            <div class="storage-bar">
                <div class="storage-progress" :style="{ width: Math.max(0, drive.percent || 0) + '%' }"
                    :data-percent="getProgressStatus(drive)">
                </div>
            </div>
            <div class="storage-details">
                <span class="storage-used">已用: {{ drive.used || '0 GB' }}</span>
                <span class="storage-total">总共: {{ drive.total || '0 GB' }}</span>
                <span class="storage-free">剩余: {{ drive.free || calculateFreeSpace(drive) }}</span>
            </div>
        </div>
        <div v-if="drives.length === 0" class="info-item">
            <span class="label">存储设备:</span>
            <span class="value">未检测到存储设备</span>
        </div>
    </div>
</template>

<script setup>
import { computed } from 'vue';

defineProps({
    drives: {
        type: Array,
        default: () => []
    }
});

const formatMountPoint = (drive) => {
    if (!drive) return '未知驱动器';
    const mount = drive.mount || '';
    const name = drive.name || '';
    // 如果是Windows驱动器格式 C:
    if (mount.includes(':')) {
        return name ? `${name} (${mount})` : mount;
    }
    // Linux/macOS 路径
    return name || mount || '未知驱动器';
};

const calculateRemainingPercent = (drive) => {
    const usedPercent = parseFloat(drive.percent || 0);
    const remainingPercent = 100 - Math.max(0, usedPercent);
    return Math.round(remainingPercent);
};

const calculateFreeSpace = (drive) => {
    // 如果已经有 free 字段，直接使用
    if (drive.free) {
        return drive.free;
    }

    if (!drive.total || !drive.used) {
        return '0 GB';
    }

    // 尝试解析为数字
    const totalMatch = drive.total.match(/([\d.]+)\s*GB/);
    const usedMatch = drive.used.match(/([\d.]+)\s*GB/);

    if (totalMatch && usedMatch) {
        const total = parseFloat(totalMatch[1]);
        const used = parseFloat(usedMatch[1]);
        const free = Math.max(0, total - used);
        return `${free.toFixed(2)} GB`;
    }

    return '0 GB';
};

const getProgressColor = (drive) => {
    const usedPercent = parseFloat(drive.percent || 0);
    const safePercent = Math.max(0, Math.min(100, usedPercent));

    if (safePercent < 70) {
        return 'var(--success-color, #10b981)';
    } else if (safePercent < 90) {
        return 'var(--warning-color, #f59e0b)';
    } else {
        return 'var(--danger-color, #ef4444)';
    }
};

function getProgressStatus(drive) {
    const usedPercent = parseFloat(drive.percent || 0);
    const remainingPercent = 100 - Math.max(0, Math.min(100, usedPercent));

    if (remainingPercent >= 80) {
        return 'very-low';
    } else if (remainingPercent >= 60) {
        return 'low';
    } else if (remainingPercent >= 40) {
        return 'medium-low';
    } else if (remainingPercent >= 20) {
        return 'medium';
    } else {
        return 'high';
    }
}
</script>

<style scoped>
.storage-info {
    width: 100%;
}

.storage-item {
    margin-bottom: 4px;
    background: var(--surface-2, #ffffff);
    border-radius: var(--radius-sm, 6px);
    padding: 10px 14px;
    box-shadow: 0 1px 2px rgba(0, 0, 0, 0.04);
    border: 1px solid var(--border-color, #e5e7eb);
    transition: all 0.3s ease;
    animation: slideInUp 0.4s ease forwards;
    opacity: 0;
    transform: translateY(8px);
}

.storage-item:hover {
    box-shadow: 0 4px 12px rgba(0, 0, 0, 0.08);
    border-color: var(--accent-color, #3b82f6);
}

@keyframes slideInUp {
    to {
        opacity: 1;
        transform: translateY(0);
    }
}

.storage-header {
    display: flex;
    justify-content: space-between;
    align-items: center;
    margin-bottom: 4px;
}

.storage-header .label {
    font-size: 16px;
    font-weight: 500;
    color: var(--text-primary, #111827);
}

.storage-percent {
    font-size: 14px;
    font-weight: 600;
    color: var(--accent-color, #3b82f6);
}

.storage-bar {
    height: 6px;
    background-color: var(--surface-3, #f3f4f6);
    border-radius: var(--radius-sm, 3px);
    overflow: hidden;
    margin-bottom: 6px;
    box-shadow: inset 0 1px 1px rgba(0, 0, 0, 0.08);
    position: relative;
}

.storage-progress {
    height: 100%;
    border-radius: 2px;
    transition: width 0.6s cubic-bezier(0.4, 0, 0.2, 1), background 0.3s ease;
    position: relative;
    overflow: hidden;
}

/* 不同状态的进度条样式 */
.storage-progress[data-percent="very-low"] {
    background: linear-gradient(90deg, #10b981, #34d399);
}

.storage-progress[data-percent="low"] {
    background: linear-gradient(90deg, #10b981, #6ee7b7);
}

.storage-progress[data-percent="medium-low"] {
    background: linear-gradient(90deg, #84cc16, #a3e635);
}

.storage-progress[data-percent="medium"] {
    background: linear-gradient(90deg, #f59e0b, #fbbf24);
}

.storage-progress[data-percent="high"] {
    background: linear-gradient(90deg, #ef4444, #f87171);
}

/* 进度条光泽效果 */
.storage-progress::before {
    content: '';
    position: absolute;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    background: linear-gradient(90deg, transparent, rgba(255, 255, 255, 0.6), transparent);
    animation: progress-shine 1.5s infinite;
}

/* 基本光泽动画 */
@keyframes progress-shine {
    0% {
        transform: translateX(-100%);
    }

    100% {
        transform: translateX(100%);
    }
}



.storage-details {
    display: flex;
    justify-content: space-between;
    font-size: 12px;
    color: var(--text-secondary, #4b5563);
    flex-wrap: wrap;
    gap: 6px;
    animation: fadeIn 0.3s ease 0.2s forwards;
    opacity: 0;
}

@keyframes fadeIn {
    to {
        opacity: 1;
    }
}

.storage-used {
    font-weight: 500;
}

.storage-total {
    opacity: 0.8;
}

.storage-free {
    font-weight: 500;
    color: var(--success-color, #10b981);
}

.info-item {
    padding: 12px;
    background-color: var(--surface-2, #ffffff);
    border: 1px solid var(--border-color, #e5e7eb);
    border-radius: var(--radius-md, 8px);
    display: flex;
    justify-content: space-between;
    align-items: center;
    box-shadow: 0 1px 3px rgba(0, 0, 0, 0.05);
    animation: slideInUp 0.4s ease forwards;
    opacity: 0;
    transform: translateY(10px);
}

.info-item .label {
    font-size: 14px;
    font-weight: 500;
    color: var(--text-primary, #111827);
}

.info-item .value {
    font-size: 14px;
    color: var(--text-secondary, #6b7280);
}

/* 响应式设计 */
@media (max-width: 768px) {
    .storage-item {
        margin-bottom: 20px;
        padding: 14px;
    }

    .storage-header {
        margin-bottom: 10px;
    }

    .storage-bar {
        height: 10px;
    }

    .storage-details {
        font-size: 13px;
    }

    .info-item {
        padding: 14px;
    }
}
</style>
