/**
 * 工具配置
 */
export const TOOL_CONFIG = {
  // 工具存储键
  SELECTED_TOOL_KEY: 'mono-selected-tool',
  // 工具列表
  TOOLS: [
    {
      id: 'inspiration',
      name: '灵感碎片',
      description: '捕捉转瞬即逝的想法',
      category: '个人生活'
    },
    {
      id: 'work-notes',
      name: '工作手记',
      description: '记录工作中的点滴',
      category: '个人生活'
    },
    {
      id: 'musings',
      name: '随想录',
      description: '随性的思考与感悟',
      category: '个人生活'
    },
    {
      id: 'password-generator',
      name: '密码生成器',
      description: '生成安全的随机密码',
      category: '实用工具'
    },
    {
      id: 'unit-converter',
      name: '单位转换',
      description: '各种单位之间的转换',
      category: '实用工具'
    },
    {
      id: 'time-tools',
      name: '时间工具',
      description: '时间转换和计算',
      category: '实用工具'
    },
    {
      id: 'file-hasher',
      name: '文件哈希',
      description: '计算文件的哈希值',
      category: '实用工具'
    },
    {
      id: 'desktop-icon',
      name: '桌面图标管理',
      description: '查看、重命名与组织桌面图标',
      category: '系统工具'
    },
    {
      id: 'file-search',
      name: '系统文件搜索',
      description: '按关键字快速定位文件',
      category: '系统工具'
    },
    {
      id: 'system-info',
      name: '系统信息',
      description: '查看系统硬件和软件信息',
      category: '系统工具'
    },
    {
      id: 'network-tools',
      name: '网络工具',
      description: '网络连接测试和管理',
      category: '系统工具'
    },
    {
      id: 'process-manager',
      name: '进程管理',
      description: '查看和管理系统进程',
      category: '系统工具'
    },
    {
      id: 'guid-generator',
      name: 'GUID 生成器',
      description: '生成并批量复制唯一标识符',
      category: '开发工具'
    },
    {
      id: 'json-formatter',
      name: 'JSON 格式化',
      description: '格式化和验证 JSON 数据',
      category: '开发工具'
    },
    {
      id: 'base64-encoder',
      name: 'Base64 工具',
      description: 'Base64 编码和解码',
      category: '开发工具'
    },
    {
      id: 'regex-tester',
      name: '正则表达式测试',
      description: '测试和验证正则表达式',
      category: '开发工具'
    },
    {
      id: 'image-converter',
      name: '图片转换',
      description: '进行格式转换与质量压缩',
      category: '媒体工具'
    },
    {
      id: 'video-converter',
      name: '视频转换',
      description: '视频格式转换和处理',
      category: '媒体工具'
    },
    {
      id: 'audio-tools',
      name: '音频工具',
      description: '音频格式转换和编辑',
      category: '媒体工具'
    },
    {
      id: 'screen-capture',
      name: '屏幕截图',
      description: '截取屏幕和编辑图片',
      category: '媒体工具'
    },
    {
      id: 'japanese-learning',
      name: '日语学习',
      description: '学习日语五十音图和常用单词',
      category: '学习'
    }
  ],
  // 设置入口
  SETTINGS_ENTRY: {
    id: 'settings',
    name: '设置',
    description: '主题与应用偏好'
  }
};