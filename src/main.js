import { createApp } from 'vue'
import './style.css'
import App from './App.vue'
import BaseButton from './components/base/BaseButton.vue'
import BaseInput from './components/base/BaseInput.vue'
import BasePanel from './components/base/BasePanel.vue'
import BaseCheckbox from './components/base/BaseCheckbox.vue'
import BaseSelect from './components/base/BaseSelect.vue'
import BaseSpinbox from './components/base/BaseSpinbox.vue'
import GlobalTooltip from './components/base/GlobalTooltip.vue'
import { vTooltip } from './directives/tooltip.js'

function showRuntimeErrorOverlay(title, detail) {
  const existing = document.getElementById('mono-runtime-error')
  if (existing) existing.remove()
  const overlay = document.createElement('div')
  overlay.id = 'mono-runtime-error'
  overlay.style.cssText = [
    'position:fixed',
    'inset:0',
    'z-index:99999',
    'background:#0b1020',
    'color:#f8fafc',
    'font-family:Consolas,monospace',
    'padding:24px',
    'overflow:auto'
  ].join(';')
  overlay.innerHTML = `<h2 style="margin:0 0 12px;">${title}</h2><pre style="white-space:pre-wrap;line-height:1.5;">${detail}</pre>`
  document.body.appendChild(overlay)
}

window.addEventListener('error', (event) => {
  showRuntimeErrorOverlay('渲染层错误（window.error）', `${event.message}\n${event.filename}:${event.lineno}:${event.colno}`)
})

window.addEventListener('unhandledrejection', (event) => {
  const reason = event.reason instanceof Error ? event.reason.stack || event.reason.message : String(event.reason)
  showRuntimeErrorOverlay('渲染层错误（unhandledrejection）', reason)
})

const app = createApp(App)
app.component('BaseButton', BaseButton)
app.component('BaseInput', BaseInput)
app.component('BasePanel', BasePanel)
app.component('BaseCheckbox', BaseCheckbox)
app.component('BaseSelect', BaseSelect)
app.component('BaseSpinbox', BaseSpinbox)
app.component('GlobalTooltip', GlobalTooltip)
app.directive('tooltip', vTooltip)
app.config.errorHandler = (err, instance, info) => {
  const stack = err instanceof Error ? err.stack || err.message : String(err)
  const componentName = instance?.$options?.name || 'UnknownComponent'
  showRuntimeErrorOverlay('Vue 运行时错误', `${stack}\n\ncomponent: ${componentName}\ninfo: ${info}`)
  console.error('[vue-error-handler]', err, { componentName, info })
}

app.mount('#app')