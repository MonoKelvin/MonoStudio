import { ref } from 'vue';

export const tooltipState = {
  visible: ref(false),
  content: ref(''),
  placement: ref('top'),
  x: ref(0),
  y: ref(0)
};

let showTimer = null;
let hideTimer = null;
const SHOW_DELAY = 400;
const HIDE_DELAY = 200;

let pendingTooltip = null;

function showTooltip(text, placement = 'top', x = 0, y = 0) {
  if (x === 0 && y === 0) return;

  if (hideTimer) {
    clearTimeout(hideTimer);
    hideTimer = null;
  }

  tooltipState.content.value = text;
  tooltipState.placement.value = placement;
  tooltipState.x.value = x;
  tooltipState.y.value = y;
  tooltipState.visible.value = true;
}

function hideTooltip() {
  if (showTimer) {
    clearTimeout(showTimer);
    showTimer = null;
  }
  if (hideTimer) {
    clearTimeout(hideTimer);
    hideTimer = null;
  }
  tooltipState.visible.value = false;
  tooltipState.x.value = 0;
  tooltipState.y.value = 0;
}

function scheduleShow() {
  if (showTimer) {
    clearTimeout(showTimer);
  }

  showTimer = setTimeout(() => {
    if (pendingTooltip) {
      showTooltip(pendingTooltip.text, pendingTooltip.placement, pendingTooltip.x, pendingTooltip.y);
    }
    showTimer = null;
  }, SHOW_DELAY);
}

function scheduleHide() {
  if (hideTimer) {
    clearTimeout(hideTimer);
  }

  hideTimer = setTimeout(() => {
    hideTooltip();
    hideTimer = null;
  }, HIDE_DELAY);
}

window.showTooltip = showTooltip;
window.hideTooltip = hideTooltip;

export const vTooltip = {
  mounted(el, binding) {
    const text = binding.value;
    if (!text) return;

    el.addEventListener('mouseenter', (e) => {
      pendingTooltip = { text, placement: binding.arg || 'top', x: e.clientX, y: e.clientY };
      scheduleShow();
    });

    el.addEventListener('mouseleave', () => {
      pendingTooltip = null;
      hideTooltip();
    });

    el.addEventListener('focus', (e) => {
      showTooltip(text, binding.arg || 'top', e.clientX, e.clientY);
    });

    el.addEventListener('blur', () => {
      hideTooltip();
    });

    el.addEventListener('mousemove', (e) => {
      if (pendingTooltip) {
        pendingTooltip.x = e.clientX;
        pendingTooltip.y = e.clientY;
      }
    });
  },
  updated(el, binding) {
    if (!binding.value) {
      pendingTooltip = null;
      hideTooltip();
    }
  },
  unmounted() {
    pendingTooltip = null;
    hideTooltip();
  }
};

export { showTooltip, hideTooltip };
