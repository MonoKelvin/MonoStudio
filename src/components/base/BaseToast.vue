<template>
    <transition :name="transitionName" appear @after-leave="$emit('close')">
        <div v-if="!isClosing" class="base-toast" :class="[positionClass, typeClass, { 'dragging': isDragging }]"
            :style="toastStyle" @click="closeOnClick && close" @mousedown="startDrag" @mouseenter="onMouseEnter"
            @mouseleave="onMouseLeave">
            <div class="toast-content">
                <div v-if="icon" class="toast-icon">{{ icon }}</div>
                <div class="toast-body" :class="{ 'selectable': selectable }">
                    <h4 v-if="title" class="toast-title">{{ title }}</h4>
                    <p v-if="message" class="toast-message">{{ message }}</p>
                    <p v-if="subtitle" class="toast-subtitle">{{ subtitle }}</p>
                </div>
                <button v-if="closable" class="toast-close" @click.stop="close">×</button>
            </div>
        </div>
    </transition>
</template>

<script>
export default {
    name: 'BaseToast',
    props: {
        type: {
            type: String,
            default: 'info',
            validator: (value) => ['info', 'success', 'warning', 'error'].includes(value)
        },
        title: {
            type: String,
            default: ''
        },
        message: {
            type: String,
            default: ''
        },
        subtitle: {
            type: String,
            default: ''
        },
        icon: {
            type: String,
            default: ''
        },
        position: {
            type: String,
            default: 'bottom-right',
            validator: (value) => [
                'top-left', 'top-center', 'top-right',
                'bottom-left', 'bottom-center', 'bottom-right',
                'left-top', 'left-center', 'left-bottom',
                'right-top', 'right-center', 'right-bottom'
            ].includes(value)
        },
        duration: {
            type: Number,
            default: 3000
        },
        stackOffset: {
            type: Number,
            default: 0
        },
        closable: {
            type: Boolean,
            default: true
        },
        selectable: {
            type: Boolean,
            default: false
        },
        closeOnClick: {
            type: Boolean,
            default: false
        },
        glassEffect: {
            type: Boolean,
            default: true
        },
        shadow: {
            type: Boolean,
            default: true
        }
    },
    data() {
        return {
            timer: null,
            isDragging: false,
            isClosing: false,
            startX: 0,
            startY: 0,
            offsetX: 0,
            offsetY: 0,
            dragDirection: null,
            initialRect: null,
            internalStackOffset: 0
        };
    },
    computed: {
        positionClass() {
            return `toast-${this.position}`;
        },
        typeClass() {
            return `toast-${this.type}`;
        },
        transitionName() {
            const positionMap = {
                'top-left': 'toast-slide-in-top-left',
                'top-center': 'toast-slide-in-top',
                'top-right': 'toast-slide-in-top-right',
                'bottom-left': 'toast-slide-in-bottom-left',
                'bottom-center': 'toast-slide-in-bottom',
                'bottom-right': 'toast-slide-in-right',
                'left-top': 'toast-slide-in-left',
                'left-center': 'toast-slide-in-left',
                'left-bottom': 'toast-slide-in-bottom-left',
                'right-top': 'toast-slide-in-right',
                'right-center': 'toast-slide-in-right',
                'right-bottom': 'toast-slide-in-right'
            };
            return positionMap[this.position] || 'toast-slide-in-right';
        },
        toastStyle() {
            const styles = {};

            if (this.shadow) {
                styles.boxShadow = '0 10px 36px rgba(0, 0, 0, 0.2)';
            }

            if (this.glassEffect) {
                styles.background = 'color-mix(in srgb, var(--bg-primary) 90%, transparent)';
                styles.backdropFilter = 'blur(14px)';
                styles.webkitBackdropFilter = 'blur(14px)';
            } else {
                styles.background = 'color-mix(in srgb, var(--bg-primary) 90%, transparent)';
            }

            if (this.isDragging) {
                styles.transform = `translate(${this.offsetX}px, ${this.offsetY}px)`;
                styles.transition = 'none';
            }

            // 应用堆叠偏移量
            if (this.internalStackOffset > 0) {
                if (this.position.includes('bottom')) {
                    styles.marginBottom = `${this.internalStackOffset}px`;
                } else if (this.position.includes('top')) {
                    styles.marginTop = `${this.internalStackOffset}px`;
                }
            }

            return styles;
        }
    },
    mounted() {
        this.internalStackOffset = this.stackOffset;
        this.startTimer();
    },
    methods: {
        startTimer() {
            this.clearTimer();
            if (this.duration > 0) {
                let dynamicDuration = this.duration;
                const contentLength = (this.title + this.message + this.subtitle).length;

                if (contentLength > 100) {
                    dynamicDuration += 2000;
                } else if (contentLength > 50) {
                    dynamicDuration += 1000;
                }

                this.timer = setTimeout(() => {
                    this.close();
                }, dynamicDuration);
            }
        },
        clearTimer() {
            if (this.timer) {
                clearTimeout(this.timer);
                this.timer = null;
            }
        },
        close() {
            this.isClosing = true;
        },
        updateStackOffset(newOffset) {
            this.internalStackOffset = newOffset;
        },
        onMouseEnter() {
            this.clearTimer();
        },
        onMouseLeave() {
            this.startTimer();
        },
        startDrag(event) {
            if (!event.target.closest('.toast-close')) {
                this.isDragging = true;
                this.startX = event.clientX;
                this.startY = event.clientY;
                this.dragDirection = null;
                this.clearTimer();

                const rect = this.$el.getBoundingClientRect();
                this.initialRect = { left: rect.left, top: rect.top, right: rect.right, bottom: rect.bottom };

                document.addEventListener('mousemove', this.onDrag);
                document.addEventListener('mouseup', this.endDrag);
            }
        },
        onDrag(event) {
            if (this.isDragging) {
                const deltaX = event.clientX - this.startX;
                const deltaY = event.clientY - this.startY;

                if (!this.dragDirection) {
                    if (Math.abs(deltaX) > Math.abs(deltaY) && Math.abs(deltaX) > 5) {
                        this.dragDirection = 'horizontal';
                    } else if (Math.abs(deltaY) > Math.abs(deltaX) && Math.abs(deltaY) > 5) {
                        this.dragDirection = 'vertical';
                    }
                }

                if (this.dragDirection === 'horizontal') {
                    if (this.position.includes('right')) {
                        this.offsetX = Math.max(0, deltaX);
                    } else if (this.position.includes('left')) {
                        this.offsetX = Math.min(0, deltaX);
                    } else {
                        this.offsetX = deltaX;
                    }
                    this.offsetY = 0;
                } else if (this.dragDirection === 'vertical') {
                    if (this.position.includes('bottom')) {
                        this.offsetY = Math.max(0, deltaY);
                    } else if (this.position.includes('top')) {
                        this.offsetY = Math.min(0, deltaY);
                    } else {
                        this.offsetY = deltaY;
                    }
                    this.offsetX = 0;
                }
            }
        },
        endDrag() {
            if (this.isDragging) {
                document.removeEventListener('mousemove', this.onDrag);
                document.removeEventListener('mouseup', this.endDrag);

                const rect = this.$el.getBoundingClientRect();
                const screenWidth = window.innerWidth;
                const screenHeight = window.innerHeight;

                const overflowLeft = Math.max(0, -rect.left);
                const overflowRight = Math.max(0, rect.right - screenWidth);
                const overflowTop = Math.max(0, -rect.top);
                const overflowBottom = Math.max(0, rect.bottom - screenHeight);

                const toastWidth = rect.width;
                const toastHeight = rect.height;
                const thirdWidth = toastWidth / 3;
                const thirdHeight = toastHeight / 3;

                const isOverflowing =
                    overflowLeft >= thirdWidth ||
                    overflowRight >= thirdWidth ||
                    overflowTop >= thirdHeight ||
                    overflowBottom >= thirdHeight;

                if (isOverflowing) {
                    this.close();
                } else {
                    this.isDragging = false;
                    requestAnimationFrame(() => {
                        this.offsetX = 0;
                        this.offsetY = 0;
                        this.dragDirection = null;
                        this.startTimer();
                    });
                }
            }
        }
    },
    beforeUnmount() {
        this.clearTimer();
        document.removeEventListener('mousemove', this.onDrag);
        document.removeEventListener('mouseup', this.endDrag);
    }
};
</script>

<style scoped>
.base-toast {
    position: fixed;
    max-width: 320px;
    min-width: 280px;
    padding: 16px;
    border-radius: 8px;
    border: 1px solid var(--border-color);
    z-index: 9999;
    display: flex;
    align-items: center;
    gap: 12px;
    animation-duration: 0.5s;
    animation-timing-function: cubic-bezier(0.25, 0.1, 0.25, 1);
    user-select: none;
    cursor: default;
    transition: all 0.3s cubic-bezier(0.25, 0.1, 0.25, 1);
}

.base-toast:hover {
    transform: translateY(-2px);
    box-shadow: 0 12px 40px rgba(0, 0, 0, 0.2);
}

.base-toast.dragging {
    cursor: grabbing;
    box-shadow: 0 16px 48px rgba(0, 0, 0, 0.25);
}

.toast-content {
    display: flex;
    align-items: flex-start;
    gap: 12px;
    flex: 1;
}

.toast-icon {
    font-size: 20px;
    flex-shrink: 0;
    margin-top: 2px;
    transition: transform 0.2s ease;
}

.base-toast:hover .toast-icon {
    transform: scale(1.1);
}

.toast-body {
    flex: 1;
    min-width: 0;
    user-select: none;
}

.toast-body.selectable {
    user-select: text;
}

.toast-title {
    font-size: 14px;
    font-weight: 600;
    color: var(--text-primary);
    margin: 0 0 4px 0;
    line-height: 1.4;
    transition: color 0.2s ease;
}

.toast-message {
    font-size: 13px;
    color: var(--text-secondary);
    margin: 0 0 4px 0;
    line-height: 1.4;
    word-break: break-word;
}

.toast-subtitle {
    font-size: 12px;
    color: var(--text-muted);
    margin: 0;
    line-height: 1.4;
    word-break: break-word;
}

.toast-close {
    background: none;
    border: none;
    font-size: 18px;
    color: var(--text-muted);
    cursor: pointer;
    padding: 0;
    width: 20px;
    height: 20px;
    display: flex;
    align-items: center;
    justify-content: center;
    border-radius: 4px;
    flex-shrink: 0;
    margin-top: 2px;
    user-select: none;
    transition: all 0.2s ease;
}

.toast-close:hover {
    background: var(--bg-secondary);
    color: var(--text-primary);
    transform: rotate(90deg);
}

.toast-top-left {
    top: 20px;
    left: 20px;
}

.toast-top-center {
    top: 20px;
    left: 50%;
    transform: translateX(-50%);
}

.toast-top-right {
    top: 20px;
    right: 20px;
}

.toast-bottom-left {
    bottom: 20px;
    left: 20px;
}

.toast-bottom-center {
    bottom: 20px;
    left: 50%;
    transform: translateX(-50%);
}

.toast-bottom-right {
    bottom: 20px;
    right: 20px;
}

.toast-left-top {
    top: 20px;
    left: 20px;
}

.toast-left-center {
    top: 50%;
    left: 20px;
    transform: translateY(-50%);
}

.toast-left-bottom {
    bottom: 20px;
    left: 20px;
}

.toast-right-top {
    top: 20px;
    right: 20px;
}

.toast-right-center {
    top: 50%;
    right: 20px;
    transform: translateY(-50%);
}

.toast-right-bottom {
    bottom: 20px;
    right: 20px;
}

.toast-info .toast-icon {
    color: var(--accent-color);
}

.toast-success .toast-icon {
    color: #10b981;
}

.toast-warning .toast-icon {
    color: #f59e0b;
}

.toast-error .toast-icon {
    color: #ef4444;
}

.toast-slide-in-right-enter-active,
.toast-slide-in-right-leave-active {
    transition: all 0.5s cubic-bezier(0.25, 0.1, 0.25, 1);
}

.toast-slide-in-right-enter-from {
    transform: translateX(100%);
    opacity: 0;
}

.toast-slide-in-right-leave-to {
    transform: translateX(100%);
    opacity: 0;
    transition: all 0.5s cubic-bezier(0.25, 0.1, 0.25, 1);
}

.toast-slide-in-left-enter-active,
.toast-slide-in-left-leave-active {
    transition: all 0.5s cubic-bezier(0.25, 0.1, 0.25, 1);
}

.toast-slide-in-left-enter-from {
    transform: translateX(-100%);
    opacity: 0;
}

.toast-slide-in-left-leave-to {
    transform: translateX(-100%);
    opacity: 0;
    transition: all 0.5s cubic-bezier(0.25, 0.1, 0.25, 1);
}

.toast-slide-in-top-enter-active,
.toast-slide-in-top-leave-active {
    transition: all 0.5s cubic-bezier(0.25, 0.1, 0.25, 1);
}

.toast-slide-in-top-enter-from {
    transform: translateY(-100%);
    opacity: 0;
}

.toast-slide-in-top-leave-to {
    transform: translateY(-100%);
    opacity: 0;
    transition: all 0.5s cubic-bezier(0.25, 0.1, 0.25, 1);
}

.toast-slide-in-bottom-enter-active,
.toast-slide-in-bottom-leave-active {
    transition: all 0.5s cubic-bezier(0.25, 0.1, 0.25, 1);
}

.toast-slide-in-bottom-enter-from {
    transform: translateY(100%);
    opacity: 0;
}

.toast-slide-in-bottom-leave-to {
    transform: translateY(100%);
    opacity: 0;
    transition: all 0.5s cubic-bezier(0.25, 0.1, 0.25, 1);
}

.toast-slide-in-top-left-enter-active,
.toast-slide-in-top-left-leave-active {
    transition: all 0.5s cubic-bezier(0.25, 0.1, 0.25, 1);
}

.toast-slide-in-top-left-enter-from {
    transform: translate(-100%, -100%);
    opacity: 0;
}

.toast-slide-in-top-left-leave-to {
    transform: translate(-100%, -100%);
    opacity: 0;
    transition: all 0.5s cubic-bezier(0.25, 0.1, 0.25, 1);
}

.toast-slide-in-top-right-enter-active,
.toast-slide-in-top-right-leave-active {
    transition: all 0.5s cubic-bezier(0.25, 0.1, 0.25, 1);
}

.toast-slide-in-top-right-enter-from {
    transform: translate(100%, -100%);
    opacity: 0;
}

.toast-slide-in-top-right-leave-to {
    transform: translate(100%, -100%);
    opacity: 0;
    transition: all 0.5s cubic-bezier(0.25, 0.1, 0.25, 1);
}

.toast-slide-in-bottom-left-enter-active,
.toast-slide-in-bottom-left-leave-active {
    transition: all 0.5s cubic-bezier(0.25, 0.1, 0.25, 1);
}

.toast-slide-in-bottom-left-enter-from {
    transform: translate(-100%, 100%);
    opacity: 0;
}

.toast-slide-in-bottom-left-leave-to {
    transform: translate(-100%, 100%);
    opacity: 0;
    transition: all 0.5s cubic-bezier(0.25, 0.1, 0.25, 1);
}

.toast-slide-in-bottom-right-enter-active,
.toast-slide-in-bottom-right-leave-active {
    transition: all 0.5s cubic-bezier(0.25, 0.1, 0.25, 1);
}

.toast-slide-in-bottom-right-enter-from {
    transform: translate(100%, 100%);
    opacity: 0;
}

.toast-slide-in-bottom-right-leave-to {
    transform: translate(100%, 100%);
    opacity: 0;
    transition: all 0.5s cubic-bezier(0.25, 0.1, 0.25, 1);
}
</style>
