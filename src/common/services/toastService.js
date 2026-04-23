import { createApp } from 'vue';
import BaseToast from '../../components/base/BaseToast.vue';

class ToastService {
  constructor() {
    this.toastContainer = null;
    this.toastInstances = [];
    this.nextToastId = 1;
  }

  // 初始化 toast 容器
  init() {
    if (!this.toastContainer) {
      // 创建容器元素
      this.toastContainer = document.createElement('div');
      this.toastContainer.id = 'toast-container';
      this.toastContainer.style.position = 'fixed';
      this.toastContainer.style.top = '0';
      this.toastContainer.style.left = '0';
      this.toastContainer.style.width = '100%';
      this.toastContainer.style.height = '100%';
      this.toastContainer.style.zIndex = '99999';
      this.toastContainer.style.pointerEvents = 'none';

      // 添加到根元素
      document.body.appendChild(this.toastContainer);
    }
  }

  // 显示 toast
  show(options) {
    this.init();

    const toastId = this.nextToastId++;
    const toastOptions = {
      id: toastId,
      visible: true,
      ...options,
      duration: options.duration || 5000
    };

    // 创建 Vue 应用实例
    const app = createApp(BaseToast, toastOptions);

    // 创建挂载元素
    const mountEl = document.createElement('div');
    mountEl.style.pointerEvents = 'auto';
    this.toastContainer.appendChild(mountEl);

    // 挂载应用
    const instance = app.mount(mountEl);

    // 存储实例
    this.toastInstances.push({
      id: toastId,
      instance,
      mountEl
    });

    // 监听关闭事件 - Vue 3 方式
    const originalClose = instance.close;
    instance.close = () => {
      originalClose();
      this.close(toastId);
    };

    return toastId;
  }

  // 关闭 toast
  close(toastId) {
    const index = this.toastInstances.findIndex(instance => instance.id === toastId);
    if (index !== -1) {
      const { instance, mountEl } = this.toastInstances[index];

      // 移除元素
      if (mountEl && mountEl.parentNode) {
        mountEl.parentNode.removeChild(mountEl);
      }

      // 销毁实例 - Vue 3 方式
      instance.$el = null;

      // 从数组中移除
      this.toastInstances.splice(index, 1);
    }
  }

  // 关闭所有 toast
  closeAll() {
    this.toastInstances.forEach(({ id }) => {
      this.close(id);
    });
  }

  // 快捷方法
  success(message, options = {}) {
    return this.show({ type: 'success', message, ...options });
  }

  error(message, options = {}) {
    return this.show({ type: 'error', message, ...options });
  }

  warning(message, options = {}) {
    return this.show({ type: 'warning', message, ...options });
  }

  info(message, options = {}) {
    return this.show({ type: 'info', message, ...options });
  }
}

// 创建单例实例
const toastService = new ToastService();

// 注册为 Vue 插件
export default {
  install(app) {
    app.config.globalProperties.$toast = toastService;
    app.provide('toast', toastService);
  }
};

export { toastService };
