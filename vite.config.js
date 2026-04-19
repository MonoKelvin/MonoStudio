const { defineConfig } = require('vite')
const vue = require('@vitejs/plugin-vue')

module.exports = defineConfig({
  base: './',
  plugins: [vue()]
})
