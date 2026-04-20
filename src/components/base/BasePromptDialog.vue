<template>
    <Teleport to="body">
        <Transition name="prompt-fade">
            <div v-if="visible" class="prompt-overlay" @click="handleOverlayClick">
                <Transition name="prompt-scale">
                    <div v-if="visible" class="prompt-container" :class="promptClasses" @click.stop>
                        <div class="prompt-header">
                            <h3 class="prompt-title">{{ title }}</h3>
                        </div>
                        <div class="prompt-body">
                            <div class="prompt-icon" :class="`prompt-icon-${type}`">
                                <img :src="iconSrc" alt="" width="24" height="24" />
                            </div>
                            <div class="prompt-body-content">
                                <slot>
                                    <p>{{ message }}</p>
                                </slot>
                            </div>
                        </div>
                        <div class="prompt-footer">
                            <BaseButton v-if="showCancel" class="prompt-btn-cancel" @click="handleCancel">
                                {{ cancelText }}
                            </BaseButton>
                            <BaseButton class="prompt-btn-confirm" :class="confirmButtonClass" @click="handleConfirm">
                                {{ confirmText }}
                            </BaseButton>
                        </div>
                    </div>
                </Transition>
            </div>
        </Transition>
    </Teleport>
</template>

<script>
import BaseButton from './BaseButton.vue';
import dialogInfoIcon from '../../assets/icons/dialog-info.svg';
import dialogWarningIcon from '../../assets/icons/dialog-warning.svg';
import dialogDebugIcon from '../../assets/icons/dialog-debug.svg';
import dialogErrorIcon from '../../assets/icons/dialog-error.svg';
import dialogCriticalIcon from '../../assets/icons/dialog-critical.svg';

export default {
    name: 'BasePromptDialog',
    components: {
        BaseButton
    },
    data() {
        return {
            iconMap: {
                info: dialogInfoIcon,
                warning: dialogWarningIcon,
                debug: dialogDebugIcon,
                error: dialogErrorIcon,
                critical: dialogCriticalIcon
            }
        };
    },
    props: {
        visible: {
            type: Boolean,
            default: false
        },
        type: {
            type: String,
            default: 'info',
            validator: (value) => ['info', 'warning', 'debug', 'error', 'critical'].includes(value)
        },
        title: {
            type: String,
            default: '提示'
        },
        message: {
            type: String,
            default: ''
        },
        confirmText: {
            type: String,
            default: '确定'
        },
        cancelText: {
            type: String,
            default: '取消'
        },
        showCancel: {
            type: Boolean,
            default: false
        },
        closeOnOverlay: {
            type: Boolean,
            default: true
        },
        enableSound: {
            type: Boolean,
            default: true
        }
    },
    emits: ['update:visible', 'confirm', 'cancel'],
    computed: {
        iconSrc() {
            return this.iconMap[this.type] || this.iconMap.info;
        },
        promptClasses() {
            return [`prompt-${this.type}`];
        },
        confirmButtonClass() {
            return `prompt-btn-${this.type}`;
        }
    },
    watch: {
        visible(newVal) {
            if (newVal && this.enableSound) {
                this.playSound();
            }
        }
    },
    methods: {
        playSound() {
            try {
                const audioContext = new (window.AudioContext || window.webkitAudioContext)();
                const oscillator = audioContext.createOscillator();
                const gainNode = audioContext.createGain();

                oscillator.connect(gainNode);
                gainNode.connect(audioContext.destination);

                const frequencies = {
                    info: 800,
                    warning: 600,
                    debug: 1000,
                    error: 400,
                    critical: 300
                };

                oscillator.frequency.value = frequencies[this.type] || 800;
                oscillator.type = 'sine';
                gainNode.gain.setValueAtTime(0.08, audioContext.currentTime);
                gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + 0.15);

                oscillator.start(audioContext.currentTime);
                oscillator.stop(audioContext.currentTime + 0.15);
            } catch (e) {
                console.warn('Failed to play sound:', e);
            }
        },
        handleConfirm() {
            this.$emit('confirm');
            this.$emit('update:visible', false);
        },
        handleCancel() {
            this.$emit('cancel');
            this.$emit('update:visible', false);
        },
        handleOverlayClick() {
            if (this.closeOnOverlay) {
                this.$emit('cancel');
                this.$emit('update:visible', false);
            }
        }
    }
};
</script>

<style scoped>
.prompt-overlay {
    position: fixed;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background: rgba(0, 0, 0, 0.2);
    display: flex;
    align-items: center;
    justify-content: center;
    z-index: 100000;
    padding: 24px;
}

.prompt-container {
    background: var(--background-primary);
    border-radius: 12px;
    box-shadow: 0 30px 60px 0 rgba(0, 0, 0, 0.08);
    max-width: 400px;
    width: 100%;
    overflow: hidden;
    border: 1px solid var(--border-color);
}

.prompt-header {
    display: flex;
    align-items: center;
    gap: 12px;
    padding: 12px 20px;
    border-bottom: 1px solid var(--border-color);
    background: var(--background-secondary);
}

.prompt-icon {
    flex-shrink: 0;
    width: 32px;
    height: 32px;
    display: flex;
    align-items: center;
    justify-content: center;
    border-radius: 8px;
}

.prompt-icon-info {
    color: var(--info-color);
    background: color-mix(in srgb, var(--info-color) 10%, transparent);
}

.prompt-icon-warning {
    color: var(--warning-color);
    background: color-mix(in srgb, var(--warning-color) 10%, transparent);
}

.prompt-icon-debug {
    color: var(--debug-color);
    background: color-mix(in srgb, var(--debug-color) 10%, transparent);
}

.prompt-icon-error {
    color: var(--error-color);
    background: color-mix(in srgb, var(--error-color) 10%, transparent);
}

.prompt-icon-critical {
    color: var(--critical-color);
    background: color-mix(in srgb, var(--critical-color) 10%, transparent);
}

.prompt-icon svg {
    width: 20px;
    height: 20px;
    color: currentColor;
}

.prompt-title {
    flex: 1;
    margin: 0;
    font-size: 16px;
    font-weight: 600;
    color: var(--text-primary);
}

.prompt-body {
    padding: 24px 16px 14px 16px;
    color: var(--text-primary);
    font-size: 14px;
    line-height: 1.5;
    min-height: 60px;
    display: flex;
    gap: 12px;
    background: var(--background-primary);
}

.prompt-body-content {
    flex: 1;
    min-width: 0;
}

.prompt-icon {
    flex-shrink: 0;
    width: 32px;
    height: 32px;
    display: flex;
    align-items: center;
    justify-content: center;
    border-radius: 8px;
    color: var(--text-primary);
    transform: translateY(-5.5px);
}

.prompt-body p {
    margin: 0;
    text-align: left;
}

.prompt-footer {
    display: flex;
    justify-content: flex-end;
    gap: 12px;
    padding: 10px 16px;
    border-top: 1px solid var(--border-color);
    background: var(--background-primary);
}

.prompt-btn-cancel {
    box-sizing: border-box;
    font: inherit;
    line-height: 1;
    height: 34px;
    border: 1px solid color-mix(in srgb, var(--bg-elevated) 82%, var(--text-secondary) 18%);
    border-radius: 6px;
    padding: 0 11px;
    color: var(--text-primary);
    background: color-mix(in srgb, var(--bg-elevated) 78%, var(--hover-bg) 22%);
    cursor: pointer;
    transition: background-color 0.18s ease, border-color 0.18s ease, color 0.18s ease;
}

.prompt-btn-cancel:hover {
    background: color-mix(in srgb, var(--hover-bg) 82%, var(--bg-elevated) 18%);
    border-color: color-mix(in srgb, var(--hover-bg) 65%, var(--text-secondary) 35%);
}

.prompt-btn-cancel:active {
    background: color-mix(in srgb, var(--accent-primary) 85%, var(--bg-elevated) 15%);
    border-color: color-mix(in srgb, var(--accent-primary) 60%, var(--text-secondary) 40%);
    box-shadow: none;
}

.prompt-btn-confirm {
    box-sizing: border-box;
    font: inherit;
    line-height: 1;
    height: 34px;
    color: #fff;
    background: var(--accent-color);
    border-color: var(--accent-color);
    border-radius: 6px;
    padding: 0 11px;
    cursor: pointer;
    transition: background-color 0.18s ease, border-color 0.18s ease, color 0.18s ease;
}

.prompt-btn-confirm:hover {
    background: color-mix(in srgb, var(--accent-color) 90%, #0b0d10 10%);
    border-color: color-mix(in srgb, var(--accent-color) 90%, #0b0d10 10%);
}

.prompt-btn-confirm:active {
    background: color-mix(in srgb, var(--accent-color) 78%, #0b0d10 22%);
    border-color: color-mix(in srgb, var(--accent-color) 58%, #0b0d10 42%);
    box-shadow: none;
}

.prompt-fade-enter-active,
.prompt-fade-leave-active {
    transition: opacity 0.2s ease;
}

.prompt-fade-enter-from,
.prompt-fade-leave-to {
    opacity: 0;
}

.prompt-scale-enter-active,
.prompt-scale-leave-active {
    transition: transform 0.2s ease;
}

.prompt-scale-enter-from,
.prompt-scale-leave-to {
    transform: scale(0.95);
}

[data-theme="dark"] .prompt-container {
    box-shadow: 0 30px 60px 0 rgba(0, 0, 0, 0.5);
}
</style>
