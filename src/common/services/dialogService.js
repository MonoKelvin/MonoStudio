import { createApp } from 'vue';
import BasePromptDialog from '../../components/base/BasePromptDialog.vue';

class DialogService {
  constructor() {
    this.dialogContainer = null;
    this.currentApp = null;
    this.currentInstance = null;
  }

  init() {
    if (!this.dialogContainer) {
      this.dialogContainer = document.createElement('div');
      this.dialogContainer.id = 'dialog-container';
      this.dialogContainer.style.position = 'fixed';
      this.dialogContainer.style.top = '0';
      this.dialogContainer.style.left = '0';
      this.dialogContainer.style.width = '100%';
      this.dialogContainer.style.height = '100%';
      this.dialogContainer.style.zIndex = '100000';
      this.dialogContainer.style.pointerEvents = 'none';
      document.body.appendChild(this.dialogContainer);
    }
  }

  show(options = {}) {
    this.init();

    const dialogOptions = {
      visible: true,
      type: options.type || 'info',
      title: options.title || '提示',
      message: options.message || '',
      confirmText: options.confirmText || '确定',
      cancelText: options.cancelText || '取消',
      showCancel: options.showCancel !== undefined ? options.showCancel : false,
      closeOnOverlay: options.closeOnOverlay !== undefined ? options.closeOnOverlay : true,
      enableSound: options.enableSound !== undefined ? options.enableSound : true,
      ...options
    };

    if (this.currentApp) {
      this.close();
    }

    const app = createApp(BasePromptDialog, {
      ...dialogOptions,
      'onUpdate:visible': (val) => {
        if (!val) {
          this.close();
        }
      },
      onConfirm: () => {
        if (dialogOptions.onConfirm) {
          dialogOptions.onConfirm();
        }
        this.close();
      },
      onCancel: () => {
        if (dialogOptions.onCancel) {
          dialogOptions.onCancel();
        }
        this.close();
      }
    });

    const mountEl = document.createElement('div');
    mountEl.style.pointerEvents = 'auto';
    this.dialogContainer.appendChild(mountEl);

    this.currentApp = app;
    this.currentInstance = app.mount(mountEl);

    return this;
  }

  close() {
    if (this.currentApp) {
      this.currentApp.unmount();
      this.currentApp = null;
      this.currentInstance = null;
      if (this.dialogContainer) {
        this.dialogContainer.innerHTML = '';
      }
    }
  }

  alert(message, title = '提示', type = 'info') {
    return this.show({
      type,
      title,
      message,
      showCancel: false
    });
  }

  confirm(message, title = '确认', type = 'warning') {
    return this.show({
      type,
      title,
      message,
      showCancel: true
    });
  }

  success(message, title = '成功') {
    return this.alert(message, title, 'info');
  }

  error(message, title = '错误') {
    return this.alert(message, title, 'error');
  }

  warning(message, title = '警告') {
    return this.alert(message, title, 'warning');
  }
}

const dialogService = new DialogService();

export default {
  install(app) {
    app.config.globalProperties.$dialog = dialogService;
    app.provide('dialog', dialogService);
  }
};

export { dialogService };
