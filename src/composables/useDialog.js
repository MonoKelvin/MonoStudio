import { ref } from 'vue';

export function useDialog() {
  const dialogVisible = ref(false);
  const dialogType = ref('info');
  const dialogTitle = ref('提示');
  const dialogMessage = ref('');
  const dialogConfirmText = ref('确定');
  const dialogCancelText = ref('取消');
  const dialogShowCancel = ref(false);
  const dialogCloseOnOverlay = ref(true);
  const dialogEnableSound = ref(true);
  const resolvePromise = ref(null);
  const rejectPromise = ref(null);

  const showDialog = (options) => {
    return new Promise((resolve, reject) => {
      dialogType.value = options.type || 'info';
      dialogTitle.value = options.title || '提示';
      dialogMessage.value = options.message || '';
      dialogConfirmText.value = options.confirmText || '确定';
      dialogCancelText.value = options.cancelText || '取消';
      dialogShowCancel.value = options.showCancel || false;
      dialogCloseOnOverlay.value = options.closeOnOverlay !== false;
      dialogEnableSound.value = options.enableSound !== false;
      resolvePromise.value = resolve;
      rejectPromise.value = reject;
      dialogVisible.value = true;
    });
  };

  const handleDialogConfirm = () => {
    if (resolvePromise.value) {
      resolvePromise.value(true);
    }
    dialogVisible.value = false;
  };

  const handleDialogCancel = () => {
    if (rejectPromise.value) {
      rejectPromise.value(false);
    }
    dialogVisible.value = false;
  };

  const alert = (message, title = '提示', type = 'info') => {
    return showDialog({
      type,
      title,
      message,
      showCancel: false
    });
  };

  const confirm = (message, title = '确认', type = 'warning') => {
    return showDialog({
      type,
      title,
      message,
      showCancel: true
    });
  };

  return {
    dialogVisible,
    dialogType,
    dialogTitle,
    dialogMessage,
    dialogConfirmText,
    dialogCancelText,
    dialogShowCancel,
    dialogCloseOnOverlay,
    dialogEnableSound,
    handleDialogConfirm,
    handleDialogCancel,
    showDialog,
    alert,
    confirm
  };
}
